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
#include <atomic>
#include <random>

// State Variable Filter with per-sample LFO cutoff modulation.
// When randomRateEnabled, the LFO rate is randomised to a new value
// every 0.5 s, producing a sample-and-hold rate modulation effect.
class LFOFilter
{
public:
    enum class FilterType { Lowpass = 0, Bandpass = 1, Highpass = 2 };

    LFOFilter();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void setEnabled(bool e)              { enabled.store(e); }
    void setFilterType(FilterType t)     { filterType.store((int)t); }
    void setCutoffHz(float hz)           { cutoffHz.store(hz); }
    void setResonance(float r)           { resonance.store(r); }
    void setLFORate(float hz)            { lfoRate.store(hz); }
    void setLFODepth(float octaves)      { lfoDepth.store(octaves); }
    void setDryWet(float mix)            { dryWet.store(mix); }
    void setRandomRateEnabled(bool e)    { randomRateEnabled.store(e); }

    bool       getEnabled()          const { return enabled.load(); }
    FilterType getFilterType()       const { return (FilterType)filterType.load(); }
    float      getCutoffHz()         const { return cutoffHz.load(); }
    float      getResonance()        const { return resonance.load(); }
    float      getLFORate()          const { return lfoRate.load(); }
    float      getLFODepth()         const { return lfoDepth.load(); }
    float      getDryWet()           const { return dryWet.load(); }
    bool       getRandomRateEnabled() const { return randomRateEnabled.load(); }

private:
    juce::dsp::StateVariableTPTFilter<float> filter;

    double sampleRate = 44100.0;
    float  lfoPhase   = 0.0f;

    // Random rate state (audio-thread only — no atomics needed)
    std::mt19937                          rng;
    std::uniform_real_distribution<float> rateDist { 0.1f, 10.0f };
    float currentRandomRate = 2.0f;
    int   randomRateCounter = 0;
    static constexpr double randomPeriodSeconds = 0.5;

    std::atomic<int>   filterType       { 0 };
    std::atomic<float> cutoffHz         { 1000.0f };
    std::atomic<float> resonance        { 0.7f };
    std::atomic<float> lfoRate          { 2.0f };
    std::atomic<float> lfoDepth         { 1.0f };
    std::atomic<float> dryWet           { 1.0f };
    std::atomic<bool>  enabled          { true };
    std::atomic<bool>  randomRateEnabled{ false };

    JUCE_DECLARE_NON_COPYABLE(LFOFilter)
};
