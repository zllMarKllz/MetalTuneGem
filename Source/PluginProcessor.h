#pragma once
#include <JuceHeader.h>

class MetalTuneAudioProcessor : public juce::AudioProcessor
{
public:
    MetalTuneAudioProcessor();
    ~MetalTuneAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "MetalTune"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    static constexpr int delayBufferSize = 88200;
    juce::AudioBuffer<float> delayBuffer;
    int writePosition { 0 };
    float readPhase { 0.0f };

    juce::dsp::StateVariableTPTFilter<float> highPassFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MetalTuneAudioProcessor)
};
