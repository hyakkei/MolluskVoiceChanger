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

// Contains all Advanced-mode UI controls (sliders, bypass toggles, LFO combobox).
// Owns its own layout logic via layoutIn(area).
// MainComponent wires onValueChange callbacks after construction.
class AdvancedPanel : public juce::Component
{
public:
    AdvancedPanel();

    // Full content height needed to display all sections without clipping
    static constexpr int preferredHeight = 1040;

    void paint(juce::Graphics&) override;
    void resized() override;

    // Place all children within the given area (in this component's local coordinates)
    void layoutIn(juce::Rectangle<int> area);

    // ---- Noise Gate ----
    juce::Label        gateLabel;
    juce::ToggleButton gateBypass      { "Bypass" };
    juce::Slider       gateThreshold,   gateAttack,     gateRelease;
    juce::Label        gateThreshLabel,  gateAttackLabel, gateReleaseLabel;

    // ---- HPF ----
    juce::Label        hpfLabel;
    juce::ToggleButton hpfBypass       { "Bypass" };
    juce::Slider       hpfCutoff;
    juce::Label        hpfCutoffLabel;

    // ---- Pitch Shifter ----
    juce::Label        pitchLabel;
    juce::ToggleButton pitchBypass     { "Bypass" };
    juce::Slider       pitchSlider;
    juce::Label        pitchSliderLabel;

    // ---- Formant Shifter ----
    juce::Label        formantLabel;
    juce::ToggleButton formantBypass   { "Bypass" };
    juce::Slider       formantSlider;
    juce::Label        formantSliderLabel;

    // ---- LFO Filter ----
    juce::Label        lfoFilterLabel;
    juce::ToggleButton lfoFilterBypass { "Bypass" };
    juce::ComboBox     lfoFilterTypeBox;
    juce::Label        lfoFilterTypeLabel;
    juce::Slider       lfoFilterCutoff,     lfoFilterResonance;
    juce::Label        lfoFilterCutoffLabel, lfoFilterResonanceLabel;
    juce::Slider       lfoFilterRate,       lfoFilterDepth,      lfoFilterDryWet;
    juce::Label        lfoFilterRateLabel,   lfoFilterDepthLabel, lfoFilterDryWetLabel;
    juce::ToggleButton lfoFilterRandomRate   { "Random" };

    // ---- Phaser ----
    juce::Label        phaserLabel;
    juce::ToggleButton phaserBypass    { "Bypass" };
    juce::Slider       phaserRate,     phaserDepth,     phaserFeedback, phaserDryWet;
    juce::Label        phaserRateLabel, phaserDepthLabel, phaserFeedbackLabel, phaserDryWetLabel;

    // ---- BPF ----
    juce::Label        bpfLabel;
    juce::ToggleButton bpfBypass       { "Bypass" };
    juce::Slider       bpfCentre,      bpfQ,      bpfDryWet;
    juce::Label        bpfCentreLabel,  bpfQLabel, bpfDryWetLabel;

    // ---- Output Gain ----
    juce::Label        gainLabel;
    juce::ToggleButton gainBypass      { "Bypass" };
    juce::Slider       gainSlider;
    juce::Label        gainSliderLabel;

private:
    juce::Array<juce::Rectangle<int>> sectionHeaderRects;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdvancedPanel)
};
