#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace juce;

namespace
{
inline float fracd (double v) { return (float) (v - std::floor (v)); }
inline float tri (float f)    { return 1.0f - 4.0f * std::abs (f - 0.5f); }
}

DriftProcessor::DriftProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput ("Input", AudioChannelSet::stereo(), true)
                          .withOutput ("Output", AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", dl::createLayout())
{
    pX      = apvts.getRawParameterValue ("x");
    pY      = apvts.getRawParameterValue ("y");
    pGlide  = apvts.getRawParameterValue ("glide");
    pMotion = apvts.getRawParameterValue ("motion");
    pCycle  = apvts.getRawParameterValue ("mcycle");
    pMSize  = apvts.getRawParameterValue ("msize");
    pMix    = apvts.getRawParameterValue ("mix");

    for (int i = 0; i < 2; ++i)
    {
        const String p = "lfo" + String (i + 1);
        lT[i] = apvts.getRawParameterValue (p + "_target");
        lS[i] = apvts.getRawParameterValue (p + "_shape");
        lR[i] = apvts.getRawParameterValue (p + "_rate");
        lD[i] = apvts.getRawParameterValue (p + "_depth");
    }

    for (int c = 0; c < 4; ++c)
        for (int k = 0; k < 12; ++k)
            corner[c][k] = apvts.getRawParameterValue (dl::sid (c, k));
}

bool DriftProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    const auto out = l.getMainOutputChannelSet();
    if (out != AudioChannelSet::mono() && out != AudioChannelSet::stereo())
        return false;
    return l.getMainInputChannelSet() == out;
}

void DriftProcessor::prepareToPlay (double sampleRate, int)
{
    sr = sampleRate;
    hlen = jmax (1024, (int) (sr * 4.0));
    hist.setSize (2, hlen);
    hist.clear();
    hw = 0;

    flen = (int) (sr * 0.02) + 4;
    fbuf.setSize (2, flen);
    fbuf.clear();
    fw = 0;

    for (auto& g : grains)
        g.on = false;
    untilNext = 0.0;

    for (auto* s : { &sClean, &sGrain, &sPanL, &sPanR, &sFDry, &sFWet, &sFfb, &sFdep, &sMix })
        s->reset (sr, 0.03);

    sClean.setCurrentAndTargetValue (1.0f);
    sGrain.setCurrentAndTargetValue (0.0f);
    sPanL.setCurrentAndTargetValue (1.0f);
    sPanR.setCurrentAndTargetValue (1.0f);
    sFDry.setCurrentAndTargetValue (1.0f);
    sFWet.setCurrentAndTargetValue (0.0f);
    sFfb.setCurrentAndTargetValue (0.0f);
    sFdep.setCurrentAndTargetValue (0.0001f);
    sMix.setCurrentAndTargetValue (1.0f);
}

float DriftProcessor::readHist (const float* h, double pos) const
{
    pos = std::fmod (pos, (double) hlen);
    if (pos < 0.0)
        pos += hlen;
    int i0 = (int) pos;
    if (i0 >= hlen)
        i0 = 0;
    const float f = (float) (pos - i0);
    int i1 = i0 + 1;
    if (i1 >= hlen)
        i1 = 0;
    return h[i0] + (h[i1] - h[i0]) * f;
}

float DriftProcessor::readFl (const float* b, float d) const
{
    float p = (float) fw - d;
    while (p < 0.0f)
        p += (float) flen;
    int i0 = ((int) p) % flen;
    const float f = p - std::floor (p);
    const int i1 = (i0 + 1) % flen;
    return b[i0] + (b[i1] - b[i0]) * f;
}

void DriftProcessor::spawn (double rate, int glen, float spraySmp, float rev, float gpan, float amp)
{
    for (auto& g : grains)
    {
        if (g.on)
            continue;

        const bool reverse = rng.nextFloat() < rev;
        // forward grains faster than 1x must start far enough back not to overtake the write head
        const double minD = reverse ? 2.0 : jmax (0.0, (rate - 1.0) * glen) + 2.0;
        double d = minD + rng.nextFloat() * spraySmp;
        d = jmin (d, (double) hlen - glen * (rate + 1.0) - 8.0);
        d = jmax (d, 2.0);

        g.pos = (double) hw - d;
        if (g.pos < 0.0)
            g.pos += hlen;
        g.inc = reverse ? -rate : rate;
        g.len = glen;
        g.age = 0;
        const float p = (rng.nextFloat() * 2.0f - 1.0f) * gpan;
        g.gl = jmin (1.0f, 1.0f - p);
        g.gr = jmin (1.0f, 1.0f + p);
        g.amp = amp;
        g.on = true;
        return;
    }
}

void DriftProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;
    const int n = buffer.getNumSamples();
    const int nch = buffer.getNumChannels();
    for (int c = getTotalNumInputChannels(); c < nch; ++c)
        buffer.clear (c, 0, n);
    if (n == 0 || nch == 0)
        return;

    // ---- tempo: follow the host when it plays, otherwise keep a free-running clock
    double bpm = 120.0, ppq = ppqFree;
    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
        {
            if (auto b = pos->getBpm())
                bpm = jlimit (20.0, 400.0, *b);
            if (pos->getIsPlaying())
                if (auto p = pos->getPpqPosition())
                    ppq = *p;
        }
    const double dtSec = n / sr;
    ppqFree = ppq + dtSec * bpm / 60.0;
    sweep.store (fracd (ppq / 4.0));

    // ---- where the puck wants to be
    float tx = pX->load(), ty = pY->load();
    const int motion = (int) pMotion->load();
    if (motion > 0 && ! touching.load())
    {
        const double phs = ppq / dl::cycleBeats[jlimit (0, 3, (int) pCycle->load())];
        const float f = fracd (phs), r = pMSize->load() * 0.5f, T = MathConstants<float>::twoPi * f;
        tx = ty = 0.5f;
        switch (motion)
        {
            case 1: tx += r * std::cos (T); ty += r * std::sin (T); break;
            case 2: tx += r * std::sin (T); ty += r * 0.8f * std::sin (2.0f * T); break;
            case 3: tx += r * tri (fracd (phs * 2.0)); ty += r * tri (fracd (phs * 3.1 + 0.25)); break;
            default:
            {
                const auto c = (long long) std::floor (phs * 8.0);
                if (c != lastRand)
                {
                    lastRand = c;
                    rtx = 0.5f + (rng.nextFloat() * 2.0f - 1.0f) * r;
                    rty = 0.5f + (rng.nextFloat() * 2.0f - 1.0f) * r;
                }
                tx = rtx; ty = rty;
            }
        }
        tx = jlimit (0.0f, 1.0f, tx);
        ty = jlimit (0.0f, 1.0f, ty);
    }
    tgtX.store (tx);
    tgtY.store (ty);

    const float glide = pGlide->load();
    const float a = glide < 0.005f ? 1.0f : 1.0f - std::exp (-(float) dtSec / glide);
    x += (tx - x) * a;
    y += (ty - y) * a;
    curX.store (x);
    curY.store (y);

    // ---- morph the four corner scenes
    const float w[4] { (1 - x) * (1 - y), x * (1 - y), (1 - x) * y, x * y };
    float P[12];
    for (int k = 0; k < 12; ++k)
    {
        float v = 0.0f;
        for (int c = 0; c < 4; ++c)
            v += w[c] * corner[c][k]->load();
        P[k] = v;
    }

    // ---- LFOs on top
    for (int i = 0; i < 2; ++i)
    {
        const double phs = ppq / dl::rateBeats[jlimit (0, 6, (int) lR[i]->load())];
        const double cyc = std::floor (phs);
        const float f = (float) (phs - cyc);
        float v;
        switch ((int) lS[i]->load())
        {
            case 0:  v = std::sin (MathConstants<float>::twoPi * f); break;
            case 1:  v = tri (f); break;
            case 2:  v = f < 0.5f ? 1.0f : -1.0f; break;
            case 3:  v = 2.0f * f - 1.0f; break;
            default:
            {
                const double h = std::sin (cyc * 12.9898 + i * 78.233) * 43758.5453;
                v = (float) (h - std::floor (h)) * 2.0f - 1.0f;
            }
        }
        lfoVal[i].store (v);
        const float depth = lD[i]->load();
        if (std::abs (depth) > 0.0001f)
        {
            const int t = jlimit (0, 11, (int) lT[i]->load());
            auto& d = dl::scene[(size_t) t];
            P[t] = jlimit (d.min, d.max, P[t] + v * depth * (d.max - d.min));
        }
    }

    const float gm = P[0], pan = P[6], fmix = P[8], frate = P[9];
    sClean.setTargetValue (1.0f - gm);
    sGrain.setTargetValue (gm);
    sPanL.setTargetValue (jmin (1.0f, 1.0f - pan));
    sPanR.setTargetValue (jmin (1.0f, 1.0f + pan));
    sFDry.setTargetValue (1.0f - fmix * 0.35f);
    sFWet.setTargetValue (fmix * 0.85f);
    sFfb.setTargetValue (P[11]);
    sFdep.setTargetValue (0.0001f + P[10] * 0.0033f);
    sMix.setTargetValue (pMix->load());

    meterG.store (jmin (1.0f, gm * (0.4f + P[3] / 100.0f)));
    meterF.store (jmin (1.0f, fmix * (0.5f + P[10] * 0.5f)));
    meterP.store (jmin (1.0f, std::abs (pan) + P[7] * 0.6f));

    // ---- grain settings for this block
    const float sizeS = P[2] / 1000.0f;
    const int glen = jmax (16, (int) (sizeS * sr));
    const double rate = std::pow (2.0, std::round (P[1]) / 12.0);
    const float dens = jmax (1.0f, P[3]);
    const float spraySmp = P[4] / 1000.0f * (float) sr;
    const float amp = jmin (1.0f, 0.9f / std::sqrt (jmax (1.0f, dens * sizeS)));

    float* L = buffer.getWritePointer (0);
    float* R = nch > 1 ? buffer.getWritePointer (1) : nullptr;
    float* hL = hist.getWritePointer (0);
    float* hR = hist.getWritePointer (1);
    float* fL = fbuf.getWritePointer (0);
    float* fR = fbuf.getWritePointer (1);
    const float twoPi = MathConstants<float>::twoPi;

    for (int i = 0; i < n; ++i)
    {
        const float inL = L[i];
        const float inR = R != nullptr ? R[i] : inL;
        hL[hw] = inL;
        hR[hw] = inR;

        if (gm > 0.001f)
        {
            untilNext -= 1.0;
            if (untilNext <= 0.0)
            {
                spawn (rate, glen, spraySmp, P[5], P[7], amp);
                untilNext = sr * (0.7 + 0.6 * rng.nextFloat()) / dens;
            }
        }

        float gL = 0.0f, gR = 0.0f;
        for (auto& g : grains)
        {
            if (! g.on)
                continue;
            const float t = (float) g.age / (float) g.len;
            const float env = 0.5f - 0.5f * std::cos (twoPi * t) ;
            const float e = env * g.amp;
            gL += readHist (hL, g.pos) * e * g.gl;
            gR += readHist (hR, g.pos) * e * g.gr;
            g.pos += g.inc;
            if (++g.age >= g.len)
                g.on = false;
        }

        if (++hw >= hlen)
            hw = 0;

        const float cg = sClean.getNextValue(), gg = sGrain.getNextValue();
        float oL = inL * cg + gL * gg;
        float oR = inR * cg + gR * gg;

        // flanger (two short modulated delays, 90 degrees apart for width)
        const float dep = sFdep.getNextValue(), fb = sFfb.getNextValue();
        const float fd = sFDry.getNextValue(), fwet = sFWet.getNextValue();
        fph += frate / sr;
        if (fph >= 1.0)
            fph -= 1.0;
        const float dLs = (0.004f + dep * std::sin (twoPi * (float) fph)) * (float) sr;
        const float dRs = (0.004f + dep * std::sin (twoPi * (float) fph + twoPi * 0.25f)) * (float) sr;
        const float wl = readFl (fL, dLs), wr = readFl (fR, dRs);
        fL[fw] = oL + wl * fb;
        fR[fw] = oR + wr * fb;
        if (++fw >= flen)
            fw = 0;
        oL = oL * fd + wl * fwet;
        oR = oR * fd + wr * fwet;

        oL *= sPanL.getNextValue();
        oR *= sPanR.getNextValue();

        const float m = sMix.getNextValue();
        if (R != nullptr)
        {
            L[i] = inL * (1.0f - m) + oL * m;
            R[i] = inR * (1.0f - m) + oR * m;
        }
        else
        {
            L[i] = inL * (1.0f - m) + 0.5f * (oL + oR) * m;
        }
    }
}

void DriftProcessor::getStateInformation (MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, dest);
}

void DriftProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (ValueTree::fromXml (*xml));
}

AudioProcessorEditor* DriftProcessor::createEditor()
{
    return new DriftEditor (*this);
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DriftProcessor();
}
