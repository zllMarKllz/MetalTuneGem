cmake_minimum_required(VERSION 3.15)
project(MetalTune VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(MetalTune
    COMPANY_NAME "MetalDSP"
    PLUGIN_MANUFACTURER_CODE "MDsp"
    PLUGIN_CODE "MtTn"
    FORMATS VST3
    PRODUCT_NAME "MetalTune"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
)

target_sources(MetalTune PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_link_libraries(MetalTune PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)

juce_generate_juce_header(MetalTune)
