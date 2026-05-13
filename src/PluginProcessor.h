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

class MolluskVoiceChangerProcessor  : public juce::AudioProcessor
{
public:
    MolluskVoiceChangerProcessor();
    ~MolluskVoiceChangerProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Mollusk Voice Changer"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override                                      { return 1; }
    int getCurrentProgram() override                                   { return 0; }
    void setCurrentProgram(int) override                               {}
    const juce::String getProgramName(int) override                    { return "Default"; }
    void changeProgramName(int, const juce::String&) override          {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // ---- Public state (accessed by editor) ----
    juce::AudioProcessorValueTreeState apvts;
    EffectChain   effectChain;
    PresetManager presetManager;

    std::atomic<float> inputLevelDb { -100.0f };

private:
    // Cached raw parameter pointers (read in processBlock, no string lookup)
    std::atomic<float>* pBypassAll       { nullptr };
    std::atomic<float>* pGateBypass      { nullptr };
    std::atomic<float>* pGateThreshold   { nullptr };
    std::atomic<float>* pGateAttack      { nullptr };
    std::atomic<float>* pGateRelease     { nullptr };
    std::atomic<float>* pHpfBypass       { nullptr };
    std::atomic<float>* pHpfCutoff       { nullptr };
    std::atomic<float>* pPitchBypass     { nullptr };
    std::atomic<float>* pPitchSemitones  { nullptr };
    std::atomic<float>* pFormantBypass   { nullptr };
    std::atomic<float>* pFormantSemitones{ nullptr };
    std::atomic<float>* pLfoBypass       { nullptr };
    std::atomic<float>* pLfoType         { nullptr };
    std::atomic<float>* pLfoCutoff       { nullptr };
    std::atomic<float>* pLfoResonance    { nullptr };
    std::atomic<float>* pLfoRate         { nullptr };
    std::atomic<float>* pLfoDepth        { nullptr };
    std::atomic<float>* pLfoDryWet       { nullptr };
    std::atomic<float>* pLfoRandom       { nullptr };
    std::atomic<float>* pPhaserBypass    { nullptr };
    std::atomic<float>* pPhaserRate      { nullptr };
    std::atomic<float>* pPhaserDepth     { nullptr };
    std::atomic<float>* pPhaserFeedback  { nullptr };
    std::atomic<float>* pPhaserDryWet    { nullptr };
    std::atomic<float>* pBpfBypass       { nullptr };
    std::atomic<float>* pBpfCentre       { nullptr };
    std::atomic<float>* pBpfQ            { nullptr };
    std::atomic<float>* pBpfDryWet       { nullptr };
    std::atomic<float>* pGainBypass      { nullptr };
    std::atomic<float>* pGainDb          { nullptr };

    void cacheParameterPointers();
    void updateEffectChain();

    // Pre-allocated mono buffer (avoids heap allocation on the audio thread)
    juce::AudioBuffer<float> monoBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MolluskVoiceChangerProcessor)
};
