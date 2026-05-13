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

class OutputGain
{
public:
    OutputGain() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void setEnabled(bool e)    { enabled.store(e); }
    void setGainDb(float db)   { gainDb.store(db); }

    bool  getEnabled() const   { return enabled.load(); }
    float getGainDb()  const   { return gainDb.load(); }

private:
    juce::dsp::Gain<float> gain;

    std::atomic<float> gainDb  { 0.0f };
    std::atomic<bool>  enabled { true };

    JUCE_DECLARE_NON_COPYABLE(OutputGain)
};
