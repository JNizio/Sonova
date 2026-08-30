#include "PluginEditor.h"

namespace SonovaColours
{
    const auto background = juce::Colour (0xff0b0e13);
    const auto panel      = juce::Colour (0xff121720);
    const auto panelEdge  = juce::Colour (0xff252c38);
    const auto text       = juce::Colour (0xfff2f5f7);
    const auto muted      = juce::Colour (0xff818b9d);
    const auto accent     = juce::Colour (0xff70d5ff);
    const auto accent2    = juce::Colour (0xff8c7bff);
}

SonovaLookAndFeel::SonovaLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, SonovaColours::text);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::ComboBox::textColourId, SonovaColours::text);
    setColour (juce::ComboBox::backgroundColourId, juce::Colour (0xff171d27));
    setColour (juce::ComboBox::outlineColourId, SonovaColours::panelEdge);
    setColour (juce::PopupMenu::backgroundColourId, juce::Colour (0xff171d27));
    setColour (juce::PopupMenu::textColourId, SonovaColours::text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour (0xff293444));
}

void SonovaLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                          juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height);
    bounds = bounds.removeFromTop (juce::jmax (40.0f, bounds.getHeight() - 24.0f)).reduced (7.0f);

    const float size = juce::jmin (bounds.getWidth(), bounds.getHeight());
    auto dial = juce::Rectangle<float> (size, size).withCentre (bounds.getCentre());
    const float cx = dial.getCentreX();
    const float cy = dial.getCentreY();
    const float radius = size * 0.5f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const float arcRadius = radius - juce::jmax (4.0f, size * 0.075f);
    const float arcWidth = juce::jmax (2.3f, size * 0.052f);

    g.setColour (juce::Colour (0xff080b10));
    g.fillEllipse (dial.expanded (1.5f));
    g.setColour (juce::Colour (0xff303844));
    g.drawEllipse (dial.expanded (1.0f), 1.0f);

    juce::Path track;
    track.addCentredArc (cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (juce::Colour (0xff2d3540));
    g.strokePath (track, juce::PathStrokeType (arcWidth, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));

    juce::Path active;
    active.addCentredArc (cx, cy, arcRadius, arcRadius, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (SonovaColours::accent.withAlpha (0.18f));
    g.strokePath (active, juce::PathStrokeType (arcWidth + 3.0f, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));
    g.setColour (SonovaColours::accent);
    g.strokePath (active, juce::PathStrokeType (arcWidth, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

    auto body = dial.reduced (arcWidth + size * 0.06f);
    juce::ColourGradient bodyGradient (juce::Colour (0xff303946), body.getX(), body.getY(),
                                       juce::Colour (0xff151b24), body.getRight(), body.getBottom(), false);
    bodyGradient.addColour (0.48, juce::Colour (0xff222a35));
    g.setGradientFill (bodyGradient);
    g.fillEllipse (body);
    g.setColour (juce::Colour (0xff46505e));
    g.drawEllipse (body, 1.2f);
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.drawEllipse (body.reduced (2.0f), 1.0f);

    const float capSize = juce::jmax (4.0f, body.getWidth() * 0.10f);
    g.setColour (juce::Colour (0xff111720));
    g.fillEllipse (cx - capSize * 0.5f, cy - capSize * 0.5f, capSize, capSize);

    const float needleStart = body.getWidth() * 0.12f;
    const float needleEnd = body.getWidth() * 0.39f;
    const float sinA = std::sin (angle);
    const float cosA = std::cos (angle);
    const juce::Point<float> p1 (cx + sinA * needleStart, cy - cosA * needleStart);
    const juce::Point<float> p2 (cx + sinA * needleEnd,   cy - cosA * needleEnd);

    juce::Path needle;
    needle.startNewSubPath (p1);
    needle.lineTo (p2);
    g.setColour (SonovaColours::text);
    g.strokePath (needle, juce::PathStrokeType (juce::jmax (2.0f, size * 0.035f),
                                                juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));

    const float dot = juce::jmax (3.0f, size * 0.055f);
    g.setColour (SonovaColours::accent);
    g.fillEllipse (p2.x - dot * 0.5f, p2.y - dot * 0.5f, dot, dot);

    juce::ignoreUnused (slider);
}

void SonovaLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool,
                                      int buttonX, int buttonY, int buttonW, int buttonH,
                                      juce::ComboBox& box)
{
    auto bounds = juce::Rectangle<float> (0.0f, 0.0f, (float) width, (float) height);
    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRoundedRectangle (bounds, 7.0f);
    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle (bounds.reduced (0.5f), 7.0f, 1.0f);

    juce::Path arrow;
    const float cx = (float) buttonX + (float) buttonW * 0.5f;
    const float cy = (float) buttonY + (float) buttonH * 0.5f;
    arrow.startNewSubPath (cx - 4.0f, cy - 2.0f);
    arrow.lineTo (cx, cy + 2.0f);
    arrow.lineTo (cx + 4.0f, cy - 2.0f);
    g.setColour (SonovaColours::accent);
    g.strokePath (arrow, juce::PathStrokeType (1.7f));
}

SonovaAudioProcessorEditor::SonovaAudioProcessorEditor (SonovaAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&sonovaLookAndFeel);
    setResizable (true, true);
    setResizeLimits (760, 690, 1200, 920);
    setSize (920, 760);

    titleLabel.setText ("SONOVA", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (juce::FontOptions (34.0f).withStyle ("Bold")));
    titleLabel.setColour (juce::Label::textColourId, SonovaColours::text);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText ("DUAL-OSC SYNTHESIZER  /  0.3", juce::dontSendNotification);
    subtitleLabel.setFont (juce::Font (juce::FontOptions (11.5f)));
    subtitleLabel.setColour (juce::Label::textColourId, SonovaColours::muted);
    subtitleLabel.setJustificationType (juce::Justification::centredRight);
    addAndMakeVisible (subtitleLabel);

    for (auto* combo : { &osc1Wave, &osc2Wave })
    {
        combo->addItemList ({ "Sine", "Saw", "Square", "Triangle" }, 1);
        combo->setJustificationType (juce::Justification::centredLeft);
        addAndMakeVisible (*combo);
    }

    setupKnob (osc1Oct, " oct");
    setupKnob (osc1Detune, " ct");
    setupKnob (osc2Oct, " oct");
    setupKnob (osc2Detune, " ct");
    setupKnob (mix);
    setupKnob (attack, " s");
    setupKnob (decay, " s");
    setupKnob (sustain);
    setupKnob (release, " s");
    setupKnob (cutoff, " Hz");
    setupKnob (resonance);
    setupKnob (gain, " dB");
    setupKnob (reverbMix, "%");
    setupKnob (reverbSize, "%");
    setupKnob (reverbDamping, "%");
    setupKnob (reverbWidth, "%");

    for (auto* slider : { &reverbMix, &reverbSize, &reverbDamping, &reverbWidth })
        slider->setNumDecimalPlacesToDisplay (2);

    reverbBypass.setButtonText ("BYPASS");
    reverbBypass.setColour (juce::ToggleButton::textColourId, SonovaColours::muted);
    reverbBypass.setColour (juce::ToggleButton::tickColourId, SonovaColours::accent);
    addAndMakeVisible (reverbBypass);

    setupSectionLabel (osc1Label, "OSCILLATOR A");
    setupSectionLabel (osc2Label, "OSCILLATOR B");
    setupSectionLabel (blendLabel, "BLEND");
    setupSectionLabel (envLabel, "AMPLITUDE");
    setupSectionLabel (filterLabel, "FILTER / OUTPUT");
    setupSectionLabel (reverbLabel, "SPACE / REVERB");

    auto& s = processor.apvts;
    osc1WaveA = std::make_unique<ComboAttachment> (s, "osc1Wave", osc1Wave);
    osc2WaveA = std::make_unique<ComboAttachment> (s, "osc2Wave", osc2Wave);
    osc1OctA = std::make_unique<SliderAttachment> (s, "osc1Oct", osc1Oct);
    osc1DetuneA = std::make_unique<SliderAttachment> (s, "osc1Detune", osc1Detune);
    osc2OctA = std::make_unique<SliderAttachment> (s, "osc2Oct", osc2Oct);
    osc2DetuneA = std::make_unique<SliderAttachment> (s, "osc2Detune", osc2Detune);
    mixA = std::make_unique<SliderAttachment> (s, "mix", mix);
    attackA = std::make_unique<SliderAttachment> (s, "attack", attack);
    decayA = std::make_unique<SliderAttachment> (s, "decay", decay);
    sustainA = std::make_unique<SliderAttachment> (s, "sustain", sustain);
    releaseA = std::make_unique<SliderAttachment> (s, "release", release);
    cutoffA = std::make_unique<SliderAttachment> (s, "cutoff", cutoff);
    resonanceA = std::make_unique<SliderAttachment> (s, "resonance", resonance);
    gainA = std::make_unique<SliderAttachment> (s, "gain", gain);
    reverbMixA = std::make_unique<SliderAttachment> (s, "reverbMix", reverbMix);
    reverbSizeA = std::make_unique<SliderAttachment> (s, "reverbSize", reverbSize);
    reverbDampingA = std::make_unique<SliderAttachment> (s, "reverbDamping", reverbDamping);
    reverbWidthA = std::make_unique<SliderAttachment> (s, "reverbWidth", reverbWidth);
    reverbBypassA = std::make_unique<ButtonAttachment> (s, "reverbBypass", reverbBypass);
}

SonovaAudioProcessorEditor::~SonovaAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void SonovaAudioProcessorEditor::setupKnob (juce::Slider& slider, const juce::String& suffix)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                                juce::MathConstants<float>::pi * 2.8f, true);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 76, 20);
    slider.setTextValueSuffix (suffix);
    addAndMakeVisible (slider);
}

void SonovaAudioProcessorEditor::setupSectionLabel (juce::Label& label, const juce::String& textValue)
{
    label.setText (textValue, juce::dontSendNotification);
    label.setFont (juce::Font (juce::FontOptions (12.0f).withStyle ("Bold")));
    label.setColour (juce::Label::textColourId, SonovaColours::muted);
    label.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (label);
}

void SonovaAudioProcessorEditor::drawPanel (juce::Graphics& g, juce::Rectangle<float> bounds) const
{
    g.setColour (SonovaColours::panel);
    g.fillRoundedRectangle (bounds, 14.0f);
    g.setColour (SonovaColours::panelEdge);
    g.drawRoundedRectangle (bounds.reduced (0.5f), 14.0f, 1.0f);
}

void SonovaAudioProcessorEditor::drawWaveMark (juce::Graphics& g, juce::Rectangle<float> area) const
{
    juce::Path wave;
    const float mid = area.getCentreY();
    const float amp = area.getHeight() * 0.28f;
    for (int i = 0; i <= 80; ++i)
    {
        const float t = (float) i / 80.0f;
        const float x = area.getX() + t * area.getWidth();
        const float y = mid + std::sin (t * juce::MathConstants<float>::twoPi * 2.0f) * amp
                            * (0.35f + 0.65f * std::sin (t * juce::MathConstants<float>::pi));
        if (i == 0) wave.startNewSubPath (x, y); else wave.lineTo (x, y);
    }
    g.setColour (SonovaColours::accent.withAlpha (0.9f));
    g.strokePath (wave, juce::PathStrokeType (2.0f));
}

void SonovaAudioProcessorEditor::drawKnobLabel (juce::Graphics& g, const juce::String& textValue,
                                                juce::Slider& slider) const
{
    auto b = slider.getBounds();
    g.setColour (SonovaColours::muted);
    g.setFont (11.0f);
    g.drawText (textValue, b.getX(), b.getBottom() + 1, b.getWidth(), 16, juce::Justification::centred);
}

void SonovaAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (SonovaColours::background);

    auto top = juce::Rectangle<float> (24.0f, 18.0f, (float) getWidth() - 48.0f, 52.0f);
    drawWaveMark (g, top.removeFromRight (150.0f).reduced (6.0f, 13.0f));

    const float margin = 24.0f;
    const float gap = 14.0f;
    const float contentW = (float) getWidth() - margin * 2.0f;
    const float topY = 88.0f;
    const float upperH = 225.0f;
    const float lowerY = 329.0f;
    const float lowerH = 200.0f;
    const float reverbY = 543.0f;
    const float reverbH = (float) getHeight() - reverbY - 24.0f;

    const float oscW = (contentW - gap * 2.0f) * 0.405f;
    const float blendW = contentW - oscW * 2.0f - gap * 2.0f;

    drawPanel (g, { margin, topY, oscW, upperH });
    drawPanel (g, { margin + oscW + gap, topY, blendW, upperH });
    drawPanel (g, { margin + oscW + gap + blendW + gap, topY, oscW, upperH });

    const float lowerLeftW = contentW * 0.55f;
    drawPanel (g, { margin, lowerY, lowerLeftW, lowerH });
    drawPanel (g, { margin + lowerLeftW + gap, lowerY, contentW - lowerLeftW - gap, lowerH });
    drawPanel (g, { margin, reverbY, contentW, reverbH });

    g.setColour (SonovaColours::accent.withAlpha (0.10f));
    g.fillEllipse ((float) getWidth() * 0.5f - 78.0f, topY + 41.0f, 156.0f, 156.0f);
    g.setColour (SonovaColours::accent2.withAlpha (0.07f));
    g.fillEllipse ((float) getWidth() * 0.5f - 55.0f, topY + 64.0f, 110.0f, 110.0f);

    g.setColour (SonovaColours::accent2.withAlpha (0.08f));
    g.fillRoundedRectangle ({ margin + 10.0f, reverbY + 38.0f, contentW - 20.0f, reverbH - 48.0f }, 12.0f);

    drawKnobLabel (g, "OCTAVE", osc1Oct);
    drawKnobLabel (g, "FINE", osc1Detune);
    drawKnobLabel (g, "OCTAVE", osc2Oct);
    drawKnobLabel (g, "FINE", osc2Detune);
    drawKnobLabel (g, "A", attack);
    drawKnobLabel (g, "D", decay);
    drawKnobLabel (g, "S", sustain);
    drawKnobLabel (g, "R", release);
    drawKnobLabel (g, "CUTOFF", cutoff);
    drawKnobLabel (g, "RESO", resonance);
    drawKnobLabel (g, "OUTPUT", gain);
    drawKnobLabel (g, "MIX", reverbMix);
    drawKnobLabel (g, "SIZE", reverbSize);
    drawKnobLabel (g, "DAMPING", reverbDamping);
    drawKnobLabel (g, "WIDTH", reverbWidth);
}

void SonovaAudioProcessorEditor::resized()
{
    const int w = getWidth();
    const int margin = 24;
    const int gap = 14;
    const int contentW = w - margin * 2;

    titleLabel.setBounds (margin, 16, 260, 42);
    subtitleLabel.setBounds (w - 360, 22, 200, 28);

    const int topY = 88;
    const int oscW = juce::roundToInt ((contentW - gap * 2) * 0.405f);
    const int blendW = contentW - oscW * 2 - gap * 2;
    const int leftX = margin;
    const int blendX = leftX + oscW + gap;
    const int rightX = blendX + blendW + gap;

    osc1Label.setBounds (leftX + 18, topY + 12, 160, 24);
    osc2Label.setBounds (rightX + 18, topY + 12, 160, 24);
    blendLabel.setBounds (blendX + 14, topY + 12, blendW - 28, 24);

    osc1Wave.setBounds (leftX + 18, topY + 44, oscW - 36, 34);
    osc2Wave.setBounds (rightX + 18, topY + 44, oscW - 36, 34);

    const int oscKnobW = (oscW - 54) / 2;
    osc1Oct.setBounds (leftX + 16, topY + 92, oscKnobW, 104);
    osc1Detune.setBounds (leftX + 30 + oscKnobW, topY + 92, oscKnobW, 104);
    osc2Oct.setBounds (rightX + 16, topY + 92, oscKnobW, 104);
    osc2Detune.setBounds (rightX + 30 + oscKnobW, topY + 92, oscKnobW, 104);

    const int mixSize = juce::jmin (blendW - 14, 126);
    mix.setBounds (blendX + (blendW - mixSize) / 2, topY + 66, mixSize, 132);

    const int lowerY = 329;
    const int lowerH = 200;
    const int lowerLeftW = juce::roundToInt (contentW * 0.55f);
    const int lowerRightX = margin + lowerLeftW + gap;
    const int lowerRightW = contentW - lowerLeftW - gap;

    envLabel.setBounds (margin + 18, lowerY + 12, 160, 24);
    filterLabel.setBounds (lowerRightX + 18, lowerY + 12, 190, 24);

    juce::Slider* envSliders[] = { &attack, &decay, &sustain, &release };
    const int envKnobW = (lowerLeftW - 40) / 4;
    for (int i = 0; i < 4; ++i)
        envSliders[i]->setBounds (margin + 12 + i * envKnobW, lowerY + 49, envKnobW, lowerH - 76);

    const int filterKnobW = (lowerRightW - 34) / 3;
    cutoff.setBounds (lowerRightX + 10, lowerY + 49, filterKnobW, lowerH - 76);
    resonance.setBounds (lowerRightX + 12 + filterKnobW, lowerY + 49, filterKnobW, lowerH - 76);
    gain.setBounds (lowerRightX + 14 + filterKnobW * 2, lowerY + 49, filterKnobW, lowerH - 76);

    const int reverbY = 543;
    const int reverbH = getHeight() - reverbY - 24;
    reverbLabel.setBounds (margin + 18, reverbY + 10, 180, 24);
    reverbBypass.setBounds (w - margin - 102, reverbY + 10, 90, 24);

    juce::Slider* reverbSliders[] = { &reverbMix, &reverbSize, &reverbDamping, &reverbWidth };
    const int rvKnobW = (contentW - 44) / 4;
    for (int i = 0; i < 4; ++i)
        reverbSliders[i]->setBounds (margin + 12 + i * rvKnobW, reverbY + 42,
                                     rvKnobW, juce::jmax (98, reverbH - 64));
}
