#include "PluginProcessor.h"
#include "PluginEditor.h"

MetalTuneAudioProcessorEditor::MetalTuneAudioProcessorEditor (MetalTuneAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (350, 220);

    auto setupSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& text) {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible (slider);
        label.setText (text, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (label);
    };

    setupSlider (pitchSlider, pitchLabel, "Pitch (Semi)");
    setupSlider (driveSlider, driveLabel, "Metal Grit");
    setupSlider (mixSlider, mixLabel, "Mix");

    pitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "pitch", pitchSlider);
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "drive", driveSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "mix", mixSlider);
}

MetalTuneAudioProcessorEditor::~MetalTuneAudioProcessorEditor() {}

void MetalTuneAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff181818));
    g.setColour (juce::Colours::crimson);
    g.setFont (20.0f);
    g.drawFittedText ("METALTUNE DSP", getLocalBounds().removeFromTop (35), juce::Justification::centred, 1);
}

void MetalTuneAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (10);
    area.removeFromTop (30);
    auto sliderWidth = area.getWidth() / 3;

    auto r1 = area.removeFromLeft (sliderWidth);
    pitchLabel.setBounds (r1.removeFromBottom (20));
    pitchSlider.setBounds (r1);

    auto r2 = area.removeFromLeft (sliderWidth);
    driveLabel.setBounds (r2.removeFromBottom (20));
    driveSlider.setBounds (r2);

    mixLabel.setBounds (area.removeFromBottom (20));
    mixSlider.setBounds (area);
}
