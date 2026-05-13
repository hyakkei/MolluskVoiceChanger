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

// Horizontal VU-style bar meter with gate-open LED and threshold marker.
// Call setLevelDb(), setGateOpen(), setThresholdDb() from a timer callback,
// then repaint().
class LevelMeter : public juce::Component
{
public:
    LevelMeter() = default;

    void setLevelDb(float db)      { levelDb = db; }
    void setGateOpen(bool open)    { gateOpen = open; }
    void setThresholdDb(float db)  { thresholdDb = db; }

    void paint(juce::Graphics& g) override
    {
        auto  bounds  = getLocalBounds().toFloat();
        const float ledSize = bounds.getHeight();
        const float barW    = bounds.getWidth() - ledSize - 6.0f;

        constexpr float minDb = -60.0f;
        constexpr float maxDb =   0.0f;
        auto dbToX = [&](float db) {
            return barW * juce::jlimit(0.0f, 1.0f,
                (db - minDb) / (maxDb - minDb));
        };

        // ---- Bar background ----
        auto barBounds = bounds.withWidth(barW);
        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.fillRoundedRectangle(barBounds, 3.0f);

        // ---- Level bar (green → yellow → red) ----
        const float fillW = dbToX(levelDb);
        if (fillW > 0.0f)
        {
            juce::ColourGradient grad(
                juce::Colour(0xff44cc44), barBounds.getX(), 0,
                juce::Colour(0xffcc4444), barBounds.getRight(), 0,
                false);
            grad.addColour(0.7, juce::Colour(0xffcccc44));
            g.setGradientFill(grad);
            g.fillRoundedRectangle(barBounds.withWidth(fillW), 3.0f);
        }

        // ---- Threshold marker (vertical white line) ----
        const float threshX = barBounds.getX() + dbToX(thresholdDb);
        g.setColour(juce::Colours::white.withAlpha(0.85f));
        g.drawLine(threshX, barBounds.getY() + 1.0f,
                   threshX, barBounds.getBottom() - 1.0f, 2.0f);

        // ---- Gate LED ----
        auto ledArea   = bounds;
        auto ledBounds = ledArea.removeFromRight(ledSize).reduced(2.0f);
        g.setColour(gateOpen ? juce::Colour(0xff44ff44)
                             : juce::Colour(0xff444444));
        g.fillEllipse(ledBounds);
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.drawEllipse(ledBounds, 1.0f);
    }

private:
    float levelDb     = -100.0f;
    float thresholdDb =  -40.0f;
    bool  gateOpen    = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeter)
};
