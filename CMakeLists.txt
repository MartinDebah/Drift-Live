cmake_minimum_required(VERSION 3.22)
project(DriftLive VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.4
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(JUCE)

if(APPLE)
    set(DRIFT_FORMATS AU VST3 Standalone)
elseif(WIN32)
    set(DRIFT_FORMATS VST3 Standalone)
else()
    set(DRIFT_FORMATS VST3 LV2 Standalone)
endif()

juce_add_plugin(DriftLive
    COMPANY_NAME "Debax"
    PLUGIN_MANUFACTURER_CODE Dbax
    PLUGIN_CODE Drlv
    FORMATS ${DRIFT_FORMATS}
    PRODUCT_NAME "Drift Live"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Fx
    AU_MAIN_TYPE kAudioUnitType_Effect
    LV2URI "https://github.com/MartinDebah/drift-live")

target_sources(DriftLive PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_definitions(DriftLive PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)

target_link_libraries(DriftLive
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
