#include "PluginEditor.h"

using namespace juce;
using namespace dlui;

static constexpr int kW = 980, kH = 860;
static constexpr float kTau = MathConstants<float>::twoPi;

Font dlui::mono (float h)
{
    return Font (FontOptions (Font::getDefaultMonospacedFontName(), h, Font::plain));
}

//==============================================================================
// Worn olive steel, generated fresh on every launch
// the maker's mark (traced from the hand-drawn "Debax"), scratched in with a nail
static const std::vector<std::vector<float>> kSig {
    { 99.8f, 0.2f, 95.0f, 2.3f, 87.3f, 6.3f, 85.1f, 7.2f, 83.4f, 8.5f, 82.3f, 8.5f, 81.9f, 7.9f, 80.5f, 7.5f, 79.9f, 6.8f, 79.0f, 6.2f, 75.2f, 4.8f, 70.6f, 3.5f, 67.4f, 3.2f, 65.3f, 2.3f, 63.0f, 2.2f },
    { 55.5f, 4.2f, 55.1f, 5.3f, 52.2f, 8.5f, 50.7f, 9.5f, 50.1f, 10.1f, 49.6f, 11.0f, 48.9f, 11.1f },
    { 37.5f, 5.2f, 37.7f, 5.1f, 38.9f, 5.9f, 39.9f, 5.9f, 41.0f, 6.4f, 43.2f, 7.9f, 45.9f, 9.2f, 45.9f, 9.6f, 45.7f, 10.0f, 42.8f, 11.0f, 41.9f, 11.5f, 41.4f, 11.4f, 40.2f, 9.5f, 39.7f, 8.0f, 39.1f, 6.9f, 38.9f, 5.9f },
    { 25.5f, 6.2f, 25.6f, 7.7f, 25.1f, 9.5f, 23.7f, 12.5f, 22.9f, 13.5f, 20.4f, 14.8f, 19.0f, 15.9f, 16.9f, 16.4f, 14.1f, 18.1f, 12.5f, 18.6f, 9.7f, 20.0f, 8.3f, 20.4f },
    { 77.0f, 8.2f, 73.2f, 8.3f, 70.3f, 8.7f, 69.8f, 9.2f, 65.6f, 8.7f, 65.0f, 8.6f, 64.4f, 8.1f, 60.0f, 7.6f, 55.9f, 5.6f, 55.3f, 5.0f },
    { 77.3f, 8.2f, 79.5f, 8.0f, 80.5f, 7.5f },
    { 12.3f, 8.3f, 12.7f, 9.2f, 12.4f, 9.6f, 10.3f, 9.6f, 9.1f, 9.3f, 8.4f, 9.3f, 8.1f, 9.5f, 6.8f, 9.1f, 4.0f, 8.7f, 0.1f, 8.6f },
    { 47.4f, 9.5f, 45.9f, 9.5f },
    { 38.4f, 9.7f, 37.7f, 9.5f, 35.7f, 9.5f, 34.6f, 8.4f, 30.0f, 6.7f, 26.9f, 6.1f, 25.6f, 6.3f },
    { 56.0f, 10.9f, 58.0f, 10.1f, 64.1f, 9.4f, 65.0f, 8.6f },
    { 94.1f, 11.6f, 91.4f, 10.7f, 91.1f, 10.4f, 90.1f, 10.0f, 86.9f, 9.4f, 84.1f, 8.6f, 83.9f, 8.3f },
    { 76.5f, 11.9f, 76.6f, 11.5f, 76.2f, 11.3f, 71.4f, 10.2f, 70.2f, 9.6f, 69.8f, 9.2f },
    { 58.5f, 12.7f, 56.8f, 12.3f, 53.0f, 12.2f, 52.3f, 11.4f, 50.2f, 11.3f, 49.6f, 11.0f },
    { 81.1f, 12.8f, 80.7f, 12.9f, 80.2f, 12.5f, 78.1f, 12.0f, 77.5f, 11.4f, 76.6f, 11.5f },
    { 36.2f, 15.0f, 35.8f, 14.6f, 36.2f, 14.1f, 38.2f, 12.9f, 40.9f, 11.7f, 41.3f, 11.4f },
    { 44.2f, 15.0f, 44.2f, 14.8f, 43.1f, 13.7f, 41.8f, 11.5f },
    { 15.3f, 15.8f, 14.6f, 14.1f, 13.5f, 12.5f, 13.2f, 9.9f, 12.7f, 9.4f },
    { 82.3f, 8.4f, 81.8f, 9.0f, 78.0f, 10.7f, 77.5f, 11.4f },
    { 35.4f, 9.3f, 35.0f, 9.7f, 28.6f, 10.9f, 26.2f, 11.8f, 25.6f, 12.4f, 24.0f, 12.4f, 23.8f, 12.3f },
    { 8.1f, 9.5f, 8.6f, 10.5f, 10.2f, 11.4f, 12.6f, 12.5f, 13.5f, 12.4f },
    { 13.5f, 9.5f, 16.6f, 9.5f, 20.0f, 10.3f, 21.7f, 10.8f, 23.8f, 12.1f },
    { 13.5f, 9.5f, 13.1f, 9.6f },
    { 52.9f, 12.1f, 52.4f, 12.6f, 49.6f, 12.9f, 46.6f, 13.5f, 45.0f, 14.1f, 44.2f, 14.8f },
    { 25.7f, 12.3f, 25.9f, 12.8f, 27.9f, 13.8f, 32.6f, 13.7f, 35.2f, 14.6f, 35.8f, 14.5f }
};

static void scratchSig (Graphics& g, float x, float y, float width, Random& r)
{
    Path p;
    for (auto& poly : kSig)
        for (size_t i = 0; i + 1 < poly.size(); i += 2)
        {
            const float px = poly[i] + (r.nextFloat() - 0.5f) * 0.4f, py = poly[i + 1] + (r.nextFloat() - 0.5f) * 0.4f;
            if (i == 0) p.startNewSubPath (px, py); else p.lineTo (px, py);
        }
    const auto t = AffineTransform::scale (width / 100.0f).rotated (-0.05f).translated (x, y);
    g.setColour (Colours::black.withAlpha (0.45f));
    g.strokePath (p, PathStrokeType (1.2f, PathStrokeType::curved, PathStrokeType::rounded), t.translated (0.6f, 0.7f));
    g.setColour (Colour (0xffe8e2c8).withAlpha (0.34f));
    g.strokePath (p, PathStrokeType (1.0f, PathStrokeType::curved, PathStrokeType::rounded), t);
}

static Image makeWear (int W, int H)
{
    Image im (Image::RGB, W, H, true);
    Random r;
    {
        Image::BitmapData bd (im, Image::BitmapData::writeOnly);
        for (int y = 0; y < H; ++y)
            for (int x = 0; x < W; ++x)
            {
                const float n = (r.nextFloat() - 0.5f) * 26.0f;
                auto c8 = [] (float v) { return (uint8) jlimit (0, 255, roundToInt (v)); };
                bd.setPixelColour (x, y, Colour (c8 (69 + n), c8 (74 + n), c8 (59 + n * 0.8f)));
            }
    }

    Graphics g (im);
    for (int i = 0; i < 700; ++i)
    {
        g.setColour (Colour (255, 250, 225).withAlpha (r.nextFloat() * 0.035f));
        const float y = r.nextFloat() * H;
        g.drawLine (0.0f, y, (float) W, y + (r.nextFloat() * 2.0f - 1.0f) * 10.0f, r.nextFloat() * 1.6f + 0.2f);
    }

    for (int i = 0; i < 55; ++i)
    {
        const float cx = r.nextFloat() * W, cy = r.nextFloat() * H, rad = 15.0f + r.nextFloat() * 130.0f, t = r.nextFloat();
        const Colour c0 = t < 0.45f ? Colour (126, 60, 22).withAlpha (0.42f)
                        : t < 0.8f  ? Colour (12, 12, 8).withAlpha (0.4f)
                                    : Colour (170, 165, 130).withAlpha (0.18f);
        ColourGradient cg (c0, cx, cy, c0.withAlpha (0.0f), cx + rad, cy, true);
        Path blob;
        for (int k = 0; k <= 14; ++k)
        {
            const float a = (float) k / 14.0f * kTau, rr = rad * (0.6f + r.nextFloat() * 0.4f);
            const Point<float> p (cx + std::cos (a) * rr, cy + std::sin (a) * rr);
            if (k == 0) blob.startNewSubPath (p); else blob.lineTo (p);
        }
        blob.closeSubPath();
        g.setGradientFill (cg);
        g.fillPath (blob);
    }

    for (int i = 0; i < 300; ++i)
    {
        const float sx = r.nextFloat() * W, sy = r.nextFloat() * H, l = 6.0f + r.nextFloat() * 110.0f, a = r.nextFloat() * MathConstants<float>::pi;
        const float ex = sx + std::cos (a) * l, ey = sy + std::sin (a) * l;
        const float mx = (sx + ex) * 0.5f + (r.nextFloat() * 2 - 1) * 8, my = (sy + ey) * 0.5f + (r.nextFloat() * 2 - 1) * 8;
        const float w = r.nextFloat() < 0.85f ? 0.7f : 1.4f;
        Path p;
        p.startNewSubPath (sx, sy);
        p.quadraticTo (mx, my, ex, ey);
        g.setColour (Colours::black.withAlpha (0.35f));
        g.strokePath (p, PathStrokeType (w), AffineTransform::translation (0.8f, 0.8f));
        g.setColour (Colour (235, 230, 205).withAlpha (0.08f + r.nextFloat() * 0.22f));
        g.strokePath (p, PathStrokeType (w));
    }

    ColourGradient edge (Colour (255, 255, 240).withAlpha (0.08f), 0, 0, Colours::black.withAlpha (0.3f), 0, (float) H, false);
    edge.addColour (0.08, Colours::transparentBlack);
    edge.addColour (0.92, Colours::transparentBlack);
    g.setGradientFill (edge);
    g.fillAll();

    for (int i = 0; i < 120; ++i)
    {
        const float ex = r.nextBool() ? r.nextFloat() * 30.0f : W - r.nextFloat() * 30.0f;
        g.setColour (Colour (190, 185, 160).withAlpha (r.nextFloat() * 0.25f));
        g.fillRect (ex, r.nextFloat() * H, r.nextFloat() * 5.0f + 1.0f, r.nextFloat() * 3.0f + 1.0f);
    }

    scratchSig (g, (float) W - 150.0f, (float) H - 44.0f, 96.0f, r);
    return im;
}

//==============================================================================
DriftLAF::DriftLAF()
{
    setColour (TextButton::textColourOffId, cream);
    setColour (TextButton::textColourOnId, phosphor);
    setColour (ComboBox::backgroundColourId, screen);
    setColour (ComboBox::textColourId, phosphor);
    setColour (ComboBox::outlineColourId, Colours::black);
    setColour (ComboBox::arrowColourId, phosphor);
    setColour (PopupMenu::backgroundColourId, Colour (0xff0b160e));
    setColour (PopupMenu::textColourId, phosphor);
    setColour (PopupMenu::highlightedBackgroundColourId, Colour (0xff1d4a2b));
    setColour (PopupMenu::highlightedTextColourId, Colours::white);
    setColour (Slider::textBoxTextColourId, phosphor);
    setColour (Slider::textBoxOutlineColourId, Colours::transparentBlack);
    setColour (Slider::textBoxBackgroundColourId, Colours::transparentBlack);
}

void DriftLAF::drawButtonBackground (Graphics& g, Button& b, const Colour&, bool over, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced (1.0f).withTrimmedBottom (2.0f);
    if (down)
        r = r.translated (0.0f, 1.0f);
    const bool on = b.getToggleState();
    g.setColour (Colours::black.withAlpha (0.7f));
    g.fillRoundedRectangle (r.translated (0.0f, down ? 1.0f : 2.0f), 6.0f);
    const Colour top = on ? Colour (0xff123620) : Colour (0xff5d614e);
    const Colour bot = on ? Colour (0xff0a2112) : Colour (0xff373a2e);
    g.setGradientFill (ColourGradient (over ? top.brighter (0.12f) : top, 0, r.getY(), bot, 0, r.getBottom(), false));
    g.fillRoundedRectangle (r, 6.0f);
    if (on)
    {
        g.setColour (phosphor.withAlpha (0.15f));
        g.drawRoundedRectangle (r.reduced (1.5f), 5.0f, 2.0f);
    }
    g.setColour (Colours::white.withAlpha (on ? 0.08f : 0.2f));
    g.drawLine (r.getX() + 5, r.getY() + 1, r.getRight() - 5, r.getY() + 1, 1.0f);
    g.setColour (Colours::black.withAlpha (0.6f));
    g.drawRoundedRectangle (r, 6.0f, 1.0f);
}

static void knurledKnob (Graphics& g, Point<float> c, float kr)
{
    g.setColour (Colours::black.withAlpha (0.6f));
    g.fillEllipse (c.x - kr, c.y - kr + 2.0f, kr * 2, kr * 2);
    g.setGradientFill (ColourGradient (Colour (0xffd6d2bb), c.x, c.y - kr, Colour (0xff6c6b5c), c.x, c.y + kr, false));
    g.fillEllipse (c.x - kr, c.y - kr, kr * 2, kr * 2);
    g.setColour (Colour (0xff3a3a30).withAlpha (0.5f));
    for (int k = 0; k < 24; ++k)
    {
        const float a = (float) k / 24.0f * kTau;
        g.drawLine (c.x + std::cos (a) * kr * 0.8f, c.y + std::sin (a) * kr * 0.8f, c.x + std::cos (a) * kr, c.y + std::sin (a) * kr, 1.0f);
    }
    g.setColour (Colour (0xff23251d));
    g.drawEllipse (c.x - kr, c.y - kr, kr * 2, kr * 2, 1.5f);
}

void DriftLAF::drawLinearSlider (Graphics& g, int x, int y, int w, int h, float pos, float minPos, float maxPos,
                                 const Slider::SliderStyle st, Slider& s)
{
    if (st != Slider::LinearHorizontal)
    {
        LookAndFeel_V4::drawLinearSlider (g, x, y, w, h, pos, minPos, maxPos, st, s);
        return;
    }
    const float cy = (float) y + (float) h * 0.5f;
    const Rectangle<float> tr ((float) x, cy - 4.0f, (float) w, 8.0f);
    g.setColour (Colour (0xff171912));
    g.fillRoundedRectangle (tr, 4.0f);
    g.setColour (Colours::white.withAlpha (0.08f));
    g.drawLine (tr.getX() + 4, tr.getBottom() + 1, tr.getRight() - 4, tr.getBottom() + 1, 1.0f);

    const bool bipolar = s.getMinimum() < 0.0 && s.getMaximum() > 0.0;
    const float from = bipolar ? (float) x + (float) w * 0.5f : tr.getX();
    g.setColour (phosphor.withAlpha (0.55f));
    g.fillRoundedRectangle (Rectangle<float> (jmin (from, pos), cy - 1.5f, std::abs (pos - from), 3.0f), 1.5f);

    knurledKnob (g, { pos, cy }, 12.0f);
}

void DriftLAF::drawRotarySlider (Graphics& g, int x, int y, int w, int h, float pos, float a0, float a1, Slider& s)
{
    auto r = Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (5.0f);
    const float rad = jmin (r.getWidth(), r.getHeight()) * 0.5f;
    const auto c = r.getCentre();
    const float ang = a0 + pos * (a1 - a0);

    Path track;
    track.addCentredArc (c.x, c.y, rad, rad, 0.0f, a0, a1, true);
    g.setColour (Colour (0xff171912));
    g.strokePath (track, PathStrokeType (4.0f, PathStrokeType::curved, PathStrokeType::rounded));

    const bool bipolar = s.getMinimum() < 0.0 && s.getMaximum() > 0.0;
    const float from = bipolar ? (a0 + a1) * 0.5f : a0;
    Path val;
    val.addCentredArc (c.x, c.y, rad, rad, 0.0f, jmin (from, ang), jmax (from, ang), true);
    g.setColour (phosphor.withAlpha (0.22f));
    g.strokePath (val, PathStrokeType (7.0f));
    g.setColour (phosphor);
    g.strokePath (val, PathStrokeType (2.2f));

    const float kr = rad * 0.7f;
    knurledKnob (g, c, kr);
    g.setColour (Colour (0xff1a1b15));
    g.drawLine (c.x + std::sin (ang) * kr * 0.2f, c.y - std::cos (ang) * kr * 0.2f,
                c.x + std::sin (ang) * kr * 0.9f, c.y - std::cos (ang) * kr * 0.9f, 2.5f);
}

void DriftLAF::drawComboBox (Graphics& g, int w, int h, bool, int, int, int, int, ComboBox&)
{
    const Rectangle<float> r (0.0f, 0.0f, (float) w, (float) h);
    g.setColour (screen);
    g.fillRoundedRectangle (r, 6.0f);
    g.setColour (Colours::black);
    g.drawRoundedRectangle (r.reduced (0.5f), 6.0f, 1.5f);
    g.setColour (phosphor.withAlpha (0.08f));
    g.drawRoundedRectangle (r.reduced (2.0f), 5.0f, 1.0f);
    Path arrow;
    const float ax = (float) w - 16.0f, ay = (float) h * 0.5f;
    arrow.addTriangle (ax - 5, ay - 3, ax + 5, ay - 3, ax, ay + 4);
    g.setColour (phosphor);
    g.fillPath (arrow);
}

Label* DriftLAF::createSliderTextBox (Slider& s)
{
    auto* l = LookAndFeel_V4::createSliderTextBox (s);
    l->setFont (mono (14.0f));
    l->setColour (Label::textColourId, phosphor);
    l->setColour (Label::outlineColourId, Colours::transparentBlack);
    l->setColour (Label::backgroundColourId, Colours::transparentBlack);
    return l;
}

//==============================================================================
Rectangle<float> Radar::field() const
{
    auto scr = getLocalBounds().toFloat().reduced (10.0f);
    const float m = 22.0f, s = jmin (scr.getWidth(), scr.getHeight()) - 2 * m;
    return { scr.getX() + m, scr.getY() + m, s, s };
}

void Radar::setFromMouse (Point<float> p)
{
    const auto f = field();
    const float nx = jlimit (0.0f, 1.0f, (p.x - f.getX()) / f.getWidth());
    const float ny = jlimit (0.0f, 1.0f, (p.y - f.getY()) / f.getHeight());
    if (auto* px = dynamic_cast<AudioParameterFloat*> (proc.apvts.getParameter ("x"))) *px = nx;
    if (auto* py = dynamic_cast<AudioParameterFloat*> (proc.apvts.getParameter ("y"))) *py = ny;
}

void Radar::mouseDown (const MouseEvent& e)
{
    proc.touching.store (true);
    for (auto id : { "x", "y" })
        if (auto* p = proc.apvts.getParameter (id)) p->beginChangeGesture();
    setFromMouse (e.position);
}

void Radar::mouseDrag (const MouseEvent& e) { setFromMouse (e.position); }

void Radar::mouseUp (const MouseEvent&)
{
    const bool snap = proc.apvts.getRawParameterValue ("snap")->load() > 0.5f;
    const bool motionOff = (int) proc.apvts.getRawParameterValue ("motion")->load() == 0;
    if (snap && motionOff)
    {
        if (auto* px = dynamic_cast<AudioParameterFloat*> (proc.apvts.getParameter ("x"))) *px = 0.0f;
        if (auto* py = dynamic_cast<AudioParameterFloat*> (proc.apvts.getParameter ("y"))) *py = 0.0f;
    }
    for (auto id : { "x", "y" })
        if (auto* p = proc.apvts.getParameter (id)) p->endChangeGesture();
    proc.touching.store (false);
}

void Radar::paint (Graphics& g)
{
    const auto b = getLocalBounds().toFloat();
    g.setColour (Colours::black.withAlpha (0.5f));
    g.fillRoundedRectangle (b.translated (0, 3), 20.0f);
    g.setColour (Colour (0xff1b1d16));
    g.fillRoundedRectangle (b, 20.0f);
    g.setColour (Colour (0xff6a6e5a));
    g.drawRoundedRectangle (b.reduced (1.5f), 20.0f, 2.0f);

    const auto scr = b.reduced (10.0f);
    const int iw = (int) scr.getWidth(), ih = (int) scr.getHeight();
    const float m = 22.0f, s = (float) jmin (iw, ih) - 2 * m, c0x = iw * 0.5f, c0y = ih * 0.5f;
    const float x = proc.curX.load(), y = proc.curY.load();
    const float px = m + x * s, py = m + y * s;
    const float sw = proc.sweep.load() * kTau - MathConstants<float>::halfPi;

    // phosphor layer with persistence: the sweep leaves a fading trace
    if (phos.getWidth() != iw || phos.getHeight() != ih)
    {
        phos = Image (Image::ARGB, iw, ih, true);
        Graphics pg (phos);
        pg.fillAll (screen);
    }
    {
        Graphics pg (phos);
        pg.setColour (screen.withAlpha (0.30f));
        pg.fillAll();
        pg.reduceClipRegion (Rectangle<int> ((int) m, (int) m, (int) s, (int) s));
        for (int k = 0; k < 28; ++k)
        {
            const float a = sw - (float) k * 0.022f;
            pg.setColour (phosphor.withAlpha (0.3f * (1.0f - (float) k / 28.0f)));
            pg.drawLine (c0x, c0y, c0x + std::cos (a) * s * 0.75f, c0y + std::sin (a) * s * 0.75f, k == 0 ? 2.6f : 2.0f);
        }
        for (int r = 4; r > 0; --r)
        {
            pg.setColour (phosphor.withAlpha (0.1f));
            pg.fillEllipse (px - r * 4.0f, py - r * 4.0f, r * 8.0f, r * 8.0f);
        }
    }

    Graphics::ScopedSaveState save (g);
    Path clip;
    clip.addRoundedRectangle (scr, 14.0f);
    g.reduceClipRegion (clip);
    g.drawImageAt (phos, (int) scr.getX(), (int) scr.getY());
    g.addTransform (AffineTransform::translation (scr.getX(), scr.getY()));

    const float w[4] { (1 - x) * (1 - y), x * (1 - y), (1 - x) * y, x * y };
    const Point<float> cs[4] { { m, m }, { m + s, m }, { m, m + s }, { m + s, m + s } };
    {
        Graphics::ScopedSaveState s2 (g);
        g.reduceClipRegion (Rectangle<int> ((int) m, (int) m, (int) s, (int) s));
        for (int i = 0; i < 4; ++i)
        {
            ColourGradient cg (phosphor.withAlpha (0.02f + 0.16f * w[i]), cs[i], phosphor.withAlpha (0.0f), cs[i].translated (s * 0.65f, 0), true);
            g.setGradientFill (cg);
            g.fillRect (m, m, s, s);
        }
        g.setColour (phosphor.withAlpha (0.16f));
        for (int r = 1; r <= 5; ++r)
        {
            const float R = s * 0.142f * (float) r;
            g.drawEllipse (c0x - R, c0y - R, R * 2, R * 2, 1.0f);
        }
        g.setColour (phosphor.withAlpha (0.1f));
        for (int k = 0; k < 12; ++k)
        {
            const float a = (float) k / 12.0f * kTau;
            g.drawLine (c0x, c0y, c0x + std::cos (a) * s, c0y + std::sin (a) * s, 1.0f);
        }
    }

    g.setColour (phosphor.withAlpha (0.5f));
    g.drawRect (Rectangle<float> (m, m, s, s), 1.5f);
    g.setColour (phosphor.withAlpha (0.45f));
    for (int k = 0; k <= 32; ++k)
    {
        const float v = m + (float) k / 32.0f * s, l = k % 8 ? 4.0f : 9.0f;
        g.fillRect (v, m, 1.0f, l);
        g.fillRect (v, m + s - l, 1.0f, l);
        g.fillRect (m, v, l, 1.0f);
        g.fillRect (m + s - l, v, l, 1.0f);
    }

    g.setFont (mono (16.0f));
    for (int i = 0; i < 4; ++i)
    {
        const String t = String (dl::cornerLetter[i]) + " " + String (dl::cornerName[i]).toUpperCase() + " " + String (roundToInt (w[i] * 100.0f)) + "%";
        const bool right = (i % 2) == 1;
        const float ty = i < 2 ? m + 10.0f : m + s - 30.0f;
        g.setColour (phosphor.withAlpha (0.4f + 0.6f * w[i]));
        g.drawText (t, Rectangle<float> (right ? m + s * 0.5f : m + 12.0f, ty, s * 0.5f - 12.0f, 20.0f),
                    right ? Justification::centredRight : Justification::centredLeft);
    }

    // target crosshair
    const float tx = m + proc.tgtX.load() * s, ty = m + proc.tgtY.load() * s;
    g.setColour (phosphor.withAlpha (0.55f));
    g.drawEllipse (tx - 15, ty - 15, 30, 30, 1.5f);
    g.drawLine (tx - 22, ty, tx - 17, ty, 1.5f);
    g.drawLine (tx + 17, ty, tx + 22, ty, 1.5f);
    g.drawLine (tx, ty - 22, tx, ty - 17, 1.5f);
    g.drawLine (tx, ty + 17, tx, ty + 22, 1.5f);

    // blip flares when the sweep passes it
    float da = std::fmod (sw - std::atan2 (py - c0y, px - c0x), kTau);
    if (da < 0) da += kTau;
    const float fl = da < 0.6f ? 1.0f - da / 0.6f : 0.0f;
    if (fl > 0.0f)
    {
        const float rr = 12.0f + (1.0f - fl) * 22.0f;
        g.setColour (phosphor.withAlpha (fl * 0.6f));
        g.drawEllipse (px - rr, py - rr, rr * 2, rr * 2, 1.5f);
    }
    for (int r = 5; r > 0; --r)
    {
        const float rr = 8.0f + fl * 3.0f + (float) r * (3.0f + fl * 2.0f);
        g.setColour (phosphor.withAlpha (0.07f));
        g.fillEllipse (px - rr, py - rr, rr * 2, rr * 2);
    }
    const float core = 8.0f + fl * 3.0f;
    g.setColour (phosphor);
    g.fillEllipse (px - core, py - core, core * 2, core * 2);

    g.setColour (phosphor.withAlpha (0.85f));
    g.setFont (mono (15.0f));
    g.drawText ("X " + String (x, 2) + "  Y " + String (1.0f - y, 2), Rectangle<float> (m + 8, c0y - 10, 200, 20), Justification::centredLeft);

    // CRT glass: scanlines, vignette, reflection
    g.setColour (Colours::black.withAlpha (0.28f));
    for (int yy = 0; yy < ih; yy += 3)
        g.fillRect (0.0f, (float) yy + 2.0f, (float) iw, 1.0f);
    g.setGradientFill (ColourGradient (Colours::transparentBlack, c0x, c0y, Colours::black.withAlpha (0.7f), 0.0f, 0.0f, true));
    g.fillRect (0.0f, 0.0f, (float) iw, (float) ih);
    g.setGradientFill (ColourGradient (Colours::white.withAlpha (0.07f), iw * 0.3f, ih * 0.2f, Colours::transparentWhite, iw * 0.75f, ih * 0.2f, true));
    g.fillRect (0.0f, 0.0f, (float) iw, (float) ih);
}

//==============================================================================
void Tubes::paint (Graphics& g)
{
    const float lv[3] { proc.meterG.load(), proc.meterF.load(), proc.meterP.load() };
    const char* names[3] { "GRAIN", "FLANGE", "PAN" };
    const float tw = 40.0f;
    const float gap = ((float) getWidth() - 3.0f * tw) / 3.0f;

    for (int i = 0; i < 3; ++i)
    {
        const float x0 = gap * 0.5f + (float) i * (tw + gap), y0 = 4.0f, cx = x0 + tw * 0.5f;
        const float l = (0.18f + 0.82f * lv[i]) * (0.92f + 0.08f * rng.nextFloat());

        g.setGradientFill (ColourGradient (neon.withAlpha (0.9f * l), cx, y0 + 40, Colour (0xff7a2bd6).withAlpha (0.0f), cx + 38, y0 + 40, true));
        g.fillEllipse (cx - 38, y0, 76, 82);

        Path glass;
        glass.startNewSubPath (x0 + 6, y0 + 72);
        glass.lineTo (x0 + 6, y0 + 20);
        glass.quadraticTo (x0 + 6, y0 + 4, cx, y0 + 4);
        glass.quadraticTo (x0 + 34, y0 + 4, x0 + 34, y0 + 20);
        glass.lineTo (x0 + 34, y0 + 72);
        glass.closeSubPath();
        g.setGradientFill (ColourGradient (Colours::white.withAlpha (0.22f), x0 + 6, 0, Colours::white.withAlpha (0.03f), cx, 0, false));
        g.fillPath (glass);
        g.setColour (Colour (0xffd8d4bb).withAlpha (0.45f));
        g.strokePath (glass, PathStrokeType (1.0f));

        g.setColour (Colour (0xff8c8872));
        g.drawRect (Rectangle<float> (x0 + 12, y0 + 22, 16, 36), 1.2f);

        Path f;
        f.startNewSubPath (cx, y0 + 26);
        f.lineTo (cx, y0 + 54);
        f.startNewSubPath (cx - 4, y0 + 28);
        f.lineTo (cx, y0 + 32);
        f.lineTo (cx + 4, y0 + 28);
        for (int k = 3; k > 0; --k)
        {
            g.setColour (neon.withAlpha (l * 0.25f));
            g.strokePath (f, PathStrokeType (1.6f + (float) k * 2.0f));
        }
        g.setColour (Colour (0xffe6ccff).withAlpha (0.3f + 0.7f * l));
        g.strokePath (f, PathStrokeType (1.4f));

        g.setColour (Colour (0xff1a1b15));
        g.fillRoundedRectangle (x0 + 3, y0 + 70, 34, 13, 2);
        g.setColour (Colour (0xff4d4f42));
        g.drawRoundedRectangle (x0 + 3, y0 + 70, 34, 13, 2, 1.0f);
        g.setColour (Colour (0xff9a9680));
        for (float pxl : { 9.0f, 15.0f, 19.0f, 25.0f })
            g.drawLine (x0 + pxl + 3, y0 + 83, x0 + pxl + 3, y0 + 87, 1.0f);

        g.setColour (soft);
        g.setFont (mono (11.0f));
        g.drawText (names[i], Rectangle<float> (x0 - 14, y0 + 89, tw + 28, 16), Justification::centred);
    }
}

void LfoMeter::paint (Graphics& g)
{
    const auto b = getLocalBounds().toFloat();
    g.setColour (Colour (0xff0b0610));
    g.fillRoundedRectangle (b, 4.0f);
    const float v = get ? get() : 0.0f;
    const float cx = b.getX() + b.getWidth() * (0.5f + v * 0.44f);
    g.setColour (neon.withAlpha (0.35f));
    g.fillRoundedRectangle (cx - 6, b.getY(), 12, b.getHeight(), 3.0f);
    g.setColour (Colour (0xffd9b3ff));
    g.fillRoundedRectangle (cx - 3, b.getY(), 6, b.getHeight(), 3.0f);
}

//==============================================================================
DriftEditor::DriftEditor (DriftProcessor& p)
    : AudioProcessorEditor (&p), proc (p), radar (p), tubes (p)
{
    setLookAndFeel (&laf);
    wear = makeWear (kW, kH);
    auto& s = proc.apvts;

    addAndMakeVisible (radar);
    addAndMakeVisible (tubes);

    auto setupLinear = [this] (Slider& sl)
    {
        sl.setSliderStyle (Slider::LinearHorizontal);
        sl.setTextBoxStyle (Slider::TextBoxRight, true, 64, 22);
        addAndMakeVisible (sl);
    };

    mix.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
    mix.setTextBoxStyle (Slider::TextBoxBelow, true, 80, 20);
    addAndMakeVisible (mix);
    sAtt.push_back (std::make_unique<SA> (s, "mix", mix));

    setupLinear (glide);
    sAtt.push_back (std::make_unique<SA> (s, "glide", glide));

    snap.setClickingTogglesState (true);
    addAndMakeVisible (snap);
    snapAtt = std::make_unique<BA> (s, "snap", snap);

    reset.onClick = [this] { setChoice ("motion", 0); setFloat ("x", 0.0f); setFloat ("y", 0.0f); };
    addAndMakeVisible (reset);

    for (int i = 0; i < dl::motionNames.size(); ++i)
    {
        auto* b = motionBtns.add (new TextButton (dl::motionNames[i]));
        b->onClick = [this, i] { setChoice ("motion", i); };
        addAndMakeVisible (b);
    }

    cycle.addItemList (dl::cycleNames, 1);
    addAndMakeVisible (cycle);
    cAtt.push_back (std::make_unique<CA> (s, "mcycle", cycle));

    setupLinear (msize);
    sAtt.push_back (std::make_unique<SA> (s, "msize", msize));

    for (int i = 0; i < 2; ++i)
    {
        const String id = "lfo" + String (i + 1);
        lt[i].addItemList (dl::targetNames(), 1);
        ls[i].addItemList (dl::shapeNames, 1);
        lr[i].addItemList (dl::rateNames, 1);
        for (auto* cb : { &lt[i], &ls[i], &lr[i] })
            addAndMakeVisible (*cb);
        cAtt.push_back (std::make_unique<CA> (s, id + "_target", lt[i]));
        cAtt.push_back (std::make_unique<CA> (s, id + "_shape", ls[i]));
        cAtt.push_back (std::make_unique<CA> (s, id + "_rate", lr[i]));
        setupLinear (ld[i]);
        sAtt.push_back (std::make_unique<SA> (s, id + "_depth", ld[i]));
        meters[i].get = [this, i] { return proc.lfoVal[i].load(); };
        addAndMakeVisible (meters[i]);
    }

    for (int c = 0; c < 4; ++c)
    {
        auto* b = tabs.add (new TextButton (String (dl::cornerLetter[c]) + "  " + dl::cornerName[c]));
        b->onClick = [this, c] { selectCorner (c); };
        addAndMakeVisible (b);
    }

    for (int k = 0; k < 12; ++k)
    {
        auto* kn = knobs.add (new Slider());
        kn->setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        kn->setTextBoxStyle (Slider::TextBoxBelow, true, 72, 18);
        addAndMakeVisible (kn);
    }
    selectCorner (0);

    setSize (kW, kH);
    startTimerHz (30);
}

DriftEditor::~DriftEditor()
{
    stopTimer();
    knobAtt.clear();
    setLookAndFeel (nullptr);
}

void DriftEditor::setChoice (const String& id, int index)
{
    if (auto* p = dynamic_cast<AudioParameterChoice*> (proc.apvts.getParameter (id)))
    {
        p->beginChangeGesture();
        *p = index;
        p->endChangeGesture();
    }
}

void DriftEditor::setFloat (const String& id, float v)
{
    if (auto* p = dynamic_cast<AudioParameterFloat*> (proc.apvts.getParameter (id)))
    {
        p->beginChangeGesture();
        *p = v;
        p->endChangeGesture();
    }
}

void DriftEditor::selectCorner (int c)
{
    corner = c;
    knobAtt.clear();
    for (int k = 0; k < 12; ++k)
        knobAtt.push_back (std::make_unique<SA> (proc.apvts, dl::sid (c, k), *knobs[k]));
    for (int i = 0; i < tabs.size(); ++i)
        tabs[i]->setToggleState (i == c, dontSendNotification);
}

void DriftEditor::timerCallback()
{
    const int m = (int) proc.apvts.getRawParameterValue ("motion")->load();
    for (int i = 0; i < motionBtns.size(); ++i)
        motionBtns[i]->setToggleState (i == m, dontSendNotification);
    radar.repaint();
    tubes.repaint();
    for (auto& mt : meters)
        mt.repaint();
}

void DriftEditor::resized()
{
    radar.setBounds (32, 140, 470, 470);
    tubes.setBounds (kW - 36 - 180, 18, 180, 112);
    mix.setBounds (kW - 36 - 180 - 140, 10, 118, 124);

    colX = 536;
    colW = kW - 36 - colX;
    int yy = 158;
    glide.setBounds (colX, yy, colW, 32);
    yy = 204;
    snap.setBounds (colX, yy, 190, 42);
    reset.setBounds (colX + 200, yy, 110, 42);

    yy = 274;
    const int bw = (colW - 4 * 6) / 5;
    for (int i = 0; i < motionBtns.size(); ++i)
        motionBtns[i]->setBounds (colX + i * (bw + 6), yy, bw, 42);
    yy = 326;
    cycle.setBounds (colX, yy, 140, 36);
    msize.setBounds (colX + 200, yy, colW - 200, 36);

    lfoY = 382;
    lfoW = (colW - 12) / 2;
    for (int i = 0; i < 2; ++i)
    {
        const int px = colX + i * (lfoW + 12);
        meters[i].setBounds (px + lfoW - 76, lfoY + 16, 64, 8);
        lt[i].setBounds (px + 10, lfoY + 40, lfoW - 20, 34);
        const int half = (lfoW - 26) / 2;
        ls[i].setBounds (px + 10, lfoY + 80, half, 34);
        lr[i].setBounds (px + 16 + half, lfoY + 80, half, 34);
        ld[i].setBounds (px + 10, lfoY + 146, lfoW - 20, 32);
    }

    sceneY = 640;
    const int tx = 270, tw = (kW - 36 - tx - 18) / 4;
    for (int c = 0; c < tabs.size(); ++c)
        tabs[c]->setBounds (tx + c * (tw + 6), sceneY - 6, tw, 42);

    const int kw = (kW - 64) / 12;
    for (int k = 0; k < knobs.size(); ++k)
        knobs[k]->setBounds (32 + k * kw, sceneY + 70, kw - 4, 96);
}

void DriftEditor::paint (Graphics& g)
{
    g.drawImageAt (wear, 0, 0);

    // rivets
    for (auto pt : { Point<float> (16, 16), Point<float> (kW - 16.0f, 16), Point<float> (16, kH - 16.0f), Point<float> (kW - 16.0f, kH - 16.0f) })
    {
        g.setColour (Colours::black.withAlpha (0.55f));
        g.fillEllipse (pt.x - 6, pt.y - 5, 12, 12);
        g.setGradientFill (ColourGradient (Colour (0xffc4c6ae), pt.x - 2, pt.y - 3, Colour (0xff4f5244), pt.x + 3, pt.y + 4, false));
        g.fillEllipse (pt.x - 5, pt.y - 5, 10, 10);
        g.setColour (Colours::black.withAlpha (0.5f));
        g.drawLine (pt.x - 3, pt.y, pt.x + 3, pt.y, 1.2f);
    }

    // stencilled title
    g.setFont (Font (FontOptions (56.0f, Font::bold)));
    g.setColour (Colours::black.withAlpha (0.55f));
    g.drawText ("DRIFT LIVE", 34, 26, 520, 64, Justification::centredLeft);
    g.setColour (Colour (0xffe9e3c3).withAlpha (0.9f));
    g.drawText ("DRIFT LIVE", 32, 24, 520, 64, Justification::centredLeft);
    g.setColour (soft);
    g.setFont (mono (14.0f));
    g.drawText ("granular field unit  /  XY radar", 34, 88, 400, 20, Justification::centredLeft);

    auto label = [&] (const String& t, int lx, int ly)
    {
        g.setColour (Colour (0xffd9d4b9));
        g.setFont (mono (13.0f));
        g.drawText (t, lx, ly, 200, 18, Justification::centredLeft);
    };
    g.setColour (cream);
    g.setFont (Font (FontOptions (20.0f, Font::bold)));
    g.drawText ("MIX", kW - 36 - 180 - 140 - 64, 50, 60, 24, Justification::centredRight);
    g.setColour (soft);
    g.setFont (mono (11.0f));
    g.drawText ("dry / wet", kW - 36 - 180 - 140 - 84, 72, 80, 16, Justification::centredRight);
    label ("GLIDE", colX, 140);
    label ("AUTO MOTION", colX, 256);
    label ("SIZE", colX + 152, 335);

    for (int i = 0; i < 2; ++i)
    {
        const Rectangle<float> pr ((float) (colX + i * (lfoW + 12)), (float) lfoY, (float) lfoW, 196.0f);
        g.setColour (Colours::black.withAlpha (0.22f));
        g.fillRoundedRectangle (pr, 10.0f);
        g.setColour (Colours::black.withAlpha (0.55f));
        g.drawRoundedRectangle (pr.reduced (0.5f), 10.0f, 1.5f);
        g.setColour (Colours::white.withAlpha (0.08f));
        g.drawLine (pr.getX() + 10, pr.getBottom() + 1, pr.getRight() - 10, pr.getBottom() + 1, 1.0f);
        g.setColour (cream);
        g.setFont (Font (FontOptions (18.0f, Font::bold)));
        g.drawText ("LFO " + String (i + 1), (int) pr.getX() + 12, (int) pr.getY() + 8, 100, 24, Justification::centredLeft);
        label ("DEPTH", (int) pr.getX() + 12, (int) pr.getY() + 126);
    }

    // scenes
    g.setColour (Colours::black.withAlpha (0.5f));
    g.drawLine (32, (float) sceneY - 22, kW - 32.0f, (float) sceneY - 22, 1.0f);
    g.setColour (Colours::white.withAlpha (0.08f));
    g.drawLine (32, (float) sceneY - 21, kW - 32.0f, (float) sceneY - 21, 1.0f);
    g.setColour (cream);
    g.setFont (Font (FontOptions (22.0f, Font::bold)));
    g.drawText ("CORNER SCENES", 32, sceneY - 4, 240, 36, Justification::centredLeft);

    const int kw = (kW - 64) / 12;
    g.setFont (mono (11.0f));
    for (int k = 0; k < 12; ++k)
    {
        g.setColour (Colour (0xffd9d4b9));
        g.drawFittedText (String (dl::scene[(size_t) k].name).toUpperCase(), 32 + k * kw, sceneY + 46, kw - 4, 24, Justification::centred, 2);
    }

    g.setColour (soft);
    g.setFont (mono (12.0f));
    g.drawText ("Insert on drums or any instrument. Each corner is a full scene; the radar blends them. LFOs ride on top.",
                32, kH - 64, kW - 220, 18, Justification::centredLeft);
}
