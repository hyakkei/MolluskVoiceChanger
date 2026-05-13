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

#pragma once
#include <JuceHeader.h>
#include "LevelMeter.h"

// Contains Basic-mode UI: 7 rotary knobs and the VU meter.
// MainComponent wires knob onValueChange callbacks after construction.
class BasicPanel : public juce::Component
{
public:
    BasicPanel();

    void resized() override;

    // Place all children within the given area (in this component's local coordinates)
    void layoutIn(juce::Rectangle<int> area);

    // Sync knob positions from external values (call when mode switches or preset loads)
    void syncKnobs(float sensitivity, float pitch, float brightness,
                   float bubble,      float slimy,  float lofi, float volume);

    // ---- Level meter ----
    LevelMeter levelMeter;

    // ---- Knobs ----
    struct Knob
    {
        juce::Slider knob;
        juce::Label  label;      // English (Bungee)
        juce::Label  japLabel;   // Japanese (RocknRoll One)
        juce::Label  valueLabel;
        juce::String suffix;

        void refresh()
        {
            valueLabel.setText(
                juce::String(knob.getValue(), 1) + suffix,
                juce::dontSendNotification);
        }
    };

    Knob bkSensitivity;
    Knob bkPitch;
    Knob bkBrightness;
    Knob bkBubble;
    Knob bkSlimy;
    Knob bkLofi;
    Knob bkVolume;

private:
    void setupKnob(Knob& k, const juce::String& name, const juce::String& jpName,
                   double min, double max, double val,
                   double step, const juce::String& suffix);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasicPanel)
};
