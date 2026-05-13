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

#include "PluginEditor.h"
#include "ui/MolluskLookAndFeel.h"

MolluskVoiceChangerEditor::MolluskVoiceChangerEditor(MolluskVoiceChangerProcessor& p)
    : AudioProcessorEditor(&p), proc(p)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&lookAndFeel);

    // ---- Top bar ----
    exportButton.setButtonText("EXPORT\nPRESET");
    importButton.setButtonText("IMPORT\nPRESET");
    exportButton.onClick = [this] { proc.presetManager.exportToFile(); };
    importButton.onClick = [this] { proc.presetManager.importFromFile(); };
    addAndMakeVisible(exportButton);
    addAndMakeVisible(importButton);

    modeButton.setComponentID("mode-button");
    modeButton.onClick = [this] { setBasicMode(!basicMode); };
    addAndMakeVisible(modeButton);

    bypassAllButton.onStateChange = [this] {
        if (auto* param = proc.apvts.getParameter("bypass_all"))
            param->setValueNotifyingHost(bypassAllButton.getToggleState() ? 1.0f : 0.0f);
    };
    addAndMakeVisible(bypassAllButton);

    // ---- Preset buttons ----
    static const char* const japNames[] = { u8"イカガール", u8"イカボーイ", u8"タコガール", u8"タコボーイ" };
    static const char* const ids[]      = { "preset-squid-girl","preset-squid-boy",
                                            "preset-octopus-girl","preset-octopus-boy" };
    // onApplyPreset は PluginProcessor のコンストラクタで設定済みのため上書きしない

    // Export: APVTS ステートを XML として書き出す
    proc.presetManager.onBuildXml = [this]() -> std::unique_ptr<juce::XmlElement> {
        auto root = std::make_unique<juce::XmlElement>("MolluskVoiceChangerPreset");
        root->setAttribute("version", 2);
        if (auto stateXml = proc.apvts.copyState().createXml())
            root->addChildElement(stateXml.release());
        return root;
    };

    // Import: XML から APVTS ステートを復元する
    proc.presetManager.onApplyXml = [this](const juce::XmlElement& root) {
        if (auto* child = root.getFirstChildElement())
        {
            auto state = juce::ValueTree::fromXml(*child);
            if (state.isValid())
                proc.apvts.replaceState(state);
        }
    };

    for (int i = 0; i < 4; ++i)
    {
        presetButtons[i].setButtonText(
            juce::String(PresetManager::presets[i].name) + "\n" +
            juce::String::fromUTF8(japNames[i]));
        presetButtons[i].setComponentID(ids[i]);
        presetButtons[i].onClick = [this, i] {
            proc.presetManager.triggerPreset(i);
            juce::MessageManager::callAsync([this, i] { setActivePreset(i); });
        };
        addAndMakeVisible(presetButtons[i]);
    }

    // ---- Panels ----
    addAndMakeVisible(basic);

    advViewport.setViewedComponent(&adv, false);
    advViewport.setScrollBarsShown(true, false);
    addAndMakeVisible(advViewport);

    // ---- APVTS attachments ----
    createAttachments();

    // Bypass All attachment (connect ToggleButton ↔ APVTS)
    attBypassAll = std::make_unique<ButtonAtt>(proc.apvts, "bypass_all", bypassAllButton);

    setSize(580, 485);
    setBasicMode(true);
    startTimerHz(30);
}

MolluskVoiceChangerEditor::~MolluskVoiceChangerEditor()
{
    stopTimer();
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void MolluskVoiceChangerEditor::createAttachments()
{
    auto& a = proc.apvts;

    // Gate
    attGateBypass    = std::make_unique<ButtonAtt>(a, "gate_bypass",       adv.gateBypass);
    attGateThreshold = std::make_unique<SliderAtt>(a, "gate_threshold",    adv.gateThreshold);
    attGateAttack    = std::make_unique<SliderAtt>(a, "gate_attack",       adv.gateAttack);
    attGateRelease   = std::make_unique<SliderAtt>(a, "gate_release",      adv.gateRelease);

    // HPF
    attHpfBypass     = std::make_unique<ButtonAtt>(a, "hpf_bypass",        adv.hpfBypass);
    attHpfCutoff     = std::make_unique<SliderAtt>(a, "hpf_cutoff",        adv.hpfCutoff);

    // Pitch
    attPitchBypass   = std::make_unique<ButtonAtt>(a, "pitch_bypass",      adv.pitchBypass);
    attPitchSemitones= std::make_unique<SliderAtt>(a, "pitch_semitones",   adv.pitchSlider);

    // Formant
    attFormantBypass     = std::make_unique<ButtonAtt>(a, "formant_bypass",    adv.formantBypass);
    attFormantSemitones  = std::make_unique<SliderAtt>(a, "formant_semitones", adv.formantSlider);

    // LFO
    attLfoBypass     = std::make_unique<ButtonAtt>(a, "lfo_bypass",        adv.lfoFilterBypass);
    attLfoType       = std::make_unique<ComboAtt> (a, "lfo_type",          adv.lfoFilterTypeBox);
    attLfoCutoff     = std::make_unique<SliderAtt>(a, "lfo_cutoff",        adv.lfoFilterCutoff);
    attLfoResonance  = std::make_unique<SliderAtt>(a, "lfo_resonance",     adv.lfoFilterResonance);
    attLfoRate       = std::make_unique<SliderAtt>(a, "lfo_rate",          adv.lfoFilterRate);
    attLfoDepth      = std::make_unique<SliderAtt>(a, "lfo_depth",         adv.lfoFilterDepth);
    attLfoDryWet     = std::make_unique<SliderAtt>(a, "lfo_drywet",        adv.lfoFilterDryWet);
    attLfoRandom     = std::make_unique<ButtonAtt>(a, "lfo_random",        adv.lfoFilterRandomRate);

    // Phaser
    attPhaserBypass  = std::make_unique<ButtonAtt>(a, "phaser_bypass",     adv.phaserBypass);
    attPhaserRate    = std::make_unique<SliderAtt>(a, "phaser_rate",       adv.phaserRate);
    attPhaserDepth   = std::make_unique<SliderAtt>(a, "phaser_depth",      adv.phaserDepth);
    attPhaserFeedback= std::make_unique<SliderAtt>(a, "phaser_feedback",   adv.phaserFeedback);
    attPhaserDryWet  = std::make_unique<SliderAtt>(a, "phaser_drywet",     adv.phaserDryWet);

    // BPF
    attBpfBypass     = std::make_unique<ButtonAtt>(a, "bpf_bypass",        adv.bpfBypass);
    attBpfCentre     = std::make_unique<SliderAtt>(a, "bpf_centre",        adv.bpfCentre);
    attBpfQ          = std::make_unique<SliderAtt>(a, "bpf_q",             adv.bpfQ);
    attBpfDryWet     = std::make_unique<SliderAtt>(a, "bpf_drywet",        adv.bpfDryWet);

    // Gain
    attGainBypass    = std::make_unique<ButtonAtt>(a, "gain_bypass",       adv.gainBypass);
    attGainDb        = std::make_unique<SliderAtt>(a, "gain_db",           adv.gainSlider);

    // Basic knobs attached to same parameters as Advanced sliders → auto-sync
    attBkSensitivity = std::make_unique<SliderAtt>(a, "gate_threshold",    basic.bkSensitivity.knob);
    attBkPitch       = std::make_unique<SliderAtt>(a, "pitch_semitones",   basic.bkPitch.knob);
    attBkBrightness  = std::make_unique<SliderAtt>(a, "formant_semitones", basic.bkBrightness.knob);
    attBkBubble      = std::make_unique<SliderAtt>(a, "lfo_drywet",        basic.bkBubble.knob);
    attBkSlimy       = std::make_unique<SliderAtt>(a, "phaser_drywet",     basic.bkSlimy.knob);
    attBkLofi        = std::make_unique<SliderAtt>(a, "bpf_drywet",        basic.bkLofi.knob);
    attBkVolume      = std::make_unique<SliderAtt>(a, "gain_db",           basic.bkVolume.knob);
}

// ---- Timer ----

void MolluskVoiceChangerEditor::timerCallback()
{
    basic.levelMeter.setLevelDb(proc.inputLevelDb.load());
    basic.levelMeter.setGateOpen(proc.effectChain.gate.getGateGain() > 0.5f);
    basic.levelMeter.setThresholdDb(proc.effectChain.gate.getThresholdDb());
    basic.levelMeter.repaint();

    // Refresh value labels on Basic knobs (since SliderAttachment replaces onValueChange)
    basic.bkSensitivity.refresh();
    basic.bkPitch.refresh();
    basic.bkBrightness.refresh();
    basic.bkBubble.refresh();
    basic.bkSlimy.refresh();
    basic.bkLofi.refresh();
    basic.bkVolume.refresh();
}

// ---- Preset ----

void MolluskVoiceChangerEditor::setActivePreset(int index)
{
    static const char* const ids[] = { "preset-squid-girl","preset-squid-boy",
                                       "preset-octopus-girl","preset-octopus-boy" };
    static const uint32_t squidOn   = MolluskLookAndFeel::colSquidOn;
    static const uint32_t octopusOn = MolluskLookAndFeel::colOctopusOn;
    static const uint32_t panel     = MolluskLookAndFeel::colPanel;

    for (int i = 0; i < 4; ++i)
    {
        juce::Colour c;
        if (i == index)
            c = (i < 2) ? juce::Colour(squidOn) : juce::Colour(octopusOn);
        else
            c = juce::Colour(panel);
        presetButtons[i].setColour(juce::TextButton::buttonColourId, c);
    }
}

void MolluskVoiceChangerEditor::clearActivePreset()
{
    for (auto& btn : presetButtons)
        btn.setColour(juce::TextButton::buttonColourId,
                      juce::Colour(MolluskLookAndFeel::colPanel));
}

// ---- Mode toggle ----

void MolluskVoiceChangerEditor::setBasicMode(bool isBasic)
{
    basicMode = isBasic;
    modeButton.setButtonText(isBasic ? "Advanced" : "Basic");

    if (isBasic)
        proc.effectChain.lfoFilter.setRandomRateEnabled(true);

    basic.setVisible(isBasic);
    advViewport.setVisible(!isBasic);

    setSize(580, isBasic ? 485 : 850);
    resized();
}

// ---- Layout ----

juce::Rectangle<int> MolluskVoiceChangerEditor::layoutCommonHeader(juce::Rectangle<int> area)
{
    {
        auto topBar = area.removeFromTop(48);
        exportButton.setBounds(topBar.removeFromLeft(110));
        topBar.removeFromLeft(4);
        importButton.setBounds(topBar.removeFromLeft(110));
        topBar.removeFromLeft(6);
        bypassAllButton.setBounds(topBar.removeFromLeft(90));
        topBar.removeFromLeft(6);
        modeButton.setBounds(topBar.removeFromLeft(90));
    }
    area.removeFromTop(4);
    {
        auto row  = area.removeFromTop(54);
        const int btnW = (row.getWidth() - 12) / 4;
        for (int i = 0; i < 4; ++i)
        {
            presetButtons[i].setBounds(row.removeFromLeft(btnW));
            if (i < 3) row.removeFromLeft(4);
        }
    }
    area.removeFromTop(6);
    return area;
}

void MolluskVoiceChangerEditor::layoutBasic(juce::Rectangle<int> area)
{
    basic.setBounds(area);
}

void MolluskVoiceChangerEditor::layoutAdvanced(juce::Rectangle<int> area)
{
    advViewport.setBounds(area);
    const int contentW = area.getWidth() - advViewport.getScrollBarThickness();
    adv.setBounds(0, 0, contentW, AdvancedPanel::preferredHeight);
}

void MolluskVoiceChangerEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MolluskVoiceChangerEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    area = layoutCommonHeader(area);

    if (basicMode) layoutBasic(area);
    else           layoutAdvanced(area);
}
