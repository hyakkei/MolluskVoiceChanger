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
#include "NoiseGate.h"
#include "HighPassFilter.h"
#include "PitchShifter.h"
#include "FormantShifter.h"
#include "LFOFilter.h"
#include "PhaserProcessor.h"
#include "BandPassFilter.h"
#include "OutputGain.h"

// Pipeline: Gate → HPF → Pitch → Formant → LFOFilter → Phaser → BPF → Gain
class EffectChain
{
public:
    EffectChain() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    NoiseGate       gate;
    HighPassFilter  hpf;
    PitchShifter    pitchShifter;
    FormantShifter  formantShifter;
    LFOFilter       lfoFilter;
    PhaserProcessor phaser;
    BandPassFilter  bpf;
    OutputGain      outputGain;

private:
    JUCE_DECLARE_NON_COPYABLE(EffectChain)
};
