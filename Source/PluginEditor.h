#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class MetalTuneAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MetalTuneAudioProcessorEditor (MetalTuneAudioProcessor&);
    ~MetalTuneAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MetalTuneAudioProcessor& audioProcessor;

    juce::Slider pitchSlider, driveSlider, mixSlider;
    juce::Label pitchLabel, driveLabel, mixLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MetalTuneAudioProcessorEditor)
};
