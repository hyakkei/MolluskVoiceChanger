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

#include "OutputGain.h"

void OutputGain::prepare(const juce::dsp::ProcessSpec& spec)
{
    gain.setRampDurationSeconds(0.05);  // 50ms smoothing to avoid clicks
    gain.setGainDecibels(gainDb.load());
    gain.prepare(spec);
}

void OutputGain::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled.load()) return;

    gain.setGainDecibels(gainDb.load());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    gain.process(ctx);
}

void OutputGain::reset()
{
    gain.reset();
}
