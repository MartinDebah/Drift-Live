#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "Params.h"

class DriftProcessor : public juce::AudioProcessor
{
public:
    DriftProcessor();
    ~DriftProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Drift Live"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 1.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;

    // Shared with the editor (written by the audio thread)
    std::atomic<float> curX { 0.0f }, curY { 0.0f }, tgtX { 0.0f }, tgtY { 0.0f }, sweep { 0.0f };
    std::atomic<float> meterG { 0.0f }, meterF { 0.0f }, meterP { 0.0f };
    std::atomic<float> lfoVal[2] { { 0.0f }, { 0.0f } };
    std::atomic<bool> touching { false };

private:
    struct Grain
    {
        bool on = false;
        double pos = 0.0, inc = 1.0;
        int len = 1, age = 0;
        float gl = 1.0f, gr = 1.0f, amp = 1.0f;
    };

    void spawn (double rate, int glen, float spraySamples, float rev, float gpan, float amp);
    float readHist (const float* h, double pos) const;
    float readFl (const float* b, float delaySamples) const;

    double sr = 44100.0;
    juce::AudioBuffer<float> hist, fbuf;
    int hw = 0, hlen = 1, fw = 0, flen = 1;
    std::array<Grain, 96> grains;
    double untilNext = 0.0, fph = 0.0, ppqFree = 0.0;
    float x = 0.0f, y = 0.0f, rtx = 0.5f, rty = 0.5f;
    long long lastRand = -1;
    juce::Random rng;

    juce::SmoothedValue<float> sClean, sGrain, sPanL, sPanR, sFDry, sFWet, sFfb, sFdep, sMix;

    std::atomic<float>* pX = nullptr;
    std::atomic<float>* pY = nullptr;
    std::atomic<float>* pGlide = nullptr;
    std::atomic<float>* pMotion = nullptr;
    std::atomic<float>* pCycle = nullptr;
    std::atomic<float>* pMSize = nullptr;
    std::atomic<float>* pMix = nullptr;
    std::atomic<float>* lT[2] {};
    std::atomic<float>* lS[2] {};
    std::atomic<float>* lR[2] {};
    std::atomic<float>* lD[2] {};
    std::atomic<float>* corner[4][12] {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DriftProcessor)
};
