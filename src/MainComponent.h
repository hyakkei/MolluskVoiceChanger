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
#include "dsp/EffectChain.h"
#include "PresetManager.h"
#include "ui/AdvancedPanel.h"
#include "ui/BasicPanel.h"
#include "ui/MolluskLookAndFeel.h"

class MainComponent : public juce::AudioAppComponent,
                      public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void openAudioSettings();
    std::unique_ptr<juce::XmlElement> buildPresetXml() const;
    void applyPresetXml(const juce::XmlElement& root);
    void applyPreset(const PresetManager::PresetData& p);
    void syncBasicFromAdv();      // sync BasicPanel knobs from AdvancedPanel slider values
    void setBasicMode(bool isBasic);
    void layoutBasic(juce::Rectangle<int> area);
    void layoutAdvanced(juce::Rectangle<int> area);
    void setActivePreset(int index);   // highlight the given preset button
    void clearActivePreset();          // remove all preset button highlights
    void saveLastSession() const;      // 終了時に設定を保存
    void loadLastSession();            // 起動時に設定を復元

    // ---- Look and feel (must be declared before any Component members) ----
    MolluskLookAndFeel lookAndFeel;

    // ---- DSP ----
    EffectChain effectChain;

    // Shared state between audio thread and UI thread
    std::atomic<float> inputLevelDb  { -100.0f };
    std::atomic<float> outputLevelDb { -100.0f };
    std::atomic<bool>  globalBypass  { false };

    // ---- Preset management ----
    PresetManager presetManager;

    // ---- Basic mode panel ----
    BasicPanel basic;

    // ---- Top bar ----
    juce::TextButton audioSettingsButton { "Audio Settings" };
    juce::TextButton exportButton        { "Export Preset" };
    juce::TextButton importButton        { "Import Preset" };
    juce::ToggleButton bypassAllButton   { "Bypass All" };
    juce::TextButton modeButton          { "Advanced" };
    bool basicMode = true;

    // ---- Preset buttons ----
    juce::TextButton presetButtons[4];

    // ---- Advanced mode panel (wrapped in Viewport for scrolling) ----
    AdvancedPanel adv;
    juce::Viewport advViewport;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
