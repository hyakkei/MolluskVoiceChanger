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

#include "AboutComponent.h"

AboutComponent::AboutComponent()
{
    titleLabel.setComponentID("about_title-label");
    titleLabel.setText("MOLLUSK VOICE CHANGER", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::left);
    addAndMakeVisible(titleLabel);

    const juce::String body =
        juce::String("Version ") + JUCE_APPLICATION_VERSION_STRING + "\n\n"
        "© 2026 Ito Hyakkei  All rights reserved.\n\n"
        "This application uses the JUCE framework.\n"
        "JUCE is © Raw Material Software Limited.\n"
        "JUCE is a trademark of Raw Material Software Limited.\n\n"
        "This software is provided “as is”, without warranty of any kind.";

    bodyLabel.setComponentID("about_body-label");
    bodyLabel.setText(body, juce::dontSendNotification);
    bodyLabel.setJustificationType(juce::Justification::topLeft);
    bodyLabel.setColour(juce::Label::textColourId, juce::Colour(0xFFAAAAAA));
    addAndMakeVisible(bodyLabel);

    logoLabel.setComponentID("about_logo-label");
    const juce::String logo = juce::String::fromUTF8("10" u8"×" "10");
    logoLabel.setText(logo, juce::dontSendNotification);
    logoLabel.setJustificationType(juce::Justification::right);
    logoLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(logoLabel);

    closeButton.setButtonText("Close");
    closeButton.onClick = [this] {
        if (auto* dw = findParentComponentOfClass<juce::DialogWindow>())
            dw->exitModalState(0);
    };
    addAndMakeVisible(closeButton);

    setSize(400, 240);
}

void AboutComponent::resized()
{
    auto area = getLocalBounds().reduced(24, 16);
    titleLabel.setBounds(area.removeFromTop(26));
    area.removeFromTop(6);
    closeButton.setBounds(area.removeFromBottom(28).withSizeKeepingCentre(80, 24));
    area.removeFromBottom(8);
    bodyLabel.setBounds(area);
    logoLabel.setBounds(getLocalBounds().reduced(24, 16).removeFromTop(32));
}

void AboutComponent::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient (
        juce::Colour(0xFF0f020b),                // 開始色
        0.0f, 0.0f,                         // 始点座標 (左上)
        juce::Colour(0xff0b0e04),                 // 終了色
        (float)getWidth(), (float)getHeight(), // 終点座標 (右下)
        false                               // リニアグラデーション（false）
    );
    gradient.addColour(0.7, juce::Colour(0xff000003));

    // 2. 塗りつぶしの種類をグラデーションに設定
    g.setGradientFill (gradient);

    // 3. 背景全体を塗りつぶす
    g.fillAll();
}
