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

#include "MolluskLookAndFeel.h"
#include <BinaryData.h>

MolluskLookAndFeel::MolluskLookAndFeel()
{
    bungee = juce::Typeface::createSystemTypefaceFor(
        BinaryData::BungeeRegular_ttf, BinaryData::BungeeRegular_ttfSize);
    rocknroll = juce::Typeface::createSystemTypefaceFor(
        BinaryData::RocknRollOneRegular_ttf, BinaryData::RocknRollOneRegular_ttfSize);
    alfaSlabOne = juce::Typeface::createSystemTypefaceFor(
        BinaryData::AlfaSlabOneRegular_ttf, BinaryData::AlfaSlabOneRegular_ttfSize);
    rubikGlitch = juce::Typeface::createSystemTypefaceFor(
        BinaryData::RubikGlitchRegular_ttf, BinaryData::RubikGlitchRegular_ttfSize);

    // SVG アイコンをロードして白色に統一
    auto loadSvg = [](const char* data, int size) -> std::unique_ptr<juce::Drawable>
    {
        if (auto xml = juce::XmlDocument::parse(juce::String::fromUTF8(data, size)))
            return juce::Drawable::createFromSVG(*xml);
        return nullptr;
    };
    squidIcon   = loadSvg(BinaryData::squid2_svg,  BinaryData::squid2_svgSize);
    octopusIcon = loadSvg(BinaryData::octopus_svg, BinaryData::octopus_svgSize);
    if (squidIcon)   recolourDrawable(squidIcon.get(),   juce::Colours::white);
    if (octopusIcon) recolourDrawable(octopusIcon.get(), juce::Colours::white);

    // タイトルバー
    setColour(juce::DocumentWindow::backgroundColourId,         juce::Colour(colAccent));
    setColour(juce::DocumentWindow::textColourId,               juce::Colours::white);

    setColour(juce::ResizableWindow::backgroundColourId,        juce::Colour(colBg));
    setColour(juce::TextButton::buttonColourId,                  juce::Colour(colPanel));
    setColour(juce::TextButton::buttonOnColourId,                juce::Colour(colAccent).darker(0.4f));
    setColour(juce::TextButton::textColourOffId,                 juce::Colours::white);
    setColour(juce::TextButton::textColourOnId,                  juce::Colours::white);
    setColour(juce::Slider::thumbColourId,                       juce::Colour(colAccent));
    setColour(juce::Slider::trackColourId,                       juce::Colour(colAccent));
    setColour(juce::Slider::backgroundColourId,                  juce::Colour(colTrack));
    setColour(juce::Slider::textBoxTextColourId,                 juce::Colour(colGreen));
    setColour(juce::Slider::textBoxBackgroundColourId,           juce::Colour(0xFF1A1A1A));
    setColour(juce::Slider::textBoxOutlineColourId,              juce::Colour(0xFF444444));
    setColour(juce::Label::textColourId,                         juce::Colours::white);
    setColour(juce::Label::backgroundColourId,                   juce::Colours::transparentBlack);
    setColour(juce::ComboBox::backgroundColourId,                juce::Colour(colPanel));
    setColour(juce::ComboBox::textColourId,                      juce::Colours::white);
    setColour(juce::ComboBox::outlineColourId,                   juce::Colour(0xFF444444));
    setColour(juce::ComboBox::arrowColourId,                     juce::Colours::white);
    setColour(juce::PopupMenu::backgroundColourId,               juce::Colour(0xFF222222));
    setColour(juce::PopupMenu::textColourId,                     juce::Colours::white);
    setColour(juce::PopupMenu::highlightedBackgroundColourId,    juce::Colour(colAccent));
    setColour(juce::ToggleButton::textColourId,                  juce::Colours::white);
    setColour(juce::ToggleButton::tickColourId,                  juce::Colour(colAccent));
    setColour(juce::ToggleButton::tickDisabledColourId,          juce::Colour(0xFF555555));
}

juce::Font MolluskLookAndFeel::getBungeeFont(float height) const
{
    if (bungee)
        return juce::Font(juce::FontOptions().withTypeface(bungee).withHeight(height));
    return juce::Font(height, juce::Font::bold);
}

juce::Font MolluskLookAndFeel::getRocknRollFont(float height) const
{
    if (rocknroll)
        return juce::Font(juce::FontOptions().withTypeface(rocknroll).withHeight(height));
    return juce::Font(height);
}

juce::Font MolluskLookAndFeel::getAlfaSlabOneFont(float height) const
{
    if (alfaSlabOne)
        return juce::Font(juce::FontOptions().withTypeface(alfaSlabOne).withHeight(height));
    return juce::Font(height);
}

juce::Font MolluskLookAndFeel::getRubikGlitchFont(float height) const
{
    if (rubikGlitch)
        return juce::Font(juce::FontOptions().withTypeface(rubikGlitch).withHeight(height));
    return juce::Font(height);
}

juce::Font MolluskLookAndFeel::getTextButtonFont(juce::TextButton&, int buttonHeight)
{
    return getBungeeFont(std::min(16.0f, buttonHeight * 0.35f));
}

juce::Font MolluskLookAndFeel::getLabelFont(juce::Label& label)
{
    const auto id = label.getComponentID();
    if (id == "section-header") return getRocknRollFont(13.0f);
    if (id == "jap-label")      return getRocknRollFont(12.0f);
    if (id == "about_title-label")  return getBungeeFont(20.0f);
    if (id == "about_body-label")   return getAlfaSlabOneFont(14.0f);
    if (id == "about_logo-label")   return getRubikGlitchFont(36.0f);
   return getBungeeFont(14.0f);
}

juce::Font MolluskLookAndFeel::getComboBoxFont(juce::ComboBox&)
{
    return getBungeeFont(13.0f);
}

juce::Font MolluskLookAndFeel::getPopupMenuFont()
{
    return getBungeeFont(13.0f);
}

void MolluskLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                               const juce::Colour& bg,
                                               bool highlighted, bool down)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);

    // プリセットボタンはアクティブ色を setColour() で直接設定しているので bg を使用
    const auto id = button.getComponentID();

    // タイトルバーの「?」ボタン: 透明背景、ホバー時のみ薄く光る
    if (id == "about-button")
    {
        if (down)
            g.setColour(juce::Colours::white.withAlpha(0.25f));
        else if (highlighted)
            g.setColour(juce::Colours::white.withAlpha(0.15f));
        else
            return;
        g.fillRoundedRectangle(bounds, 4.0f);
        return;
    }

    juce::Colour fill;
    if (id == "mode-button")
        fill = juce::Colour(colAmber);
    else if (id == "preset-squid-girl" || id == "preset-squid-boy"
          || id == "preset-octopus-girl" || id == "preset-octopus-boy")
        fill = bg;
    else if (button.getToggleState())
        fill = juce::Colour(colAccent).darker(0.4f);
    else
        fill = juce::Colour(colPanel);

    if (down)             fill = fill.darker(0.2f);
    else if (highlighted) fill = fill.brighter(0.12f);

    g.setColour(fill);
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(juce::Colour(0xFF484848));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);
}

void MolluskLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                         bool isOver, bool isDown)
{
    const auto bounds = button.getLocalBounds();
    const auto text   = button.getButtonText();
    const int  nl     = text.indexOfChar('\n');

    const auto id = button.getComponentID();

    if (id == "about-button")
    {
        g.setColour(isOver ? juce::Colours::white : juce::Colours::white.withAlpha(0.55f));
        g.setFont(getAlfaSlabOneFont(bounds.getHeight() * 0.7f));
        g.drawText(text, bounds, juce::Justification::centred, false);
        return;
    }
    if (nl >= 0 && (id == "preset-squid-girl" || id == "preset-squid-boy"
                    || id == "preset-octopus-girl" || id == "preset-octopus-boy"))
    {
        const auto line1 = text.substring(0, nl);
        const auto line2 = text.substring(nl + 1);

        // アイコン領域：左端に縦いっぱい、幅は高さの 0.60 倍
        const float iconW   = bounds.getHeight() * 0.60f;
        const float iconPad = 5.0f;
        const auto  iconRect = juce::Rectangle<float>(
            bounds.getX() + iconPad,
            bounds.getY() + 2.0f,
            iconW,
            bounds.getHeight() - 4.0f);

        auto* drawable = id.startsWith("preset-squid") ? squidIcon.get() : octopusIcon.get();
        if (drawable)
            drawable->drawWithin(g, iconRect,
                                 juce::RectanglePlacement::centred, 0.85f);

        // 性別記号をアイコンの中央に重ねて描画
        const bool        isGirl   = id.endsWith("-girl");
        const juce::String sex     = juce::String::fromUTF8(isGirl ? u8"♀" : u8"♂");
        const juce::Colour symCol  = isGirl ? juce::Colour(colAccent)
                                            : juce::Colour(colTurquoise);
        const float symSize = iconRect.getHeight() * 0.44f;
        const auto  symRect = juce::Rectangle<float>(
            iconRect.getCentreX() - symSize * 0.5f,
            iconRect.getCentreY() - symSize * 0.5f - 3.0f,
            symSize, symSize);
        g.setColour(symCol);
        g.setFont(getRocknRollFont(symSize * 0.9f));
        g.drawText(sex, symRect, juce::Justification::centred, false);

        // テキスト領域：アイコンの右側
        const auto textRect = bounds.withTrimmedLeft((int)(iconPad + iconW + 2));
        const int  half     = bounds.getHeight() / 2;
        g.setColour(juce::Colours::white);
        g.setFont(getBungeeFont(15.0f));
        g.drawText(line1, textRect.withHeight(half), juce::Justification::centredBottom, false);
        g.setColour(juce::Colour(colJpGray));
        g.setFont(getRocknRollFont(14.0f));
        g.drawText(line2, textRect.withTrimmedTop(half), juce::Justification::centredTop, false);
    }
    else if (nl >= 0)
    {
        const auto line1 = text.substring(0, nl);
        const auto line2 = text.substring(nl + 1);
        const int  half  = bounds.getHeight() / 2;
        g.setColour(juce::Colours::white);
        g.setFont(getBungeeFont(12.0f));
        g.drawText(line1, bounds.withHeight(half), juce::Justification::centredBottom, false);
        g.drawText(line2, bounds.withTrimmedTop(half), juce::Justification::centredTop, false);
    }
    else
    {
        g.setColour(juce::Colours::white);
        g.setFont(getBungeeFont(std::min(14.0f, bounds.getHeight() * 0.4f)));
        g.drawText(text, bounds, juce::Justification::centred, false);
    }
}

void MolluskLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                           bool, bool)
{
    const float boxSize = 14.0f;
    const float boxX    = 1.0f;
    const float boxY    = (button.getHeight() - boxSize) * 0.5f;

    g.setColour(juce::Colour(colTrack));
    g.fillRoundedRectangle(boxX, boxY, boxSize, boxSize, 3.0f);
    g.setColour(juce::Colour(0xFF555555));
    g.drawRoundedRectangle(boxX, boxY, boxSize, boxSize, 3.0f, 1.0f);

    if (button.getToggleState())
    {
        g.setColour(juce::Colour(colAccent));
        const float pad = 3.0f;
        g.fillRoundedRectangle(boxX + pad, boxY + pad,
                               boxSize - pad * 2, boxSize - pad * 2, 2.0f);
    }

    g.setColour(juce::Colours::white);
    g.setFont(getBungeeFont(12.0f));
    g.drawText(button.getButtonText(),
               (int)(boxX + boxSize + 4), 0,
               button.getWidth() - (int)(boxX + boxSize + 4),
               button.getHeight(),
               juce::Justification::centredLeft, false);
}

void MolluskLookAndFeel::drawLinearSlider(juce::Graphics& g,
                                           int x, int y, int w, int h,
                                           float sliderPos, float minPos, float maxPos,
                                           juce::Slider::SliderStyle style,
                                           juce::Slider& slider)
{
    if (style == juce::Slider::LinearHorizontal)
    {
        const float trackH = 4.0f;
        const float cy     = y + h * 0.5f;
        const float thumbR = 8.0f;

        g.setColour(juce::Colour(colTrack));
        g.fillRoundedRectangle((float)x, cy - trackH * 0.5f, (float)w, trackH, 2.0f);

        const float fillW = sliderPos - x;
        if (fillW > 0.0f)
        {
            g.setColour(juce::Colour(colAccent));
            g.fillRoundedRectangle((float)x, cy - trackH * 0.5f, fillW, trackH, 2.0f);
        }

        g.setColour(juce::Colour(colAccent));
        g.fillEllipse(sliderPos - thumbR, cy - thumbR, thumbR * 2, thumbR * 2);
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawEllipse(sliderPos - thumbR, cy - thumbR, thumbR * 2, thumbR * 2, 1.0f);
    }
    else
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, w, h, sliderPos, minPos, maxPos, style, slider);
    }
}

void MolluskLookAndFeel::drawRotarySlider(juce::Graphics& g,
                                           int x, int y, int w, int h,
                                           float sliderPos, float startAngle, float endAngle,
                                           juce::Slider&)
{
    const float radius   = std::min(w * 0.5f, h * 0.5f) - 6.0f;
    const float cx       = x + w * 0.5f;
    const float cy       = y + h * 0.5f;
    const float angle    = startAngle + sliderPos * (endAngle - startAngle);
    const float arcThick = 10.0f;

    juce::Path bgPath;
    bgPath.addCentredArc(cx, cy, radius, radius, 0.0f, startAngle, endAngle, true);
    g.setColour(juce::Colour(colTrack));
    g.strokePath(bgPath, juce::PathStrokeType(arcThick,
                                               juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));

    if (sliderPos > 0.001f)
    {
        juce::Path valPath;
        valPath.addCentredArc(cx, cy, radius, radius, 0.0f, startAngle, angle, true);
        g.setColour(juce::Colour(colAccent));
        g.strokePath(valPath, juce::PathStrokeType(arcThick,
                                                    juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));
    }

    const float dotR    = 5.5f;
    const float dotDist = radius;
    const float dotX    = cx + std::sin(angle) * dotDist;
    const float dotY    = cy - std::cos(angle) * dotDist;
    g.setColour(juce::Colour(colAmber));
    g.fillEllipse(dotX - dotR, dotY - dotR, dotR * 2, dotR * 2);
}

int MolluskLookAndFeel::getSliderThumbRadius(juce::Slider&) { return 8; }

void MolluskLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    const auto bgColor = label.findColour(juce::Label::backgroundColourId);
    if (!bgColor.isTransparent())
        g.fillAll(bgColor);

    if (!label.isBeingEdited())
    {
        const auto id = label.getComponentID();

        if (id == "section-header")
        {
            const auto text  = label.getText();
            const auto area  = label.getLocalBounds().reduced(6, 0);
            const int  slash = text.indexOfChar('/');

            g.setColour(juce::Colour(colGreen));

            if (slash >= 0)
            {
                const auto engPart    = text.substring(0, slash);
                const auto jpPart     = text.substring(slash);
                const auto bungeeFont = getBungeeFont(16.0f);
                const int  engW       = bungeeFont.getStringWidth(engPart);

                g.setFont(bungeeFont);
                g.drawText(engPart, area.withWidth(engW),
                           juce::Justification::centredLeft, false);

                g.setColour(juce::Colour(colJpGray));
                g.setFont(getRocknRollFont(14.0f));
                g.drawText(jpPart, area.withTrimmedLeft(engW),
                           juce::Justification::centredLeft, false);
            }
            else
            {
                g.setFont(getBungeeFont(16.0f));
                g.drawText(text, area, juce::Justification::centredLeft, false);
            }
            return;
        }

        if (id == "jap-label")
        {
            g.setColour(juce::Colour(colJpGray));
            g.setFont(getRocknRollFont(14.0f));
            g.drawText(label.getText(),
                       label.getLocalBounds(),
                       label.getJustificationType(), false);
            return;
        }

        if (id == "val-label")
        {
            g.setColour(juce::Colour(colGreen));
            g.setFont(getBungeeFont(16.0f));
            g.drawText(label.getText(),
                       label.getLocalBounds(),
                       label.getJustificationType(), false);
            return;
        }
    }

    LookAndFeel_V4::drawLabel(g, label);
}

void MolluskLookAndFeel::drawComboBox(juce::Graphics& g, int w, int h, bool,
                                       int bx, int by, int bw, int bh,
                                       juce::ComboBox&)
{
    g.setColour(juce::Colour(colPanel));
    g.fillRoundedRectangle(0.0f, 0.0f, (float)w, (float)h, 4.0f);
    g.setColour(juce::Colour(0xFF444444));
    g.drawRoundedRectangle(0.5f, 0.5f, w - 1.0f, h - 1.0f, 4.0f, 1.0f);

    const float arrowX = bx + bw * 0.5f;
    const float arrowY = by + bh * 0.5f;
    juce::Path arrow;
    arrow.addTriangle(arrowX - 5.0f, arrowY - 2.0f,
                      arrowX + 5.0f, arrowY - 2.0f,
                      arrowX,        arrowY + 4.0f);
    g.setColour(juce::Colours::white);
    g.fillPath(arrow);
}

void MolluskLookAndFeel::drawScrollbar(juce::Graphics& g, juce::ScrollBar&,
                                        int x, int y, int w, int h,
                                        bool isVertical,
                                        int thumbStart, int thumbSize,
                                        bool isOver, bool isDown)
{
    g.setColour(juce::Colour(0xFF1A1A1A));
    g.fillRect(x, y, w, h);

    const int margin = 2;
    const juce::Colour thumbColour = (isOver || isDown)
        ? juce::Colour(colAccent)
        : juce::Colour(0xFF555555);

    if (isVertical)
    {
        g.setColour(thumbColour);
        g.fillRoundedRectangle((float)(x + margin),
                               (float)(y + thumbStart + margin),
                               (float)(w - margin * 2),
                               (float)(thumbSize - margin * 2), 3.0f);
    }
    else
    {
        g.setColour(thumbColour);
        g.fillRoundedRectangle((float)(x + thumbStart + margin),
                               (float)(y + margin),
                               (float)(thumbSize - margin * 2),
                               (float)(h - margin * 2), 3.0f);
    }
}

int MolluskLookAndFeel::getDefaultScrollbarWidth() { return 8; }

// SVG Drawable 内の全 DrawablePath の色を指定色に置換する（再帰）
void MolluskLookAndFeel::recolourDrawable(juce::Drawable* d, juce::Colour c)
{
    if (auto* path = dynamic_cast<juce::DrawablePath*>(d))
    {
        path->setFill(juce::FillType(c));
        path->setStrokeFill(juce::FillType(c));
    }
    for (int i = 0; i < d->getNumChildComponents(); ++i)
        if (auto* child = dynamic_cast<juce::Drawable*>(d->getChildComponent(i)))
            recolourDrawable(child, c);
}

juce::Button* MolluskLookAndFeel::createDocumentWindowButton(int buttonType)
{
    juce::Path shape;
    const float thick = 0.15f;

    if (buttonType == juce::DocumentWindow::closeButton)
    {
        shape.addLineSegment({ 0.0f, 0.0f, 0.8f, 0.8f }, thick);
        shape.addLineSegment({ 0.8f, 0.0f, 0.0f, 0.8f }, thick);
    }
    else if (buttonType == juce::DocumentWindow::minimiseButton)
    {
        shape.addRectangle(0.1f, 0.5f - thick * 0.5f, 0.8f, thick);
    }
    else
    {
        return LookAndFeel_V4::createDocumentWindowButton(buttonType);
    }

    // 通常：半透明の白アイコン
    juce::DrawablePath normal;
    normal.setFill(juce::Colours::white.withAlpha(0.55f));
    normal.setPath(shape);

    // ホバー：不透明の白（閉じるは赤）
    juce::DrawablePath over;
    over.setFill(buttonType == juce::DocumentWindow::closeButton
                 ? juce::Colour(0xFFFF5555)
                 : juce::Colours::white);
    over.setPath(shape);

    // 押下：さらに暗く
    juce::DrawablePath down;
    down.setFill(buttonType == juce::DocumentWindow::closeButton
                 ? juce::Colour(0xFFCC3333)
                 : juce::Colours::white.withAlpha(0.75f));
    down.setPath(shape);

    auto* b = new juce::DrawableButton("button", juce::DrawableButton::ImageFitted);
    b->setImages(&normal, &over, &down);
    b->setWantsKeyboardFocus(false);
    return b;
}

void MolluskLookAndFeel::drawDocumentWindowTitleBar(
    juce::DocumentWindow& window, juce::Graphics& g,
    int w, int h, int titleSpaceX, int titleSpaceW,
    const juce::Image*, bool drawTitleTextOnLeft)
{
    // タイトルバー背景（アクセントカラー）
    g.setColour(juce::Colour(colTurquoise));
    g.fillAll();

    // タイトル文字列：大文字 + Bungee フォント
    g.setColour(juce::Colours::white);
    g.setFont(getBungeeFont(14.0f));
    g.drawText(window.getName().toUpperCase(),
               titleSpaceX, 0, titleSpaceW, h,
               drawTitleTextOnLeft ? juce::Justification::centredLeft
                                   : juce::Justification::centred,
               true);
}

void MolluskLookAndFeel::drawAlertBox (Graphics& g, AlertWindow& alert, const Rectangle<int>& textArea, TextLayout& textLayout)
{
    if (alert.getAlertType() != MessageBoxIconType::NoIcon)
    {
        return LookAndFeel_V4::drawAlertBox(g, alert, textArea, textLayout);
    }
    else
    {
        auto cornerSize = 4.0f;

        g.setColour (alert.findColour (AlertWindow::outlineColourId));
        g.drawRoundedRectangle (alert.getLocalBounds().toFloat(), cornerSize, 2.0f);

        auto bounds = alert.getLocalBounds().reduced (1);
        g.reduceClipRegion (bounds);


        juce::ColourGradient gradient (
            juce::Colour(0xFF0f020b),
            0.0f, 0.0f,
            juce::Colour(0xff0b0e04),
            (float)alert.getWidth(), (float)alert.getHeight(),
            false
        );
        gradient.addColour(0.7, juce::Colour(0xff000003));

        g.setGradientFill (gradient);
        g.fillAll();
        g.fillRoundedRectangle (bounds.toFloat(), cornerSize);

        g.setColour (alert.findColour (AlertWindow::textColourId));

        auto iconSpaceUsed = 0;
        Rectangle<int> alertBounds (bounds.getX() + iconSpaceUsed, 30,
                                    bounds.getWidth(), bounds.getHeight() - getAlertWindowButtonHeight() - 20);
        
        textLayout.draw (g, alertBounds.toFloat());
    }
}

Font MolluskLookAndFeel::getAlertWindowTitleFont()
{
    return getBungeeFont(16.0f);
}
Font MolluskLookAndFeel::getAlertWindowMessageFont()
{
    return getBungeeFont(12.0f);
}