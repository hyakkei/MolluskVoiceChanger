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

#include "HighPassFilter.h"

void HighPassFilter::prepare(const juce::dsp::ProcessSpec& spec)
{
    filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    filter.setCutoffFrequency(cutoffHz.load());
    filter.prepare(spec);
}

void HighPassFilter::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled.load()) return;

    // StateVariableTPTFilter supports real-time cutoff updates
    filter.setCutoffFrequency(cutoffHz.load());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    filter.process(ctx);
}

void HighPassFilter::reset()
{
    filter.reset();
}
