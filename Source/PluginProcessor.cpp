#include "PluginProcessor.h"
#include "PluginEditor.h"

MetalTuneAudioProcessor::MetalTuneAudioProcessor()
    : AudioProcessor (BusesProperties().withInput ("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

MetalTuneAudioProcessor::~MetalTuneAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout MetalTuneAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat>("pitch", "Pitch Semitones", -12.0f, 12.0f, 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("drive", "Drive (Metal Grit)", 1.0f, 15.0f, 2.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0.0f, 1.0f, 1.0f));

    return { params.begin(), params.end() };
}

void MetalTuneAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    delayBuffer.setSize (getTotalNumInputChannels(), delayBufferSize);
    delayBuffer.clear();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumInputChannels();

    highPassFilter.prepare (spec);
    highPassFilter.setType (juce::dsp::StateVariableTPTFilterType::highpass);
    highPassFilter.setCutoffFrequency (85.0f);
}

void MetalTuneAudioProcessor::releaseResources() {}

void MetalTuneAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float pitchSemis = apvts.getRawParameterValue ("pitch")->load();
    float drive = apvts.getRawParameterValue ("drive")->load();
    float mix = apvts.getRawParameterValue ("mix")->load();

    float pitchRatio = std::pow (2.0f, pitchSemis / 12.0f);
    float rateDiff = (1.0f - pitchRatio);
    int numSamples = buffer.getNumSamples();

    const float windowSize = 2048.0f;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        readPhase += rateDiff;
        if (readPhase >= windowSize) readPhase -= windowSize;
        if (readPhase < 0.0f)        readPhase += windowSize;

        float offset1 = readPhase;
        float offset2 = std::fmod (readPhase + (windowSize * 0.5f), windowSize);

        float w1 = 1.0f - std::abs ((offset1 / (windowSize * 0.5f)) - 1.0f);
        float w2 = 1.0f - std::abs ((offset2 / (windowSize * 0.5f)) - 1.0f);

        for (int ch = 0; ch < totalNumInputChannels; ++ch)
        {
            float inputSample = buffer.getSample (ch, sample);
            delayBuffer.setSample (ch, writePosition, inputSample);

            auto getDelayed = [&](float offset) {
                float pos = (float)writePosition - offset;
                if (pos < 0.0f) pos += (float)delayBufferSize;
                int i0 = (int)pos;
                float frac = pos - (float)i0;
                int i1 = (i0 + 1) % delayBufferSize;
                return delayBuffer.getSample (ch, i0) * (1.0f - frac) + delayBuffer.getSample (ch, i1) * frac;
            };

            float pitched = (getDelayed (offset1) * w1) + (getDelayed (offset2) * w2);
            float saturated = std::tanh (pitched * drive);
            float output = (inputSample * (1.0f - mix)) + (saturated * mix);
            buffer.setSample (ch, sample, output);
        }

        if (++writePosition >= delayBufferSize)
            writePosition = 0;
    }

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context (block);
    highPassFilter.process (context);
}

juce::AudioProcessorEditor* MetalTuneAudioProcessor::createEditor()
{
    return new MetalTuneAudioProcessorEditor (*this);
}

void MetalTuneAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void MetalTuneAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MetalTuneAudioProcessor();
}
