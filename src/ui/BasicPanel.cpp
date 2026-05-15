// Mollusk Voice Changer
// Copyright (C) 2026 Ito Hyakkei
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include "BasicPanel.h"

static juce::String jp(const char* utf8) { return juce::String::fromUTF8(utf8); }

BasicPanel::BasicPanel()
{
    inputMeterLabel.setText("INPUT", juce::dontSendNotification);
    inputMeterLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(inputMeterLabel);
    addAndMakeVisible(levelMeter);

    outputMeterLabel.setText("OUTPUT", juce::dontSendNotification);
    outputMeterLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(outputMeterLabel);
    outputLevelMeter.setShowGateLed(false);
    addAndMakeVisible(outputLevelMeter);

    setupKnob(bkSensitivity, "INPUT\nSENSITIVITY", jp(u8"にゅうりょくかんど"),  -80.0,  0.0, -40.0, 1.0,  " dB");
    setupKnob(bkPitch,       "PITCH",               jp(u8"たかさ"),       0.0,  12.0,   0.0, 0.5,  " st");
    setupKnob(bkBrightness,  "BRIGHTNESS",          jp(u8"あかるさ"),   -12.0,  12.0,   0.0, 0.5,  " st");
    setupKnob(bkBubble,      "BUBBLE",              jp(u8"ぶくぶく"),     0.0,   1.0,   0.0, 0.01, "");
    setupKnob(bkSlimy,       "SLIMY",               jp(u8"ぬるぬる"),     0.0,   1.0,   0.0, 0.01, "");
    setupKnob(bkLofi,        "LO-FI",               jp(u8"こもりぐあい"),   0.0,   1.0,   0.0, 0.01, "");
    setupKnob(bkVolume,      "VOLUME",              jp(u8"おんりょう"), -60.0,  12.0,   0.0, 0.5,  " dB");
}

void BasicPanel::setupKnob(Knob& k, const juce::String& name, const juce::String& jpName,
                            double min, double max, double val,
                            double step, const juce::String& suffix)
{
    k.knob.setRange(min, max, step);
    k.knob.setValue(val, juce::dontSendNotification);
    k.knob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    k.knob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    k.knob.setDoubleClickReturnValue(true, val);

    k.label.setText(name, juce::dontSendNotification);
    k.label.setJustificationType(juce::Justification::centred);

    k.japLabel.setText(jpName, juce::dontSendNotification);
    k.japLabel.setJustificationType(juce::Justification::centred);
    k.japLabel.setComponentID("jap-label");

    k.suffix = suffix;
    k.valueLabel.setJustificationType(juce::Justification::centred);
    k.valueLabel.setComponentID("val-label");
    k.refresh();

    addAndMakeVisible(k.knob);
    addAndMakeVisible(k.label);
    addAndMakeVisible(k.japLabel);
    addAndMakeVisible(k.valueLabel);
}

void BasicPanel::syncKnobs(float sensitivity, float pitch, float brightness,
                            float bubble,      float slimy,  float lofi, float volume)
{
    auto set = [](Knob& k, float v) {
        k.knob.setValue(v, juce::dontSendNotification);
        k.refresh();
    };
    set(bkSensitivity, sensitivity);
    set(bkPitch,       pitch);
    set(bkBrightness,  brightness);
    set(bkBubble,      bubble);
    set(bkSlimy,       slimy);
    set(bkLofi,        lofi);
    set(bkVolume,      volume);
}

void BasicPanel::resized()
{
    layoutIn(getLocalBounds());
}

void BasicPanel::layoutIn(juce::Rectangle<int> area)
{
    constexpr int labelW   = 60;
    constexpr int labelGap = 4;
    constexpr int meterH   = 26;
    {
        auto row = area.removeFromTop(meterH);
        inputMeterLabel.setBounds(row.removeFromLeft(labelW));
        row.removeFromLeft(labelGap);
        levelMeter.setBounds(row);
    }
    area.removeFromTop(4);
    {
        auto row = area.removeFromTop(meterH);
        outputMeterLabel.setBounds(row.removeFromLeft(labelW));
        row.removeFromLeft(labelGap);
        outputLevelMeter.setBounds(row);
    }
    area.removeFromTop(4);

    const int totalW  = area.getWidth();
    const int cellW   = totalW / 4;   // 4 columns fill the full width
    constexpr int knobH = 74;
    constexpr int lblH  = 28;
    constexpr int japH  = 14;
    constexpr int valH  = 18;
    constexpr int cellH = lblH + japH + knobH + valH + 4;

    auto layoutKnob = [&](juce::Rectangle<int>& row, Knob& k)
    {
        auto cell = row.removeFromLeft(cellW);
        k.label.setBounds(cell.removeFromTop(lblH));
        k.japLabel.setBounds(cell.removeFromTop(japH));
        k.knob.setBounds(cell.removeFromTop(knobH).reduced(4));
        k.valueLabel.setBounds(cell.removeFromTop(valH));
    };

    // Row 1: Sensitivity, Pitch, Brightness, Bubble
    {
        auto row = area.removeFromTop(cellH);
        layoutKnob(row, bkSensitivity);
        layoutKnob(row, bkPitch);
        layoutKnob(row, bkBrightness);
        layoutKnob(row, bkBubble);
    }
    area.removeFromTop(8);
    // Row 2: Slimy, Lo-fi, Volume (centred using same cellW)
    {
        auto row = area.removeFromTop(cellH);
        row.removeFromLeft((row.getWidth() - cellW * 3) / 2);
        layoutKnob(row, bkSlimy);
        layoutKnob(row, bkLofi);
        layoutKnob(row, bkVolume);
    }
}
