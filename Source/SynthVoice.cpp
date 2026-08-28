#include "SynthVoice.h"

SynthVoice::SynthVoice (juce::AudioProcessorValueTreeState& state)
    : apvts (state)
{
}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SimpleSound*> (sound) != nullptr;
}

void SynthVoice::prepare (double sampleRate, int samplesPerBlock, int)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = 1;

    osc1.prepare (sampleRate);
    osc2.prepare (sampleRate);
    filter.prepare (spec);
    filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    ampEnvelope.setSampleRate (sampleRate);
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    baseFrequency = static_cast<float> (juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));
    noteVelocity = velocity;

    // Deterministic reset keeps transients clean and phase coherent between voices.
    osc1.reset (0.0);
    osc2.reset (0.25);

    updateOscillatorWaveforms();
    updateEnvelope();
    updateFrequencies();
    filter.reset();
    ampEnvelope.noteOn();
}

void SynthVoice::stopNote (float, bool allowTailOff)
{
    if (allowTailOff)
        ampEnvelope.noteOff();
    else
    {
        ampEnvelope.reset();
        clearCurrentNote();
    }
}

void SynthVoice::updateEnvelope()
{
    adsrParams.attack  = apvts.getRawParameterValue ("attack")->load();
    adsrParams.decay   = apvts.getRawParameterValue ("decay")->load();
    adsrParams.sustain = apvts.getRawParameterValue ("sustain")->load();
    adsrParams.release = apvts.getRawParameterValue ("release")->load();
    ampEnvelope.setParameters (adsrParams);
}

void SynthVoice::updateOscillatorWaveforms()
{
    osc1.setWaveform (static_cast<int> (apvts.getRawParameterValue ("osc1Wave")->load()));
    osc2.setWaveform (static_cast<int> (apvts.getRawParameterValue ("osc2Wave")->load()));
}

void SynthVoice::updateFrequencies()
{
    const float octave1 = apvts.getRawParameterValue ("osc1Oct")->load();
    const float octave2 = apvts.getRawParameterValue ("osc2Oct")->load();
    const float detune1 = apvts.getRawParameterValue ("osc1Detune")->load();
    const float detune2 = apvts.getRawParameterValue ("osc2Detune")->load();

    const auto pitchRatio = [] (float oct, float cents)
    {
        return std::pow (2.0f, oct + cents / 1200.0f);
    };

    osc1.setFrequency (baseFrequency * pitchRatio (octave1, detune1));
    osc2.setFrequency (baseFrequency * pitchRatio (octave2, detune2));
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    updateOscillatorWaveforms();
    updateEnvelope();
    updateFrequencies();

    const float mix = apvts.getRawParameterValue ("mix")->load();
    const float cutoff = apvts.getRawParameterValue ("cutoff")->load();
    const float resonance = apvts.getRawParameterValue ("resonance")->load();

    filter.setCutoffFrequency (juce::jlimit (20.0f, static_cast<float> (currentSampleRate * 0.45), cutoff));
    filter.setResonance (resonance);

    // Equal-power blend avoids the level dip of a linear crossfade around 50/50.
    const float gainA = std::cos (mix * juce::MathConstants<float>::halfPi);
    const float gainB = std::sin (mix * juce::MathConstants<float>::halfPi);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float one = osc1.process();
        const float two = osc2.process();
        float value = (one * gainA + two * gainB) * 0.72f;

        value = filter.processSample (0, value);
        value *= ampEnvelope.getNextSample() * noteVelocity * 0.32f;

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addSample (channel, startSample + sample, value);
    }

    if (! ampEnvelope.isActive())
        clearCurrentNote();
}
