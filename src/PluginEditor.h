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
#include "PluginProcessor.h"
#include "PresetManager.h"
#include "ui/BasicPanel.h"
#include "ui/AdvancedPanel.h"
#include "ui/MolluskLookAndFeel.h"

class MolluskVoiceChangerEditor : public juce::AudioProcessorEditor,
                                   public juce::Timer
{
public:
    explicit MolluskVoiceChangerEditor(MolluskVoiceChangerProcessor&);
    ~MolluskVoiceChangerEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    MolluskVoiceChangerProcessor& proc;
    MolluskLookAndFeel lookAndFeel;

    // ---- Top bar (no Audio Settings) ----
    juce::TextButton   exportButton  { "Export Preset" };
    juce::TextButton   importButton  { "Import Preset" };
    juce::ToggleButton bypassAllButton { "Bypass All" };
    juce::TextButton   modeButton    { "Advanced" };
    bool basicMode = true;

    // ---- Preset buttons ----
    juce::TextButton presetButtons[4];

    // ---- Panels ----
    BasicPanel   basic;
    AdvancedPanel adv;
    juce::Viewport advViewport;

    // ---- APVTS attachment aliases ----
    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAtt = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboAtt  = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    // Gate
    std::unique_ptr<ButtonAtt> attGateBypass;
    std::unique_ptr<SliderAtt> attGateThreshold, attGateAttack, attGateRelease;
    // HPF
    std::unique_ptr<ButtonAtt> attHpfBypass;
    std::unique_ptr<SliderAtt> attHpfCutoff;
    // Pitch
    std::unique_ptr<ButtonAtt> attPitchBypass;
    std::unique_ptr<SliderAtt> attPitchSemitones;
    // Formant
    std::unique_ptr<ButtonAtt> attFormantBypass;
    std::unique_ptr<SliderAtt> attFormantSemitones;
    // LFO
    std::unique_ptr<ButtonAtt> attLfoBypass, attLfoRandom;
    std::unique_ptr<ComboAtt>  attLfoType;
    std::unique_ptr<SliderAtt> attLfoCutoff, attLfoResonance, attLfoRate, attLfoDepth, attLfoDryWet;
    // Phaser
    std::unique_ptr<ButtonAtt> attPhaserBypass;
    std::unique_ptr<SliderAtt> attPhaserRate, attPhaserDepth, attPhaserFeedback, attPhaserDryWet;
    // BPF
    std::unique_ptr<ButtonAtt> attBpfBypass;
    std::unique_ptr<SliderAtt> attBpfCentre, attBpfQ, attBpfDryWet;
    // Gain
    std::unique_ptr<ButtonAtt> attGainBypass;
    std::unique_ptr<SliderAtt> attGainDb;

    // Basic knobs (same parameters as Advanced sliders → auto-sync via APVTS)
    std::unique_ptr<SliderAtt> attBkSensitivity, attBkPitch, attBkBrightness;
    std::unique_ptr<SliderAtt> attBkBubble, attBkSlimy, attBkLofi, attBkVolume;

    // Bypass All
    std::unique_ptr<ButtonAtt> attBypassAll;

    void createAttachments();
    void setBasicMode(bool isBasic);
    void syncBasicLabels();
    void setActivePreset(int index);
    void clearActivePreset();

    juce::Rectangle<int> layoutCommonHeader(juce::Rectangle<int> area);
    void layoutBasic(juce::Rectangle<int> area);
    void layoutAdvanced(juce::Rectangle<int> area);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MolluskVoiceChangerEditor)
};
