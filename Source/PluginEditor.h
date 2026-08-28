#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SonovaLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SonovaLookAndFeel();
    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;
    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;
};

class SonovaAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SonovaAudioProcessorEditor (SonovaAudioProcessor&);
    ~SonovaAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SonovaAudioProcessor& processor;
    SonovaLookAndFeel sonovaLookAndFeel;

    juce::ComboBox osc1Wave, osc2Wave;
    juce::Slider osc1Oct, osc1Detune, osc2Oct, osc2Detune, mix;
    juce::Slider attack, decay, sustain, release;
    juce::Slider cutoff, resonance, gain;

    juce::Label titleLabel, subtitleLabel;
    juce::Label osc1Label, osc2Label, blendLabel, envLabel, filterLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<ComboAttachment> osc1WaveA, osc2WaveA;
    std::unique_ptr<SliderAttachment> osc1OctA, osc1DetuneA, osc2OctA, osc2DetuneA, mixA;
    std::unique_ptr<SliderAttachment> attackA, decayA, sustainA, releaseA;
    std::unique_ptr<SliderAttachment> cutoffA, resonanceA, gainA;

    void setupKnob (juce::Slider&, const juce::String& suffix = {});
    void setupSectionLabel (juce::Label&, const juce::String& text);
    void drawPanel (juce::Graphics&, juce::Rectangle<float>) const;
    void drawWaveMark (juce::Graphics&, juce::Rectangle<float>) const;
    void drawKnobLabel (juce::Graphics&, const juce::String&, juce::Slider&) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SonovaAudioProcessorEditor)
};
