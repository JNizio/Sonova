#include "SynthVoice.h"

SynthVoice::SynthVoice (juce::AudioProcessorValueTreeState& state)
    : apvts (state),
      osc1 ([] (float x) { return std::sin (x); }),
      osc2 ([] (float x) { return std::sin (x); })
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

    osc1.prepare (spec);
    osc2.prepare (spec);
    filter.prepare (spec);
    filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    ampEnvelope.setSampleRate (sampleRate);
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    baseFrequency = static_cast<float> (juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));
    noteVelocity = velocity;
    updateEnvelope();
    updateFrequencies();
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
    const int wave1 = static_cast<int> (apvts.getRawParameterValue ("osc1Wave")->load());
    const int wave2 = static_cast<int> (apvts.getRawParameterValue ("osc2Wave")->load());

    auto makeFn = [] (int waveform)
    {
        return [waveform] (float phase)
        {
            switch (waveform)
            {
                case 1:
                    return juce::jmap (phase, -juce::MathConstants<float>::pi,
                                      juce::MathConstants<float>::pi, -1.0f, 1.0f);
                case 2:
                    return phase < 0.0f ? -1.0f : 1.0f;
                case 3:
                    return (2.0f / juce::MathConstants<float>::pi) * std::asin (std::sin (phase));
                default:
                    return std::sin (phase);
            }
        };
    };

    osc1.initialise (makeFn (wave1));
    osc2.initialise (makeFn (wave2));
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

    osc1.setFrequency (baseFrequency * pitchRatio (octave1, detune1), true);
    osc2.setFrequency (baseFrequency * pitchRatio (octave2, detune2), true);
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

    filter.setCutoffFrequency (cutoff);
    filter.setResonance (resonance);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float one = osc1.processSample (0.0f);
        const float two = osc2.processSample (0.0f);
        float value = one * (1.0f - mix) + two * mix;
        value = filter.processSample (0, value);
        value *= ampEnvelope.getNextSample() * noteVelocity * 0.35f;

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addSample (channel, startSample + sample, value);
    }

    if (! ampEnvelope.isActive())
        clearCurrentNote();
}
