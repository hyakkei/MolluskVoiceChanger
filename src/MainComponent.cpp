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

#include "MainComponent.h"


MainComponent::MainComponent()
{
    juce::LookAndFeel::setDefaultLookAndFeel(&lookAndFeel);

    // ---- PresetManager callbacks ----
    presetManager.onBuildXml    = [this]                         { return buildPresetXml(); };
    presetManager.onApplyXml    = [this](const juce::XmlElement& x) { applyPresetXml(x); };
    presetManager.onApplyPreset = [this](const PresetManager::PresetData& p) { applyPreset(p); };

    // ---- Top bar ----
    audioSettingsButton.setButtonText("AUDIO\nSETTINGS");
    exportButton.setButtonText("EXPORT\nPRESET");
    importButton.setButtonText("IMPORT\nPRESET");
    audioSettingsButton.onClick = [this] { openAudioSettings(); };
    exportButton.onClick        = [this] { presetManager.exportToFile(); };
    importButton.onClick        = [this] { presetManager.importFromFile(); };
    addAndMakeVisible(audioSettingsButton);
    addAndMakeVisible(exportButton);
    addAndMakeVisible(importButton);

    // ---- Mode toggle ----
    modeButton.setComponentID("mode-button");
    modeButton.onClick = [this] { setBasicMode(!basicMode); };
    addAndMakeVisible(modeButton);

    // ---- Global bypass ----
    bypassAllButton.onStateChange = [this] {
        globalBypass.store(bypassAllButton.getToggleState());
    };
    addAndMakeVisible(bypassAllButton);

    // ---- Preset buttons ----
    static const char* const japPresetNames[] = { u8"イカガール", u8"イカボーイ", u8"タコガール", u8"タコボーイ" };
    for (int i = 0; i < 4; ++i)
    {
        String id;
        if (i == 0){id = "preset-squid-girl";}
        else if(i == 1){id = "preset-squid-boy";}
        else if(i == 2){id = "preset-octopus-girl";}
        else if(i == 3){id = "preset-octopus-boy";}        

        presetButtons[i].setButtonText(
            juce::String(PresetManager::presets[i].name) + "\n" +
            juce::String::fromUTF8(japPresetNames[i]));
        presetButtons[i].setComponentID(id);
        presetButtons[i].onClick = [this, i] {
            presetManager.triggerPreset(i);
            // setValue(sendNotification) in applyPreset fires onValueChange asynchronously
            // via triggerAsyncUpdate(). Defer setActivePreset so it runs after those
            // callbacks have been processed.
            juce::MessageManager::callAsync([this, i] { setActivePreset(i); });
        };
        addAndMakeVisible(presetButtons[i]);
    }

    // ---- BasicPanel: set initial values from DSP, then wire callbacks ----
    basic.syncKnobs(effectChain.gate.getThresholdDb(),
                    effectChain.pitchShifter.getSemitones(),
                    effectChain.formantShifter.getSemitones(),
                    effectChain.lfoFilter.getDryWet(),
                    effectChain.phaser.getMix(),
                    effectChain.bpf.getDryWet(),
                    effectChain.outputGain.getGainDb());

    basic.bkSensitivity.knob.onValueChange = [this] {
        effectChain.gate.setThresholdDb((float)basic.bkSensitivity.knob.getValue());
        adv.gateThreshold.setValue(basic.bkSensitivity.knob.getValue(), juce::dontSendNotification);
        basic.bkSensitivity.refresh();
    };
    basic.bkPitch.knob.onValueChange = [this] {
        effectChain.pitchShifter.setSemitones((float)basic.bkPitch.knob.getValue());
        adv.pitchSlider.setValue(basic.bkPitch.knob.getValue(), juce::dontSendNotification);
        basic.bkPitch.refresh();
        clearActivePreset();
    };
    basic.bkBrightness.knob.onValueChange = [this] {
        effectChain.formantShifter.setSemitones((float)basic.bkBrightness.knob.getValue());
        adv.formantSlider.setValue(basic.bkBrightness.knob.getValue(), juce::dontSendNotification);
        basic.bkBrightness.refresh();
        clearActivePreset();
    };
    basic.bkBubble.knob.onValueChange = [this] {
        effectChain.lfoFilter.setDryWet((float)basic.bkBubble.knob.getValue());
        adv.lfoFilterDryWet.setValue(basic.bkBubble.knob.getValue(), juce::dontSendNotification);
        basic.bkBubble.refresh();
        clearActivePreset();
    };
    basic.bkSlimy.knob.onValueChange = [this] {
        effectChain.phaser.setMix((float)basic.bkSlimy.knob.getValue());
        adv.phaserDryWet.setValue(basic.bkSlimy.knob.getValue(), juce::dontSendNotification);
        basic.bkSlimy.refresh();
        clearActivePreset();
    };
    basic.bkLofi.knob.onValueChange = [this] {
        effectChain.bpf.setDryWet((float)basic.bkLofi.knob.getValue());
        adv.bpfDryWet.setValue(basic.bkLofi.knob.getValue(), juce::dontSendNotification);
        basic.bkLofi.refresh();
        clearActivePreset();
    };
    basic.bkVolume.knob.onValueChange = [this] {
        effectChain.outputGain.setGainDb((float)basic.bkVolume.knob.getValue());
        adv.gainSlider.setValue(basic.bkVolume.knob.getValue(), juce::dontSendNotification);
        basic.bkVolume.refresh();
    };

    addAndMakeVisible(basic);

    // ---- AdvancedPanel: wire DSP callbacks ----
    // onStateChange はホバー等の視覚的状態変化でも発火するため、DSP 更新のみに留める
    // clearActivePreset() はユーザーが実際にクリックしたときだけ呼ぶ → onClick を使用
    adv.gateBypass.onStateChange     = [this] { effectChain.gate.setEnabled(!adv.gateBypass.getToggleState()); };
    adv.gateBypass.onClick           = [this] { clearActivePreset(); };
    adv.gateThreshold.onValueChange  = [this] { effectChain.gate.setThresholdDb((float)adv.gateThreshold.getValue()); };  // 入力感度 → リセットしない
    adv.gateAttack.onValueChange     = [this] { effectChain.gate.setAttackMs((float)adv.gateAttack.getValue()); clearActivePreset(); };
    adv.gateRelease.onValueChange    = [this] { effectChain.gate.setReleaseMs((float)adv.gateRelease.getValue()); clearActivePreset(); };

    adv.hpfBypass.onStateChange      = [this] { effectChain.hpf.setEnabled(!adv.hpfBypass.getToggleState()); };
    adv.hpfBypass.onClick            = [this] { clearActivePreset(); };
    adv.hpfCutoff.onValueChange      = [this] { effectChain.hpf.setCutoffHz((float)adv.hpfCutoff.getValue()); clearActivePreset(); };

    adv.pitchBypass.onStateChange    = [this] { effectChain.pitchShifter.setEnabled(!adv.pitchBypass.getToggleState()); };
    adv.pitchBypass.onClick          = [this] { clearActivePreset(); };
    adv.pitchSlider.onValueChange    = [this] { effectChain.pitchShifter.setSemitones((float)adv.pitchSlider.getValue()); clearActivePreset(); };

    adv.formantBypass.onStateChange  = [this] { effectChain.formantShifter.setEnabled(!adv.formantBypass.getToggleState()); };
    adv.formantBypass.onClick        = [this] { clearActivePreset(); };
    adv.formantSlider.onValueChange  = [this] { effectChain.formantShifter.setSemitones((float)adv.formantSlider.getValue()); clearActivePreset(); };

    adv.lfoFilterBypass.onStateChange    = [this] { effectChain.lfoFilter.setEnabled(!adv.lfoFilterBypass.getToggleState()); };
    adv.lfoFilterBypass.onClick          = [this] { clearActivePreset(); };
    adv.lfoFilterTypeBox.onChange        = [this] { effectChain.lfoFilter.setFilterType((LFOFilter::FilterType)(adv.lfoFilterTypeBox.getSelectedId() - 1)); clearActivePreset(); };
    adv.lfoFilterCutoff.onValueChange    = [this] { effectChain.lfoFilter.setCutoffHz((float)adv.lfoFilterCutoff.getValue()); clearActivePreset(); };
    adv.lfoFilterResonance.onValueChange = [this] { effectChain.lfoFilter.setResonance((float)adv.lfoFilterResonance.getValue()); clearActivePreset(); };
    adv.lfoFilterRate.onValueChange      = [this] { effectChain.lfoFilter.setLFORate((float)adv.lfoFilterRate.getValue()); clearActivePreset(); };
    adv.lfoFilterDepth.onValueChange     = [this] { effectChain.lfoFilter.setLFODepth((float)adv.lfoFilterDepth.getValue()); clearActivePreset(); };
    adv.lfoFilterDryWet.onValueChange    = [this] { effectChain.lfoFilter.setDryWet((float)adv.lfoFilterDryWet.getValue()); clearActivePreset(); };
    adv.lfoFilterRandomRate.onStateChange= [this] { effectChain.lfoFilter.setRandomRateEnabled(adv.lfoFilterRandomRate.getToggleState()); };
    adv.lfoFilterRandomRate.onClick      = [this] { clearActivePreset(); };

    adv.phaserBypass.onStateChange   = [this] { effectChain.phaser.setEnabled(!adv.phaserBypass.getToggleState()); };
    adv.phaserBypass.onClick         = [this] { clearActivePreset(); };
    adv.phaserRate.onValueChange     = [this] { effectChain.phaser.setRate((float)adv.phaserRate.getValue()); clearActivePreset(); };
    adv.phaserDepth.onValueChange    = [this] { effectChain.phaser.setDepth((float)adv.phaserDepth.getValue()); clearActivePreset(); };
    adv.phaserFeedback.onValueChange = [this] { effectChain.phaser.setFeedback((float)adv.phaserFeedback.getValue()); clearActivePreset(); };
    adv.phaserDryWet.onValueChange   = [this] { effectChain.phaser.setMix((float)adv.phaserDryWet.getValue()); clearActivePreset(); };

    adv.bpfBypass.onStateChange      = [this] { effectChain.bpf.setEnabled(!adv.bpfBypass.getToggleState()); };
    adv.bpfBypass.onClick            = [this] { clearActivePreset(); };
    adv.bpfCentre.onValueChange      = [this] { effectChain.bpf.setCentreHz((float)adv.bpfCentre.getValue()); clearActivePreset(); };
    adv.bpfQ.onValueChange           = [this] { effectChain.bpf.setQ((float)adv.bpfQ.getValue()); clearActivePreset(); };
    adv.bpfDryWet.onValueChange      = [this] { effectChain.bpf.setDryWet((float)adv.bpfDryWet.getValue()); clearActivePreset(); };

    adv.gainBypass.onStateChange     = [this] { effectChain.outputGain.setEnabled(!adv.gainBypass.getToggleState()); };
    adv.gainBypass.onClick           = [this] { clearActivePreset(); };
    adv.gainSlider.onValueChange     = [this] { effectChain.outputGain.setGainDb((float)adv.gainSlider.getValue()); };

    advViewport.setViewedComponent(&adv, false);
    advViewport.setScrollBarsShown(true, false);
    addAndMakeVisible(advViewport);

    setSize(580, 1115);
    setBasicMode(true);   // start in Basic mode
    setAudioChannels(1, 2);
    startTimerHz(30);  // 30fps for meter updates

    loadLastSession();
}

MainComponent::~MainComponent()
{
    saveLastSession();
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    stopTimer();
    shutdownAudio();
}

// ---- AudioAppComponent ----

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlockExpected;
    spec.numChannels      = 1;
    effectChain.prepare(spec);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* buffer      = bufferToFill.buffer;
    const int start   = bufferToFill.startSample;
    const int numSamp = bufferToFill.numSamples;

    juce::AudioBuffer<float> mono(1, numSamp);
    mono.copyFrom(0, 0, *buffer, 0, start, numSamp);

    // Measure peak input level (before effects) for VU meter
    float peak = 0.0f;
    const float* src = mono.getReadPointer(0);
    for (int i = 0; i < numSamp; ++i)
        peak = std::max(peak, std::abs(src[i]));
    inputLevelDb.store(juce::Decibels::gainToDecibels(peak, -100.0f));

    if (!globalBypass.load())
        effectChain.process(mono);

    // Measure peak output level (after effects) for output VU meter
    float outPeak = 0.0f;
    const float* out = mono.getReadPointer(0);
    for (int i = 0; i < numSamp; ++i)
        outPeak = std::max(outPeak, std::abs(out[i]));
    outputLevelDb.store(juce::Decibels::gainToDecibels(outPeak, -100.0f));

    for (int ch = 0; ch < buffer->getNumChannels(); ++ch)
        buffer->copyFrom(ch, start, mono, 0, 0, numSamp);
}

void MainComponent::releaseResources()
{
    effectChain.reset();
}

// ---- UI ----

void MainComponent::applyPreset(const PresetManager::PresetData& p)
{
    // Gate
    adv.gateThreshold.setValue(p.gateThreshDb,  juce::sendNotification);
    adv.gateAttack   .setValue(p.gateAttackMs,  juce::sendNotification);
    adv.gateRelease  .setValue(p.gateReleaseMs, juce::sendNotification);
    // HPF
    adv.hpfCutoff.setValue(p.hpfCutoffHz, juce::sendNotification);
    // Pitch
    adv.pitchSlider.setValue(p.pitchSemitones, juce::sendNotification);
    // Formant
    adv.formantSlider.setValue(p.formantSemitones, juce::sendNotification);
    // LFO Filter
    adv.lfoFilterTypeBox.setSelectedId(p.lfoType,         juce::sendNotification);
    adv.lfoFilterCutoff   .setValue(p.lfoCutoffHz,         juce::sendNotification);
    adv.lfoFilterResonance.setValue(p.lfoResonance,        juce::sendNotification);
    adv.lfoFilterRate     .setValue(p.lfoRateHz,           juce::sendNotification);
    adv.lfoFilterDepth    .setValue(p.lfoDepthOct,         juce::sendNotification);
    adv.lfoFilterDryWet   .setValue(p.lfoDryWet,           juce::sendNotification);
    // Phaser
    adv.phaserRate    .setValue(p.phaserRate,     juce::sendNotification);
    adv.phaserDepth   .setValue(p.phaserDepth,    juce::sendNotification);
    adv.phaserFeedback.setValue(p.phaserFeedback, juce::sendNotification);
    adv.phaserDryWet  .setValue(p.phaserDryWet,   juce::sendNotification);
    // BPF
    adv.bpfCentre.setValue(p.bpfCentreHz, juce::sendNotification);
    adv.bpfQ     .setValue(p.bpfQ,        juce::sendNotification);
    adv.bpfDryWet.setValue(p.bpfDryWet,   juce::sendNotification);
    // Output gain
    adv.gainSlider.setValue(p.outputGainDb, juce::sendNotification);

    syncBasicFromAdv();
}

void MainComponent::syncBasicFromAdv()
{
    basic.syncKnobs((float)adv.gateThreshold.getValue(),
                    (float)adv.pitchSlider.getValue(),
                    (float)adv.formantSlider.getValue(),
                    (float)adv.lfoFilterDryWet.getValue(),
                    (float)adv.phaserDryWet.getValue(),
                    (float)adv.bpfDryWet.getValue(),
                    (float)adv.gainSlider.getValue());
}

void MainComponent::openAudioSettings()
{
    auto* selector = new juce::AudioDeviceSelectorComponent(
        deviceManager, 1, 1, 2, 2, false, false, false, false);
    selector->setSize(500, 400);

    juce::DialogWindow::LaunchOptions opts;
    opts.content.setOwned(selector);
    opts.dialogTitle                  = "Audio Settings";
    opts.dialogBackgroundColour       = getLookAndFeel().findColour(
        juce::ResizableWindow::backgroundColourId);
    opts.escapeKeyTriggersCloseButton = true;
    opts.useNativeTitleBar            = true;
    opts.resizable                    = false;
    opts.launchAsync();
}

// ---- Preset export ----

std::unique_ptr<juce::XmlElement> MainComponent::buildPresetXml() const
{
    auto root = std::make_unique<juce::XmlElement>("MolluskVoiceChangerPreset");
    root->setAttribute("version", 1);

    auto add = [&](const juce::String& tag,
                   const juce::ToggleButton& bypass,
                   std::initializer_list<std::pair<juce::String, double>> attrs)
    {
        auto* el = root->createNewChildElement(tag);
        el->setAttribute("enabled", bypass.getToggleState() ? 0 : 1);
        for (auto& [k, v] : attrs)
            el->setAttribute(k, v);
    };

    add("Gate", adv.gateBypass, {
        {"threshold", adv.gateThreshold.getValue()},
        {"attack",    adv.gateAttack.getValue()},
        {"release",   adv.gateRelease.getValue()}
    });
    add("HPF",         adv.hpfBypass,     {{"cutoff",    adv.hpfCutoff.getValue()}});
    add("PitchShift",  adv.pitchBypass,   {{"semitones", adv.pitchSlider.getValue()}});
    add("FormantShift",adv.formantBypass, {{"semitones", adv.formantSlider.getValue()}});

    auto* lfo = root->createNewChildElement("LFOFilter");
    lfo->setAttribute("enabled",   adv.lfoFilterBypass.getToggleState() ? 0 : 1);
    lfo->setAttribute("type",      adv.lfoFilterTypeBox.getSelectedId());
    lfo->setAttribute("cutoff",    adv.lfoFilterCutoff.getValue());
    lfo->setAttribute("resonance", adv.lfoFilterResonance.getValue());
    lfo->setAttribute("lfoRate",   adv.lfoFilterRate.getValue());
    lfo->setAttribute("randomRate",adv.lfoFilterRandomRate.getToggleState() ? 0 : 1);
    lfo->setAttribute("lfoDepth",  adv.lfoFilterDepth.getValue());
    lfo->setAttribute("dryWet",    adv.lfoFilterDryWet.getValue());

    add("Phaser", adv.phaserBypass, {
        {"rate",     adv.phaserRate.getValue()},
        {"depth",    adv.phaserDepth.getValue()},
        {"feedback", adv.phaserFeedback.getValue()},
        {"dryWet",   adv.phaserDryWet.getValue()}
    });
    add("BPF", adv.bpfBypass, {
        {"centre", adv.bpfCentre.getValue()},
        {"q",      adv.bpfQ.getValue()},
        {"dryWet", adv.bpfDryWet.getValue()}
    });
    add("OutputGain", adv.gainBypass, {{"gainDb", adv.gainSlider.getValue()}});

    return root;
}


void MainComponent::applyPresetXml(const juce::XmlElement& root)
{
    auto setBypass = [](juce::ToggleButton& btn, const juce::XmlElement* el)
    {
        if (el) btn.setToggleState(el->getIntAttribute("enabled", 1) == 0,
                                   juce::sendNotification);
    };

    auto setRandomRate = [](juce::ToggleButton& btn, const juce::XmlElement* el)
    {
        if (el) btn.setToggleState(el->getIntAttribute("randomRate", 1) == 0,
                                   juce::sendNotification);
    };

    auto setSlider = [](juce::Slider& sl, const juce::XmlElement* el,
                        const juce::String& attr, double def)
    {
        if (el) sl.setValue(el->getDoubleAttribute(attr, def), juce::sendNotification);
    };

    if (auto* el = root.getChildByName("Gate"))
    {
        setBypass(adv.gateBypass, el);
        setSlider(adv.gateThreshold, el, "threshold", -40.0);
        setSlider(adv.gateAttack,    el, "attack",      5.0);
        setSlider(adv.gateRelease,   el, "release",   100.0);
    }
    if (auto* el = root.getChildByName("HPF"))
    {
        setBypass(adv.hpfBypass, el);
        setSlider(adv.hpfCutoff, el, "cutoff", 80.0);
    }
    if (auto* el = root.getChildByName("PitchShift"))
    {
        setBypass(adv.pitchBypass, el);
        setSlider(adv.pitchSlider, el, "semitones", 4.0);
    }
    if (auto* el = root.getChildByName("FormantShift"))
    {
        setBypass(adv.formantBypass, el);
        setSlider(adv.formantSlider, el, "semitones", 0.0);
    }
    if (auto* el = root.getChildByName("LFOFilter"))
    {
        setBypass(adv.lfoFilterBypass, el);
        adv.lfoFilterTypeBox.setSelectedId(el->getIntAttribute("type", 1),
                                       juce::sendNotification);
        setSlider(adv.lfoFilterCutoff,    el, "cutoff",    1000.0);
        setSlider(adv.lfoFilterResonance, el, "resonance", 0.7);
        setSlider(adv.lfoFilterRate,      el, "lfoRate",   2.0);
        setRandomRate(adv.lfoFilterRandomRate,el);
        setSlider(adv.lfoFilterDepth,     el, "lfoDepth",  1.0);
        setSlider(adv.lfoFilterDryWet,    el, "dryWet",    1.0);
    }
    if (auto* el = root.getChildByName("Phaser"))
    {
        setBypass(adv.phaserBypass, el);
        setSlider(adv.phaserRate,     el, "rate",     3.0);
        setSlider(adv.phaserDepth,    el, "depth",    0.7);
        setSlider(adv.phaserFeedback, el, "feedback", 0.6);
        setSlider(adv.phaserDryWet,   el, "dryWet",   1.0);
    }
    if (auto* el = root.getChildByName("BPF"))
    {
        setBypass(adv.bpfBypass, el);
        setSlider(adv.bpfCentre,  el, "centre", 2000.0);
        setSlider(adv.bpfQ,       el, "q",      0.7);
        setSlider(adv.bpfDryWet,  el, "dryWet", 1.0);
    }
    if (auto* el = root.getChildByName("OutputGain"))
    {
        setBypass(adv.gainBypass, el);
        setSlider(adv.gainSlider, el, "gainDb", 0.0);
    }
    syncBasicFromAdv();
    clearActivePreset();   // XML import doesn't match any preset button
}

void MainComponent::setActivePreset(int index)
{
    for (int i = 0; i < 4; ++i)
    {
        juce::Colour c;
        if (i == index)
            c = (i < 2) ? juce::Colour(MolluskLookAndFeel::colSquidOn)
                        : juce::Colour(MolluskLookAndFeel::colOctopusOn);
        else
            c = juce::Colour(MolluskLookAndFeel::colPanel);
        presetButtons[i].setColour(juce::TextButton::buttonColourId, c);
    }
}

void MainComponent::clearActivePreset()
{
    for (int i = 0; i < 4; ++i)
        presetButtons[i].setColour(juce::TextButton::buttonColourId,
                                   juce::Colour(MolluskLookAndFeel::colPanel));
}

void MainComponent::saveLastSession() const
{
    auto xml = buildPresetXml();
    if (!xml) return;

    // オーディオデバイス設定を子要素として追加
    if (auto audioState = deviceManager.createStateXml())
    {
        auto* wrapper = xml->createNewChildElement("AudioSettings");
        wrapper->addChildElement(audioState.release());
    }

    auto dir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                   .getChildFile("MolluskVoiceChanger");
    dir.createDirectory();
    xml->writeTo(dir.getChildFile("last_session.xml"));
}

void MainComponent::loadLastSession()
{
    auto file = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                    .getChildFile("MolluskVoiceChanger")
                    .getChildFile("last_session.xml");
    if (!file.existsAsFile()) return;

    auto xml = juce::parseXML(file);
    if (!xml || xml->getTagName() != "MolluskVoiceChangerPreset") return;

    // オーディオデバイス設定を復元
    if (auto* audioEl = xml->getChildByName("AudioSettings"))
        if (auto* state = audioEl->getFirstChildElement())
            deviceManager.initialise(1, 2, state, true);

    // エフェクトパラメータを復元
    applyPresetXml(*xml);
}

void MainComponent::setBasicMode(bool isBasic)
{
    basicMode = isBasic;
    modeButton.setButtonText(isBasic ? "Advanced" : "Basic");

    // When returning to Basic, sync knobs from current Advanced slider state
    if (isBasic)
        syncBasicFromAdv();

    // In Basic mode, LFO random rate is always ON
    if (isBasic)
    {
        effectChain.lfoFilter.setRandomRateEnabled(true);
        adv.lfoFilterRandomRate.setToggleState(true, juce::dontSendNotification);
    }

    basic.setVisible(isBasic);
    advViewport.setVisible(!isBasic);

    setSize(580, isBasic ? 515 : 850);
    resized();
}

void MainComponent::timerCallback()
{
    basic.levelMeter.setLevelDb(inputLevelDb.load());
    basic.levelMeter.setGateOpen(effectChain.gate.getGateGain() > 0.5f);
    basic.levelMeter.setThresholdDb(effectChain.gate.getThresholdDb());
    basic.levelMeter.repaint();

    basic.outputLevelMeter.setLevelDb(outputLevelDb.load());
    basic.outputLevelMeter.repaint();
}

void MainComponent::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient (
        juce::Colour(0xFF0f020b),                // 開始色
        0.0f, 0.0f,                         // 始点座標 (左上)
        juce::Colour(0xff0b0e04),                 // 終了色
        (float)getWidth(), (float)getHeight(), // 終点座標 (右下)
        false                               // リニアグラデーション（false）
    );
    gradient.addColour(0.7, juce::Colour(0xff000003));

    // 2. 塗りつぶしの種類をグラデーションに設定
    g.setGradientFill (gradient);

    // 3. 背景全体を塗りつぶす
    g.fillAll();
}

// Common header: top bar + preset row
static juce::Rectangle<int> layoutCommonHeader(
    juce::Rectangle<int> area,
    juce::TextButton& audioSettings,
    juce::TextButton& exportBtn,
    juce::TextButton& importBtn,
    juce::ToggleButton& bypassAll,
    juce::TextButton& modeBtn,
    juce::TextButton presets[4])
{
    {
        auto topBar = area.removeFromTop(48);
        audioSettings.setBounds(topBar.removeFromLeft(140));
        topBar.removeFromLeft(6);
        exportBtn.setBounds(topBar.removeFromLeft(110));
        topBar.removeFromLeft(4);
        importBtn.setBounds(topBar.removeFromLeft(110));
        topBar.removeFromLeft(6);
        bypassAll.setBounds(topBar.removeFromLeft(90));
        topBar.removeFromLeft(6);
        modeBtn.setBounds(topBar.removeFromLeft(90));
    }
    area.removeFromTop(4);
    {
        auto row = area.removeFromTop(54);
        const int btnW = (row.getWidth() - 12) / 4;
        for (int i = 0; i < 4; ++i)
        {
            presets[i].setBounds(row.removeFromLeft(btnW));
            if (i < 3) row.removeFromLeft(4);
        }
    }
    area.removeFromTop(6);
    return area;
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(10);
    area = layoutCommonHeader(area,
        audioSettingsButton, exportButton, importButton,
        bypassAllButton, modeButton, presetButtons);

    if (basicMode) layoutBasic(area);
    else           layoutAdvanced(area);
}

void MainComponent::layoutBasic(juce::Rectangle<int> area)
{
    basic.setBounds(area);
}

void MainComponent::layoutAdvanced(juce::Rectangle<int> area)
{
    advViewport.setBounds(area);
    const int contentW = area.getWidth() - advViewport.getScrollBarThickness();
    adv.setBounds(0, 0, contentW, AdvancedPanel::preferredHeight);
}
