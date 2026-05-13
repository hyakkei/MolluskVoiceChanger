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
#include <functional>

// Owns preset definitions and handles file export/import.
// Communicates with the owner (MainComponent) via callbacks.
class PresetManager
{
public:
    // All DSP parameters (Advanced-level, used for presets and XML I/O)
    struct PresetData
    {
        const char* name;
        float gateThreshDb, gateAttackMs, gateReleaseMs;
        float hpfCutoffHz;
        float pitchSemitones;
        float formantSemitones;
        int   lfoType;  // 1=LP 2=BP 3=HP
        float lfoCutoffHz, lfoResonance, lfoRateHz, lfoDepthOct, lfoDryWet;
        float phaserRate, phaserDepth, phaserFeedback, phaserDryWet;
        float bpfCentreHz, bpfQ, bpfDryWet;
        float outputGainDb;
    };

    static const PresetData presets[4];

    // ---- Callbacks (set by owner before use) ----
    // Return an XML element representing current state for export
    std::function<std::unique_ptr<juce::XmlElement>()> onBuildXml;
    // Apply imported XML element to the owner's state
    std::function<void(const juce::XmlElement&)>       onApplyXml;
    // Apply a PresetData entry to the owner's state
    std::function<void(const PresetData&)>             onApplyPreset;

    // ---- Actions ----
    void exportToFile();
    void importFromFile();
    void triggerPreset(int index);
};
