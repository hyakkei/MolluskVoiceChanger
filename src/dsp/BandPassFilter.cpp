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

#include "BandPassFilter.h"

void BandPassFilter::prepare(const juce::dsp::ProcessSpec& spec)
{
    filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    filter.setCutoffFrequency(centreHz.load());
    filter.setResonance(qValue.load());
    filter.prepare(spec);
}

void BandPassFilter::process(juce::AudioBuffer<float>& buffer)
{
    if (!enabled.load()) return;

    filter.setCutoffFrequency(centreHz.load());
    filter.setResonance(qValue.load());

    const float mix         = dryWet.load();
    const int   numChannels = buffer.getNumChannels();
    const int   numSamples  = buffer.getNumSamples();

    if (mix >= 1.0f)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        filter.process(ctx);
        return;
    }

    // Dry/wet mix: process into a temp buffer, then blend
    juce::AudioBuffer<float> wet(numChannels, numSamples);
    for (int ch = 0; ch < numChannels; ++ch)
        wet.copyFrom(ch, 0, buffer, ch, 0, numSamples);

    juce::dsp::AudioBlock<float> block(wet);
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    filter.process(ctx);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* dry = buffer.getReadPointer(ch);
        const float* w   = wet.getReadPointer(ch);
        float*       out = buffer.getWritePointer(ch);
        for (int i = 0; i < numSamples; ++i)
            out[i] = dry[i] * (1.0f - mix) + w[i] * mix;
    }
}

void BandPassFilter::reset()
{
    filter.reset();
}
