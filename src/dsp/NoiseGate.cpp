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

#include "NoiseGate.h"
#include <cmath>

void NoiseGate::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;
    updateCoeffs();
    reset();
}

void NoiseGate::reset()
{
    envelope = 0.0f;
    gateGain = 0.0f;
}

void NoiseGate::updateCoeffs()
{
    if (sampleRate <= 0.0) return;

    // Envelope release: exponential decay τ = releaseMs
    const double relSamples = releaseMs.load() * sampleRate / 1000.0;
    releaseCoeff = (float)std::exp(-1.0 / relSamples);

    // Gate open/close: linear ramp over attack/release times
    const double attSamples = attackMs.load()  * sampleRate / 1000.0;
    attackInc   = (float)(1.0 / attSamples);
    releaseInc  = (float)(1.0 / relSamples);
}

void NoiseGate::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled.load()) return;

    const int   numSamples  = buffer.getNumSamples();
    const int   numChannels = buffer.getNumChannels();
    const float threshold   = thresholdDb.load();

    for (int i = 0; i < numSamples; ++i)
    {
        // ---- Envelope follower: peak-hold, exponential release ----
        float peak = 0.0f;
        for (int ch = 0; ch < numChannels; ++ch)
            peak = std::max(peak, std::abs(buffer.getReadPointer(ch)[i]));

        if (peak > envelope)
            envelope = peak;                      // instant peak hold
        else
            envelope *= releaseCoeff;             // exponential decay

        // ---- Gate decision ----
        const float envDb  = juce::Decibels::gainToDecibels(envelope, -100.0f);
        const float target = (envDb >= threshold) ? 1.0f : 0.0f;

        // ---- Smooth gate transitions (linear ramp) ----
        if (target > gateGain)
            gateGain = std::min(1.0f, gateGain + attackInc);
        else
            gateGain = std::max(0.0f, gateGain - releaseInc);

        // ---- Apply gain ----
        for (int ch = 0; ch < numChannels; ++ch)
            buffer.getWritePointer(ch)[i] *= gateGain;
    }

    // Mirror final gate gain for UI thread (once per block is sufficient)
    gateGainAtomic.store(gateGain);
}
