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

#include "PhaserProcessor.h"

void PhaserProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    phaser.prepare(spec);
    phaser.setRate(rate.load());
    phaser.setDepth(depth.load());
    phaser.setFeedback(feedback.load());
    phaser.setMix(mix.load());
}

void PhaserProcessor::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled.load()) return;

    // Update params from atomics each block (audio-thread safe)
    phaser.setRate(rate.load());
    phaser.setDepth(depth.load());
    phaser.setFeedback(feedback.load());
    phaser.setMix(mix.load());

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    phaser.process(ctx);
}

void PhaserProcessor::reset()
{
    phaser.reset();
}
