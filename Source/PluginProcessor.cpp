#include "PluginProcessor.h"
#include "PluginEditor.h"

SonovaAudioProcessor::SonovaAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    for (int i = 0; i < 8; ++i)
        synth.addVoice (new SynthVoice (apvts));

    synth.addSound (new SimpleSound());
}

void SonovaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    masterGain.reset (sampleRate, 0.02);
    masterGain.setCurrentAndTargetValue (
        juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("gain")->load()));

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
            voice->prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

bool SonovaAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto set = layouts.getMainOutputChannelSet();
    return set == juce::AudioChannelSet::mono() || set == juce::AudioChannelSet::stereo();
}

void SonovaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synth.renderNextBlock (buffer, midi, 0, buffer.getNumSamples());

    masterGain.setTargetValue (
        juce::Decibels::decibelsToGain (apvts.getRawParameterValue ("gain")->load()));

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        const auto gain = masterGain.getNextValue();
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.setSample (channel, sample, buffer.getSample (channel, sample) * gain);
    }
}

juce::AudioProcessorEditor* SonovaAudioProcessor::createEditor()
{
    return new SonovaAudioProcessorEditor (*this);
}

void SonovaAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void SonovaAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorValueTreeState::ParameterLayout SonovaAudioProcessor::createParameterLayout()
{
    using APF = juce::AudioParameterFloat;
    using APC = juce::AudioParameterChoice;
    using PID = juce::ParameterID;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<APC> (PID { "osc1Wave", 1 }, "Oscillator A Wave",
                                              juce::StringArray { "Sine", "Saw", "Square", "Triangle" }, 1));
    params.push_back (std::make_unique<APF> (PID { "osc1Oct", 1 }, "Oscillator A Octave",
                                             juce::NormalisableRange<float> (-2.0f, 2.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<APF> (PID { "osc1Detune", 1 }, "Oscillator A Fine",
                                             juce::NormalisableRange<float> (-50.0f, 50.0f, 0.1f), 0.0f));

    params.push_back (std::make_unique<APC> (PID { "osc2Wave", 1 }, "Oscillator B Wave",
                                              juce::StringArray { "Sine", "Saw", "Square", "Triangle" }, 2));
    params.push_back (std::make_unique<APF> (PID { "osc2Oct", 1 }, "Oscillator B Octave",
                                             juce::NormalisableRange<float> (-2.0f, 2.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<APF> (PID { "osc2Detune", 1 }, "Oscillator B Fine",
                                             juce::NormalisableRange<float> (-50.0f, 50.0f, 0.1f), 7.0f));

    params.push_back (std::make_unique<APF> (PID { "mix", 1 }, "Blend",
                                             juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.5f));
    params.push_back (std::make_unique<APF> (PID { "attack", 1 }, "Attack",
                                             juce::NormalisableRange<float> (0.001f, 5.0f, 0.001f, 0.35f), 0.01f));
    params.push_back (std::make_unique<APF> (PID { "decay", 1 }, "Decay",
                                             juce::NormalisableRange<float> (0.001f, 5.0f, 0.001f, 0.35f), 0.25f));
    params.push_back (std::make_unique<APF> (PID { "sustain", 1 }, "Sustain",
                                             juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.8f));
    params.push_back (std::make_unique<APF> (PID { "release", 1 }, "Release",
                                             juce::NormalisableRange<float> (0.001f, 8.0f, 0.001f, 0.35f), 0.5f));

    params.push_back (std::make_unique<APF> (PID { "cutoff", 1 }, "Cutoff",
                                             juce::NormalisableRange<float> (40.0f, 20000.0f, 1.0f, 0.22f), 12000.0f));
    params.push_back (std::make_unique<APF> (PID { "resonance", 1 }, "Resonance",
                                             juce::NormalisableRange<float> (0.1f, 1.2f, 0.001f), 0.2f));
    params.push_back (std::make_unique<APF> (PID { "gain", 1 }, "Output",
                                             juce::NormalisableRange<float> (-24.0f, 6.0f, 0.1f), -3.0f));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SonovaAudioProcessor();
}
