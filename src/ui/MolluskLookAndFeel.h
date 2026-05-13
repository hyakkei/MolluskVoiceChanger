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

class MolluskLookAndFeel : public juce::LookAndFeel_V4
{
public:
    static constexpr uint32_t colAccent   = 0xFFD5006B;
    static constexpr uint32_t colGreen    = 0xFFAAFF00;
    static constexpr uint32_t colBg       = 0xFF111111;
    static constexpr uint32_t colHeaderBg = 0xFF1A2B1A;
    static constexpr uint32_t colTrack    = 0xFF3A3A3A;
    static constexpr uint32_t colPanel    = 0xFF1C1C1C;
    static constexpr uint32_t colAmber    = 0xFFB07800;
    static constexpr uint32_t colJpGray   = 0xffA0A0A0;
    static constexpr uint32_t colSquidOn  = 0xff5E7D00;
    static constexpr uint32_t colOctopusOn= 0xffA3003C;
    static constexpr uint32_t colTurquoise= 0xFF06C9B3;

    MolluskLookAndFeel();

    juce::Font getBungeeFont(float height) const;
    juce::Font getRocknRollFont(float height) const;
    juce::Font getAlfaSlabOneFont(float height) const;
    juce::Font getRubikGlitchFont(float height) const;

    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override;
    juce::Font getLabelFont(juce::Label&) override;
    juce::Font getComboBoxFont(juce::ComboBox&) override;
    juce::Font getPopupMenuFont() override;

    void drawButtonBackground(juce::Graphics&, juce::Button&,
                              const juce::Colour& bg,
                              bool highlighted, bool down) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&,
                        bool highlighted, bool down) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&,
                          bool highlighted, bool down) override;

    void drawLinearSlider(juce::Graphics&, int x, int y, int w, int h,
                          float sliderPos, float minPos, float maxPos,
                          juce::Slider::SliderStyle, juce::Slider&) override;
    void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h,
                          float sliderPos, float startAngle, float endAngle,
                          juce::Slider&) override;
    int getSliderThumbRadius(juce::Slider&) override;

    void drawLabel(juce::Graphics&, juce::Label&) override;

    void drawComboBox(juce::Graphics&, int w, int h, bool isDown,
                      int bx, int by, int bw, int bh,
                      juce::ComboBox&) override;

    void drawScrollbar(juce::Graphics&, juce::ScrollBar&,
                       int x, int y, int w, int h,
                       bool isVertical, int thumbStart, int thumbSize,
                       bool isOver, bool isDown) override;
    int getDefaultScrollbarWidth() override;

    void drawDocumentWindowTitleBar(juce::DocumentWindow&, juce::Graphics&,
                                    int w, int h,
                                    int titleSpaceX, int titleSpaceW,
                                    const juce::Image* icon,
                                    bool drawTitleTextOnLeft) override;

    juce::Button* createDocumentWindowButton(int buttonType) override;

    void drawAlertBox (Graphics&, AlertWindow&, const Rectangle<int>& textArea, TextLayout&) override;
    Font getAlertWindowTitleFont() override;
    Font getAlertWindowMessageFont() override;

private:
    juce::Typeface::Ptr bungee;
    juce::Typeface::Ptr rocknroll;
    juce::Typeface::Ptr alfaSlabOne;
    juce::Typeface::Ptr rubikGlitch;

    std::unique_ptr<juce::Drawable> squidIcon;
    std::unique_ptr<juce::Drawable> octopusIcon;

    // SVG Drawable 内の全 DrawablePath の色を置き換える
    static void recolourDrawable(juce::Drawable* d, juce::Colour c);
};
