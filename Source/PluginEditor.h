#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

namespace dlui
{
const juce::Colour phosphor { 0xff5dff8f };
const juce::Colour neon     { 0xffb45cff };
const juce::Colour cream    { 0xffe6e1c8 };
const juce::Colour soft     { 0xffa9a68e };
const juce::Colour screen   { 0xff061209 };
juce::Font mono (float h);
}

struct DriftLAF : public juce::LookAndFeel_V4
{
    DriftLAF();
    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool over, bool down) override;
    juce::Font getTextButtonFont (juce::TextButton&, int) override { return dlui::mono (15.0f); }
    void drawLinearSlider (juce::Graphics&, int x, int y, int w, int h, float pos, float minPos, float maxPos,
                           const juce::Slider::SliderStyle, juce::Slider&) override;
    int getSliderThumbRadius (juce::Slider&) override { return 12; }
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos, float a0, float a1, juce::Slider&) override;
    void drawComboBox (juce::Graphics&, int w, int h, bool down, int bx, int by, int bw, int bh, juce::ComboBox&) override;
    juce::Font getComboBoxFont (juce::ComboBox&) override { return dlui::mono (16.0f); }
    juce::Font getPopupMenuFont() override { return dlui::mono (16.0f); }
    juce::Label* createSliderTextBox (juce::Slider&) override;
};

class Radar : public juce::Component
{
public:
    explicit Radar (DriftProcessor& p) : proc (p) {}
    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;

private:
    void setFromMouse (juce::Point<float>);
    juce::Rectangle<float> field() const;
    DriftProcessor& proc;
    juce::Image phos;
};

class Tubes : public juce::Component
{
public:
    explicit Tubes (DriftProcessor& p) : proc (p) {}
    void paint (juce::Graphics&) override;

private:
    DriftProcessor& proc;
    juce::Random rng;
};

struct LfoMeter : public juce::Component
{
    std::function<float()> get;
    void paint (juce::Graphics&) override;
};

class DriftEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit DriftEditor (DriftProcessor&);
    ~DriftEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void selectCorner (int);
    void setChoice (const juce::String& id, int index);
    void setFloat (const juce::String& id, float v);

    DriftProcessor& proc;
    DriftLAF laf;
    juce::Image wear;

    Radar radar;
    Tubes tubes;
    juce::Slider mix, glide, msize;
    juce::TextButton snap { "Snap back" }, reset { "Reset" };
    juce::OwnedArray<juce::TextButton> motionBtns, tabs;
    juce::ComboBox cycle;
    juce::ComboBox lt[2], ls[2], lr[2];
    juce::Slider ld[2];
    LfoMeter meters[2];
    juce::OwnedArray<juce::Slider> knobs;
    int corner = 0;

    // layout values shared by resized() and paint()
    int colX = 0, colW = 0, lfoY = 0, lfoW = 0, sceneY = 0;

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::vector<std::unique_ptr<SA>> sAtt, knobAtt;
    std::vector<std::unique_ptr<CA>> cAtt;
    std::unique_ptr<BA> snapAtt;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DriftEditor)
};
