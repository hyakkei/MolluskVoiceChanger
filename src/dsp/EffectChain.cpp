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

#include "EffectChain.h"

void EffectChain::prepare(const juce::dsp::ProcessSpec& spec)
{
    gate.prepare(spec);
    hpf.prepare(spec);
    pitchShifter.prepare(spec);
    formantShifter.prepare(spec);
    lfoFilter.prepare(spec);
    phaser.prepare(spec);
    bpf.prepare(spec);
    outputGain.prepare(spec);
}

void EffectChain::process(juce::AudioBuffer<float>& buffer)
{
    gate.process(buffer);
    hpf.process(buffer);
    pitchShifter.process(buffer);
    formantShifter.process(buffer);
    lfoFilter.process(buffer);
    phaser.process(buffer);
    bpf.process(buffer);
    outputGain.process(buffer);
}

void EffectChain::reset()
{
    gate.reset();
    hpf.reset();
    pitchShifter.reset();
    formantShifter.reset();
    lfoFilter.reset();
    phaser.reset();
    bpf.reset();
    outputGain.reset();
}
