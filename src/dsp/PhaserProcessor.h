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

class PhaserProcessor
{
public:
    PhaserProcessor() = default;
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void setEnabled(bool e)         { enabled.store(e); }
    void setRate(float hz)          { rate.store(hz); }
    void setDepth(float d)          { depth.store(d); }
    void setFeedback(float fb)      { feedback.store(fb); }
    void setMix(float m)            { mix.store(m); }

    bool  getEnabled()  const       { return enabled.load(); }
    float getRate()     const       { return rate.load(); }
    float getDepth()    const       { return depth.load(); }
    float getFeedback() const       { return feedback.load(); }
    float getMix()      const       { return mix.load(); }

private:
    juce::dsp::Phaser<float> phaser;

    std::atomic<bool>  enabled  { true };
    std::atomic<float> rate     { 3.0f };
    std::atomic<float> depth    { 0.7f };
    std::atomic<float> feedback { 0.6f };
    std::atomic<float> mix      { 1.0f };

    JUCE_DECLARE_NON_COPYABLE(PhaserProcessor)
};
