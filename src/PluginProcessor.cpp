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

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/LFOFilter.h"

// ---- Parameter layout ----

juce::AudioProcessorValueTreeState::ParameterLayout
MolluskVoiceChangerProcessor::createParameterLayout()
{
    using Param  = juce::AudioParameterFloat;
    using BParam = juce::AudioParameterBool;
    using CParam = juce::AudioParameterChoice;
    using NR     = juce::NormalisableRange<float>;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    // Global
    p.push_back(std::make_unique<BParam>("bypass_all",        "Bypass All",            false));

    // Noise Gate
    p.push_back(std::make_unique<BParam>("gate_bypass",       "Gate Bypass",           false));
    p.push_back(std::make_unique<Param>("gate_threshold",     "Gate Threshold",        NR(-80.f,  0.f,  1.f), -40.f));
    p.push_back(std::make_unique<Param>("gate_attack",        "Gate Attack",           NR(  1.f,100.f,  1.f),   5.f));
    p.push_back(std::make_unique<Param>("gate_release",       "Gate Release",          NR( 10.f,500.f,  5.f), 100.f));

    // High Pass Filter
    p.push_back(std::make_unique<BParam>("hpf_bypass",        "HPF Bypass",            false));
    p.push_back(std::make_unique<Param>("hpf_cutoff",         "HPF Cutoff",            NR( 20.f,500.f,  1.f),  80.f));

    // Pitch Shifter
    p.push_back(std::make_unique<BParam>("pitch_bypass",      "Pitch Bypass",          false));
    p.push_back(std::make_unique<Param>("pitch_semitones",    "Pitch",                 NR(  0.f, 12.f,0.5f),   0.f));

    // Formant Shifter
    p.push_back(std::make_unique<BParam>("formant_bypass",    "Formant Bypass",        false));
    p.push_back(std::make_unique<Param>("formant_semitones",  "Formant",               NR(-12.f, 12.f,0.5f),   0.f));

    // LFO Filter
    p.push_back(std::make_unique<BParam>("lfo_bypass",        "LFO Bypass",            false));
    p.push_back(std::make_unique<CParam>("lfo_type",          "LFO Type",
                                          juce::StringArray{"Lowpass","Bandpass","Highpass"}, 0));
    p.push_back(std::make_unique<Param>("lfo_cutoff",         "LFO Cutoff",            NR( 20.f,10000.f, 1.f),1000.f));
    p.push_back(std::make_unique<Param>("lfo_resonance",      "LFO Resonance",         NR(0.1f,  4.f,0.01f),  0.7f));
    p.push_back(std::make_unique<Param>("lfo_rate",           "LFO Rate",              NR(0.1f, 20.f, 0.1f),  2.0f));
    p.push_back(std::make_unique<Param>("lfo_depth",          "LFO Depth",             NR(  0.f,  4.f, 0.1f),  1.f));
    p.push_back(std::make_unique<Param>("lfo_drywet",         "LFO Dry/Wet",           NR(  0.f,  1.f,0.01f),  1.f));
    p.push_back(std::make_unique<BParam>("lfo_random",        "LFO Random Rate",       false));

    // Phaser
    p.push_back(std::make_unique<BParam>("phaser_bypass",     "Phaser Bypass",         false));
    p.push_back(std::make_unique<Param>("phaser_rate",        "Phaser Rate",           NR(0.1f, 10.f, 0.1f),  3.f));
    p.push_back(std::make_unique<Param>("phaser_depth",       "Phaser Depth",          NR(  0.f,  1.f,0.01f),0.7f));
    p.push_back(std::make_unique<Param>("phaser_feedback",    "Phaser Feedback",       NR( -1.f,  1.f,0.01f),0.6f));
    p.push_back(std::make_unique<Param>("phaser_drywet",      "Phaser Dry/Wet",        NR(  0.f,  1.f,0.01f),  1.f));

    // Band Pass Filter
    p.push_back(std::make_unique<BParam>("bpf_bypass",        "BPF Bypass",            false));
    p.push_back(std::make_unique<Param>("bpf_centre",         "BPF Centre",            NR(200.f,8000.f,10.f),2000.f));
    p.push_back(std::make_unique<Param>("bpf_q",              "BPF Q",                 NR(0.1f,  8.f, 0.1f),  0.7f));
    p.push_back(std::make_unique<Param>("bpf_drywet",         "BPF Dry/Wet",           NR(  0.f,  1.f,0.01f),  1.f));

    // Output Gain
    p.push_back(std::make_unique<BParam>("gain_bypass",       "Gain Bypass",           false));
    p.push_back(std::make_unique<Param>("gain_db",            "Gain",                  NR(-60.f, 12.f, 0.1f),  0.f));

    return { p.begin(), p.end() };
}

// ---- Constructor ----

MolluskVoiceChangerProcessor::MolluskVoiceChangerProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::mono(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    presetManager.onApplyPreset = [this](const PresetManager::PresetData& p) {
        // Apply preset to APVTS parameters
        auto set = [&](const juce::String& id, float val) {
            if (auto* param = apvts.getParameter(id))
                param->setValueNotifyingHost(param->convertTo0to1(val));
        };
        set("gate_threshold",   p.gateThreshDb);
        set("gate_attack",      p.gateAttackMs);
        set("gate_release",     p.gateReleaseMs);
        set("hpf_cutoff",       p.hpfCutoffHz);
        set("pitch_semitones",  p.pitchSemitones);
        set("formant_semitones",p.formantSemitones);
        set("lfo_cutoff",       p.lfoCutoffHz);
        set("lfo_resonance",    p.lfoResonance);
        set("lfo_rate",         p.lfoRateHz);
        set("lfo_depth",        p.lfoDepthOct);
        set("lfo_drywet",       p.lfoDryWet);
        set("phaser_rate",      p.phaserRate);
        set("phaser_depth",     p.phaserDepth);
        set("phaser_feedback",  p.phaserFeedback);
        set("phaser_drywet",    p.phaserDryWet);
        set("bpf_centre",       p.bpfCentreHz);
        set("bpf_q",            p.bpfQ);
        set("bpf_drywet",       p.bpfDryWet);
        set("gain_db",          p.outputGainDb);
        // LFO type (choice: 0-based)
        if (auto* param = apvts.getParameter("lfo_type"))
            param->setValueNotifyingHost(param->convertTo0to1((float)(p.lfoType - 1)));
    };

    cacheParameterPointers();
}

void MolluskVoiceChangerProcessor::cacheParameterPointers()
{
    pBypassAll        = apvts.getRawParameterValue("bypass_all");
    pGateBypass       = apvts.getRawParameterValue("gate_bypass");
    pGateThreshold    = apvts.getRawParameterValue("gate_threshold");
    pGateAttack       = apvts.getRawParameterValue("gate_attack");
    pGateRelease      = apvts.getRawParameterValue("gate_release");
    pHpfBypass        = apvts.getRawParameterValue("hpf_bypass");
    pHpfCutoff        = apvts.getRawParameterValue("hpf_cutoff");
    pPitchBypass      = apvts.getRawParameterValue("pitch_bypass");
    pPitchSemitones   = apvts.getRawParameterValue("pitch_semitones");
    pFormantBypass    = apvts.getRawParameterValue("formant_bypass");
    pFormantSemitones = apvts.getRawParameterValue("formant_semitones");
    pLfoBypass        = apvts.getRawParameterValue("lfo_bypass");
    pLfoType          = apvts.getRawParameterValue("lfo_type");
    pLfoCutoff        = apvts.getRawParameterValue("lfo_cutoff");
    pLfoResonance     = apvts.getRawParameterValue("lfo_resonance");
    pLfoRate          = apvts.getRawParameterValue("lfo_rate");
    pLfoDepth         = apvts.getRawParameterValue("lfo_depth");
    pLfoDryWet        = apvts.getRawParameterValue("lfo_drywet");
    pLfoRandom        = apvts.getRawParameterValue("lfo_random");
    pPhaserBypass     = apvts.getRawParameterValue("phaser_bypass");
    pPhaserRate       = apvts.getRawParameterValue("phaser_rate");
    pPhaserDepth      = apvts.getRawParameterValue("phaser_depth");
    pPhaserFeedback   = apvts.getRawParameterValue("phaser_feedback");
    pPhaserDryWet     = apvts.getRawParameterValue("phaser_drywet");
    pBpfBypass        = apvts.getRawParameterValue("bpf_bypass");
    pBpfCentre        = apvts.getRawParameterValue("bpf_centre");
    pBpfQ             = apvts.getRawParameterValue("bpf_q");
    pBpfDryWet        = apvts.getRawParameterValue("bpf_drywet");
    pGainBypass       = apvts.getRawParameterValue("gain_bypass");
    pGainDb           = apvts.getRawParameterValue("gain_db");
}

// ---- Audio ----

void MolluskVoiceChangerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = (juce::uint32)samplesPerBlock;
    spec.numChannels      = 1;
    effectChain.prepare(spec);

    // Pre-allocate mono work buffer (prevents heap allocation in processBlock)
    monoBuffer.setSize(1, samplesPerBlock, false, true, false);
}

void MolluskVoiceChangerProcessor::releaseResources()
{
    effectChain.reset();
}

bool MolluskVoiceChangerProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono();
}

void MolluskVoiceChangerProcessor::updateEffectChain()
{
    effectChain.gate.setEnabled          (!(*pGateBypass    > 0.5f));
    effectChain.gate.setThresholdDb      (*pGateThreshold);
    effectChain.gate.setAttackMs         (*pGateAttack);
    effectChain.gate.setReleaseMs        (*pGateRelease);

    effectChain.hpf.setEnabled           (!(*pHpfBypass     > 0.5f));
    effectChain.hpf.setCutoffHz          (*pHpfCutoff);

    effectChain.pitchShifter.setEnabled  (!(*pPitchBypass   > 0.5f));
    effectChain.pitchShifter.setSemitones(*pPitchSemitones);

    effectChain.formantShifter.setEnabled  (!(*pFormantBypass   > 0.5f));
    effectChain.formantShifter.setSemitones(*pFormantSemitones);

    effectChain.lfoFilter.setEnabled     (!(*pLfoBypass     > 0.5f));
    effectChain.lfoFilter.setFilterType  ((LFOFilter::FilterType)(int)(*pLfoType));
    effectChain.lfoFilter.setCutoffHz    (*pLfoCutoff);
    effectChain.lfoFilter.setResonance   (*pLfoResonance);
    effectChain.lfoFilter.setLFORate     (*pLfoRate);
    effectChain.lfoFilter.setLFODepth    (*pLfoDepth);
    effectChain.lfoFilter.setDryWet      (*pLfoDryWet);
    effectChain.lfoFilter.setRandomRateEnabled(*pLfoRandom > 0.5f);

    effectChain.phaser.setEnabled        (!(*pPhaserBypass  > 0.5f));
    effectChain.phaser.setRate           (*pPhaserRate);
    effectChain.phaser.setDepth          (*pPhaserDepth);
    effectChain.phaser.setFeedback       (*pPhaserFeedback);
    effectChain.phaser.setMix            (*pPhaserDryWet);

    effectChain.bpf.setEnabled           (!(*pBpfBypass     > 0.5f));
    effectChain.bpf.setCentreHz          (*pBpfCentre);
    effectChain.bpf.setQ                 (*pBpfQ);
    effectChain.bpf.setDryWet            (*pBpfDryWet);

    effectChain.outputGain.setEnabled    (!(*pGainBypass    > 0.5f));
    effectChain.outputGain.setGainDb     (*pGainDb);
}

void MolluskVoiceChangerProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                 juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Update DSP parameters from APVTS (all atomic reads, RT-safe)
    updateEffectChain();

    // Global bypass: completely unmodified passthrough
    if (*pBypassAll > 0.5f)
        return;

    // Copy mono input into pre-allocated work buffer (no heap allocation on audio thread)
    monoBuffer.setSize(1, numSamples, false, false, true); // avoidRealloc = true
    monoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);

    // Measure input level for VU meter
    float peak = 0.0f;
    const float* src = monoBuffer.getReadPointer(0);
    for (int i = 0; i < numSamples; ++i)
        peak = std::max(peak, std::abs(src[i]));
    inputLevelDb.store(juce::Decibels::gainToDecibels(peak, -100.0f));

    effectChain.process(monoBuffer);

    // Copy mono result to all output channels
    for (int ch = 0; ch < numChannels; ++ch)
        buffer.copyFrom(ch, 0, monoBuffer, 0, 0, numSamples);
}

// ---- State ----

void MolluskVoiceChangerProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    auto xml   = state.createXml();
    copyXmlToBinary(*xml, destData);
}

void MolluskVoiceChangerProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

// ---- Editor ----

juce::AudioProcessorEditor* MolluskVoiceChangerProcessor::createEditor()
{
    return new MolluskVoiceChangerEditor(*this);
}

// ---- Plugin entry point ----

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MolluskVoiceChangerProcessor();
}
