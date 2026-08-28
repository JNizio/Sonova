#pragma once
#include <JuceHeader.h>

class SimpleSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
    explicit SynthVoice (juce::AudioProcessorValueTreeState& state);

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}
    void prepare (double sampleRate, int samplesPerBlock, int outputChannels);
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::dsp::Oscillator<float> osc1, osc2;
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::ADSR ampEnvelope;
    juce::ADSR::Parameters adsrParams;

    double currentSampleRate = 44100.0;
    float baseFrequency = 440.0f;
    float noteVelocity = 1.0f;

    void updateOscillatorWaveforms();
    void updateEnvelope();
    void updateFrequencies();
};
