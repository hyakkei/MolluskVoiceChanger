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

#include "LFOFilter.h"
#include <cmath>

LFOFilter::LFOFilter()
    : rng(std::random_device{}())
{
}

void LFOFilter::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    reset();

    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    filter.setCutoffFrequency(cutoffHz.load());
    filter.setResonance(resonance.load());
    filter.prepare(spec);
}

void LFOFilter::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled.load()) return;

    const int   numSamples  = buffer.getNumSamples();
    const int   numChannels = buffer.getNumChannels();
    const float base        = cutoffHz.load();
    const float depth       = lfoDepth.load();
    const float res         = resonance.load();
    const float mix         = dryWet.load();

    // ---- Determine active LFO rate ----
    float activeRate = lfoRate.load();

    if (randomRateEnabled.load())
    {
        // Advance counter and randomise rate every 0.5 s
        const int period = (int)(sampleRate * randomPeriodSeconds);
        randomRateCounter += numSamples;
        while (randomRateCounter >= period)
        {
            randomRateCounter -= period;
            currentRandomRate = rateDist(rng);
        }
        activeRate = currentRandomRate;
    }

    const float phaseInc = activeRate / (float)sampleRate;

    // ---- Filter type (update only when changed) ----
    static int lastType = -1;
    const int  curType  = filterType.load();
    if (curType != lastType)
    {
        lastType = curType;
        using T = juce::dsp::StateVariableTPTFilterType;
        filter.setType(curType == 1 ? T::bandpass :
                       curType == 2 ? T::highpass  : T::lowpass);
    }

    filter.setResonance(res);

    // ---- Per-sample LFO: cutoff = base * 2^(depth * sin(phase)) ----
    for (int i = 0; i < numSamples; ++i)
    {
        const float lfoVal    = std::sin(juce::MathConstants<float>::twoPi * lfoPhase);
        const float modCutoff = juce::jlimit(
            20.0f, 20000.0f,
            base * std::pow(2.0f, depth * lfoVal));

        filter.setCutoffFrequency(modCutoff);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* data     = buffer.getWritePointer(ch);
            const float dry = data[i];
            const float wet = filter.processSample(ch, dry);
            data[i] = dry * (1.0f - mix) + wet * mix;
        }

        lfoPhase += phaseInc;
        if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
    }
}

void LFOFilter::reset()
{
    filter.reset();
    lfoPhase          = 0.0f;
    randomRateCounter = 0;
    currentRandomRate = lfoRate.load();
}
