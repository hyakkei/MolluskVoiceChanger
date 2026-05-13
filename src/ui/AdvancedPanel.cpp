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

#include "AdvancedPanel.h"
#include "MolluskLookAndFeel.h"

static juce::String jp(const char* utf8) { return juce::String::fromUTF8(utf8); }

namespace {
    constexpr int labelW     = 100;
    constexpr int rowH       = 32;
    constexpr int sectionPad = 6;

    void initSlider(juce::Slider& s, juce::Label& lbl, const juce::String& name,
                    double min, double max, double val, double step = 0.0)
    {
        lbl.setText(name, juce::dontSendNotification);
        s.setRange(min, max, step);
        s.setValue(val, juce::dontSendNotification);
        s.setSliderStyle(juce::Slider::LinearHorizontal);
        s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    }
}

AdvancedPanel::AdvancedPanel()
{
    // ---- Noise Gate ----
    gateLabel.setText(jp(u8"NOISE GATE / ノイズゲート"), juce::dontSendNotification);
    gateLabel.setComponentID("section-header");
    initSlider(gateThreshold, gateThreshLabel,  "Threshold (dB)", -80.0,   0.0, -40.0, 1.0);
    initSlider(gateAttack,    gateAttackLabel,  "Attack (ms)",      1.0, 100.0,   5.0, 1.0);
    initSlider(gateRelease,   gateReleaseLabel, "Release (ms)",    10.0, 500.0, 100.0, 5.0);
    addAndMakeVisible(gateLabel);       addAndMakeVisible(gateBypass);
    addAndMakeVisible(gateThreshLabel); addAndMakeVisible(gateThreshold);
    addAndMakeVisible(gateAttackLabel); addAndMakeVisible(gateAttack);
    addAndMakeVisible(gateReleaseLabel);addAndMakeVisible(gateRelease);

    // ---- HPF ----
    hpfLabel.setText(jp(u8"HIGH PASS FILTER / ハイパスフィルター"), juce::dontSendNotification);
    hpfLabel.setComponentID("section-header");
    initSlider(hpfCutoff, hpfCutoffLabel, "Cutoff (Hz)", 20.0, 500.0, 80.0, 1.0);
    addAndMakeVisible(hpfLabel);      addAndMakeVisible(hpfBypass);
    addAndMakeVisible(hpfCutoffLabel);addAndMakeVisible(hpfCutoff);

    // ---- Pitch Shifter ----
    pitchLabel.setText(jp(u8"PITCH SHIFT / ピッチシフト"), juce::dontSendNotification);
    pitchLabel.setComponentID("section-header");
    initSlider(pitchSlider, pitchSliderLabel, "Semitones", 0.0, 12.0, 0.0, 0.5);
    addAndMakeVisible(pitchLabel);       addAndMakeVisible(pitchBypass);
    addAndMakeVisible(pitchSliderLabel); addAndMakeVisible(pitchSlider);

    // ---- Formant Shifter ----
    formantLabel.setText(jp(u8"FORMANT SHIFT / フォルマントシフト"), juce::dontSendNotification);
    formantLabel.setComponentID("section-header");
    initSlider(formantSlider, formantSliderLabel, "Shift (semitones)", -12.0, 12.0, 0.0, 0.5);
    addAndMakeVisible(formantLabel);       addAndMakeVisible(formantBypass);
    addAndMakeVisible(formantSliderLabel); addAndMakeVisible(formantSlider);

    // ---- LFO Filter ----
    lfoFilterLabel.setText(jp(u8"LFO FILTER / LFOフィルター"), juce::dontSendNotification);
    lfoFilterLabel.setComponentID("section-header");
    lfoFilterTypeLabel.setText("Type", juce::dontSendNotification);
    lfoFilterTypeBox.addItem("Lowpass",  1);
    lfoFilterTypeBox.addItem("Bandpass", 2);
    lfoFilterTypeBox.addItem("Highpass", 3);
    lfoFilterTypeBox.setSelectedId(1, juce::dontSendNotification);
    initSlider(lfoFilterCutoff,    lfoFilterCutoffLabel,    "Cutoff (Hz)",     20.0, 10000.0, 1000.0, 1.0);
    initSlider(lfoFilterResonance, lfoFilterResonanceLabel, "Resonance",        0.1,     4.0,    0.7, 0.01);
    initSlider(lfoFilterRate,      lfoFilterRateLabel,      "LFO Rate (Hz)",    0.1,    20.0,    2.0, 0.1);
    initSlider(lfoFilterDepth,     lfoFilterDepthLabel,     "LFO Depth (oct)",  0.0,     4.0,    1.0, 0.1);
    initSlider(lfoFilterDryWet,    lfoFilterDryWetLabel,    "Dry/Wet",          0.0,     1.0,    1.0, 0.01);
    addAndMakeVisible(lfoFilterLabel);       addAndMakeVisible(lfoFilterBypass);
    addAndMakeVisible(lfoFilterTypeLabel);   addAndMakeVisible(lfoFilterTypeBox);
    addAndMakeVisible(lfoFilterCutoffLabel); addAndMakeVisible(lfoFilterCutoff);
    addAndMakeVisible(lfoFilterResonanceLabel); addAndMakeVisible(lfoFilterResonance);
    addAndMakeVisible(lfoFilterRateLabel);   addAndMakeVisible(lfoFilterRate);
    addAndMakeVisible(lfoFilterRandomRate);
    addAndMakeVisible(lfoFilterDepthLabel);  addAndMakeVisible(lfoFilterDepth);
    addAndMakeVisible(lfoFilterDryWetLabel); addAndMakeVisible(lfoFilterDryWet);

    // ---- Phaser ----
    phaserLabel.setText(jp(u8"PHASER / フェイザー"), juce::dontSendNotification);
    phaserLabel.setComponentID("section-header");
    initSlider(phaserRate,     phaserRateLabel,     "Rate (Hz)",  0.1, 10.0, 3.0, 0.1);
    initSlider(phaserDepth,    phaserDepthLabel,    "Depth",      0.0,  1.0, 0.7, 0.01);
    initSlider(phaserFeedback, phaserFeedbackLabel, "Feedback",  -1.0,  1.0, 0.6, 0.01);
    initSlider(phaserDryWet,   phaserDryWetLabel,   "Dry/Wet",    0.0,  1.0, 1.0, 0.01);
    addAndMakeVisible(phaserLabel);        addAndMakeVisible(phaserBypass);
    addAndMakeVisible(phaserRateLabel);    addAndMakeVisible(phaserRate);
    addAndMakeVisible(phaserDepthLabel);   addAndMakeVisible(phaserDepth);
    addAndMakeVisible(phaserFeedbackLabel);addAndMakeVisible(phaserFeedback);
    addAndMakeVisible(phaserDryWetLabel);  addAndMakeVisible(phaserDryWet);

    // ---- BPF ----
    bpfLabel.setText(jp(u8"BAND PASS FILTER / バンドパスフィルター"), juce::dontSendNotification);
    bpfLabel.setComponentID("section-header");
    initSlider(bpfCentre,  bpfCentreLabel, "Centre (Hz)", 200.0, 8000.0, 2000.0, 10.0);
    initSlider(bpfQ,       bpfQLabel,      "Q",             0.1,    8.0,    0.7,  0.1);
    initSlider(bpfDryWet,  bpfDryWetLabel, "Dry/Wet",       0.0,    1.0,    1.0, 0.01);
    addAndMakeVisible(bpfLabel);       addAndMakeVisible(bpfBypass);
    addAndMakeVisible(bpfCentreLabel); addAndMakeVisible(bpfCentre);
    addAndMakeVisible(bpfQLabel);      addAndMakeVisible(bpfQ);
    addAndMakeVisible(bpfDryWetLabel); addAndMakeVisible(bpfDryWet);

    // ---- Output Gain ----
    gainLabel.setText(jp(u8"OUTPUT GAIN / アウトプット"), juce::dontSendNotification);
    gainLabel.setComponentID("section-header");
    initSlider(gainSlider, gainSliderLabel, "Gain (dB)", -60.0, 12.0, 0.0, 0.1);
    addAndMakeVisible(gainLabel);      addAndMakeVisible(gainBypass);
    addAndMakeVisible(gainSliderLabel);addAndMakeVisible(gainSlider);
}

void AdvancedPanel::paint(juce::Graphics& g)
{
    for (const auto& r : sectionHeaderRects)
    {
        g.setColour(juce::Colour(MolluskLookAndFeel::colHeaderBg));
        g.fillRoundedRectangle(r.toFloat(), 4.0f);
    }
}

void AdvancedPanel::resized()
{
    layoutIn(getLocalBounds());
}

void AdvancedPanel::layoutIn(juce::Rectangle<int> area)
{
    sectionHeaderRects.clearQuick();

    auto section = [&](juce::Label& sectionLabel, juce::ToggleButton& bypass,
                       std::initializer_list<std::pair<juce::Label*, juce::Slider*>> rows)
    {
        auto s = area.removeFromTop((int)rows.size() * rowH + rowH + sectionPad * 2);
        s.removeFromTop(sectionPad);
        auto titleRow = s.removeFromTop(rowH);

        sectionHeaderRects.add(titleRow);
        bypass.setBounds(titleRow.withLeft(titleRow.getRight() - 80));
        sectionLabel.setBounds(titleRow.withRight(titleRow.getRight() - 84));

        for (auto& [lbl, sl] : rows)
        {
            auto row = s.removeFromTop(rowH);
            lbl->setBounds(row.removeFromLeft(labelW));
            sl->setBounds(row);
        }
        area.removeFromTop(sectionPad);
    };

    section(gateLabel, gateBypass, {
        {&gateThreshLabel,  &gateThreshold},
        {&gateAttackLabel,  &gateAttack},
        {&gateReleaseLabel, &gateRelease}
    });
    section(hpfLabel,    hpfBypass,    { {&hpfCutoffLabel,    &hpfCutoff} });
    section(pitchLabel,  pitchBypass,  { {&pitchSliderLabel,  &pitchSlider} });
    section(formantLabel,formantBypass,{ {&formantSliderLabel,&formantSlider} });

    // LFO Filter (has ComboBox row + sliders)
    {
        const int numRows = 6;
        auto s = area.removeFromTop(numRows * rowH + rowH + sectionPad * 2);
        s.removeFromTop(sectionPad);
        auto titleRow = s.removeFromTop(rowH);

        sectionHeaderRects.add(titleRow);
        lfoFilterBypass.setBounds(titleRow.withLeft(titleRow.getRight() - 80));
        lfoFilterLabel.setBounds(titleRow.withRight(titleRow.getRight() - 84));

        {
            auto r = s.removeFromTop(rowH);
            lfoFilterTypeLabel.setBounds(r.removeFromLeft(labelW));
            lfoFilterTypeBox.setBounds(r.removeFromLeft(130));
        }
        auto slRow = [&](juce::Label& l, juce::Slider& sl) {
            auto r = s.removeFromTop(rowH);
            l.setBounds(r.removeFromLeft(labelW));
            sl.setBounds(r);
        };
        slRow(lfoFilterCutoffLabel,    lfoFilterCutoff);
        slRow(lfoFilterResonanceLabel, lfoFilterResonance);
        {
            auto r = s.removeFromTop(rowH);
            lfoFilterRateLabel.setBounds(r.removeFromLeft(labelW));
            lfoFilterRandomRate.setBounds(r.removeFromRight(80));
            lfoFilterRate.setBounds(r);
        }
        slRow(lfoFilterDepthLabel,  lfoFilterDepth);
        slRow(lfoFilterDryWetLabel, lfoFilterDryWet);
        area.removeFromTop(sectionPad);
    }

    section(phaserLabel, phaserBypass, {
        {&phaserRateLabel,     &phaserRate},
        {&phaserDepthLabel,    &phaserDepth},
        {&phaserFeedbackLabel, &phaserFeedback},
        {&phaserDryWetLabel,   &phaserDryWet}
    });
    section(bpfLabel, bpfBypass, {
        {&bpfCentreLabel, &bpfCentre},
        {&bpfQLabel,      &bpfQ},
        {&bpfDryWetLabel, &bpfDryWet}
    });
    section(gainLabel, gainBypass, { {&gainSliderLabel, &gainSlider} });
}
