#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <array>

namespace dl
{
struct PDef
{
    const char* key;
    const char* name;
    float min, max;
    float def[4];   // default for corners A, B, C, D
    float step;
};

// One scene = these 12 values. Each corner of the radar holds its own scene.
inline const std::array<PDef, 12> scene {{
    { "gmix",   "Grains",           0.0f,   1.0f,  { 0.0f, 1.0f,   0.15f, 0.6f   }, 0.01f },
    { "pitch",  "Pitch",          -12.0f,  12.0f,  { 0.0f, 0.0f,   0.0f, -12.0f  }, 1.0f  },
    { "size",   "Size",            15.0f, 400.0f,  { 90.0f, 230.0f, 40.0f, 130.0f }, 1.0f  },
    { "dens",   "Density",          2.0f,  60.0f,  { 18.0f, 32.0f,  24.0f, 22.0f  }, 1.0f  },
    { "spray",  "Spray",            0.0f, 800.0f,  { 80.0f, 520.0f, 30.0f, 220.0f }, 1.0f  },
    { "rev",    "Reverse",          0.0f,   1.0f,  { 0.0f, 0.4f,   0.1f, 0.3f    }, 0.01f },
    { "pan",    "Pan",             -1.0f,   1.0f,  { 0.0f, 0.0f,  -0.5f, 0.5f    }, 0.01f },
    { "gpan",   "Grain spread",     0.0f,   1.0f,  { 0.0f, 0.8f,   0.3f, 0.6f    }, 0.01f },
    { "fmix",   "Flanger",          0.0f,   1.0f,  { 0.0f, 0.2f,   0.75f, 0.45f  }, 0.01f },
    { "frate",  "Flanger rate",     0.05f,  6.0f,  { 0.3f, 0.1f,   2.4f, 0.6f    }, 0.01f },
    { "fdepth", "Flanger depth",    0.0f,   1.0f,  { 0.5f, 0.8f,   0.6f, 1.0f    }, 0.01f },
    { "ffb",    "Flanger feedback",-0.9f,   0.9f,  { 0.3f, 0.5f,   0.8f, -0.7f   }, 0.01f },
}};

inline const char* cornerLetter[4] { "A", "B", "C", "D" };
inline const char* cornerName[4]   { "Clean", "Cloud", "Swirl", "Abyss" };

inline juce::String sid (int corner, int k)
{
    return juce::String (cornerLetter[corner]) + "_" + scene[(size_t) k].key;
}

inline const juce::StringArray motionNames { "Off", "Circle", "Eight", "Bounce", "Random" };
inline const juce::StringArray cycleNames  { "1 bar", "2 bars", "4 bars", "8 bars" };
inline const float cycleBeats[4]           { 4.0f, 8.0f, 16.0f, 32.0f };
inline const juce::StringArray shapeNames  { "Sine", "Triangle", "Square", "Saw", "Random" };
inline const juce::StringArray rateNames   { "1/16", "1/8", "1/4", "1/2", "1 bar", "2 bars", "4 bars" };
inline const float rateBeats[7]            { 0.25f, 0.5f, 1.0f, 2.0f, 4.0f, 8.0f, 16.0f };

inline juce::StringArray targetNames()
{
    juce::StringArray s;
    for (auto& p : scene)
        s.add (p.name);
    return s;
}

inline juce::AudioProcessorValueTreeState::ParameterLayout createLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout l;

    auto addF = [&] (const String& id, const String& name, float mn, float mx, float def, float step)
    {
        l.add (std::make_unique<AudioParameterFloat> (ParameterID { id, 1 }, name,
                                                      NormalisableRange<float> (mn, mx, step), def));
    };
    auto addC = [&] (const String& id, const String& name, const StringArray& items, int def)
    {
        l.add (std::make_unique<AudioParameterChoice> (ParameterID { id, 1 }, name, items, def));
    };

    // Mix goes first so it shows at the top of the DAW parameter list
    l.add (std::make_unique<AudioParameterFloat> (
        ParameterID { "mix", 1 }, "Mix", NormalisableRange<float> (0.0f, 1.0f, 0.01f), 1.0f,
        AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int) { return String (roundToInt (v * 100.0f)) + "%"; })
            .withValueFromStringFunction ([] (const String& t) { return t.getFloatValue() / 100.0f; })));
    addF ("x", "X", 0.0f, 1.0f, 0.0f, 0.0f);
    addF ("y", "Y", 0.0f, 1.0f, 0.0f, 0.0f);
    addF ("glide", "Glide", 0.0f, 3.0f, 0.35f, 0.01f);
    l.add (std::make_unique<AudioParameterBool> (ParameterID { "snap", 1 }, "Snap back", false));
    addC ("motion", "Motion", motionNames, 0);
    addC ("mcycle", "Motion cycle", cycleNames, 1);
    addF ("msize", "Motion size", 0.1f, 1.0f, 0.7f, 0.01f);

    for (int i = 0; i < 2; ++i)
    {
        const String p = "lfo" + String (i + 1);
        const String n = "LFO " + String (i + 1);
        addC (p + "_target", n + " target", targetNames(), i == 0 ? 4 : 6);
        addC (p + "_shape",  n + " shape",  shapeNames, 0);
        addC (p + "_rate",   n + " rate",   rateNames, i == 0 ? 4 : 5);
        addF (p + "_depth",  n + " depth", -1.0f, 1.0f, i == 0 ? 0.4f : 0.0f, 0.01f);
    }

    for (int c = 0; c < 4; ++c)
        for (int k = 0; k < (int) scene.size(); ++k)
        {
            auto& d = scene[(size_t) k];
            addF (sid (c, k), String (cornerLetter[c]) + " " + d.name, d.min, d.max, d.def[c], d.step);
        }

    return l;
}
} // namespace dl
