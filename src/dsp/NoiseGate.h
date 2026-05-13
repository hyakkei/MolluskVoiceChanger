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

// Noise gate placed at the head of the DSP chain.
// Uses a peak-hold / exponential-release envelope follower for level detection,
// then linearly ramps the gate gain open/closed to avoid clicks.
class NoiseGate
{
public:
    NoiseGate() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void setEnabled(bool e)           { enabled.store(e); }
    void setThresholdDb(float db)     { thresholdDb.store(db); }
    void setAttackMs(float ms)        { attackMs.store(ms); updateCoeffs(); }
    void setReleaseMs(float ms)       { releaseMs.store(ms); updateCoeffs(); }

    bool  getEnabled()      const     { return enabled.load(); }
    float getThresholdDb()  const     { return thresholdDb.load(); }
    float getAttackMs()     const     { return attackMs.load(); }
    float getReleaseMs()    const     { return releaseMs.load(); }
    // Read current gate gain [0,1] from the UI thread for LED display
    float getGateGain()     const     { return gateGainAtomic.load(); }

private:
    void updateCoeffs();

    double sampleRate = 44100.0;

    // Envelope follower state (audio thread only)
    float envelope  = 0.0f;
    float gateGain  = 0.0f;   // smoothed gate gain [0, 1]

    std::atomic<float> gateGainAtomic { 0.0f };  // mirrored for UI thread reads

    // Coefficients computed from attack/release times
    float releaseCoeff  = 0.0f;  // exponential release for envelope
    float attackInc     = 0.0f;  // linear open increment per sample
    float releaseInc    = 0.0f;  // linear close increment per sample

    std::atomic<float> thresholdDb { -40.0f };
    std::atomic<float> attackMs    {   5.0f };
    std::atomic<float> releaseMs   { 100.0f };
    std::atomic<bool>  enabled     { true };

    JUCE_DECLARE_NON_COPYABLE(NoiseGate)
};
