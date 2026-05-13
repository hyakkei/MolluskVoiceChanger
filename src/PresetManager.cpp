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

#include "PresetManager.h"

// ---- Preset definitions ----
const PresetManager::PresetData PresetManager::presets[4] =
{
    { "Squid Girl",
      -40.0f,  5.0f, 100.0f,   125.0f,  12.0f,  2.0f,
      2,  3700.0f, 3.3f, 7.7f, 3.0f, 0.38f,
      2.0f, 0.55f,  0.60f, 0.38f,
      2130.0f, 4.0f, 0.42f,  0.0f },

    { "Squid Boy",
      -40.0f,  5.0f, 180.0f,   125.0f,  12.0f,  -6.5f,
      2,  3700.0f, 3.3f, 8.4f, 3.0f, 0.30f,
      2.5f, 0.55f,  0.65f, 0.60f,
      1300.0f, 3.3f, 0.25f,  0.0f },

    { "Octopus Girl",
      -40.0f,  5.0f,  180.0f,  125.0f,  12.0f,  -4.0f,
      2, 3700.0f, 2.8f, 7.7f, 2.0f, 0.28f,
      2.0f, 0.60f,  0.60f, 0.60f,
      1900.0f, 4.0f, 0.50f,  0.0f },

    { "Octopus Boy",
      -40.0f,  5.0f,  180.0f,  190.0f,  0.0f,  12.0f,
      2,  3700.0f, 3.2f, 8.4f, 2.8f, 0.25f,
      9.0f, 0.40f,  0.46f, 0.40f,
      3000.0f, 6.0f, 0.70f,  0.0f },
};

// ---- Actions ----

void PresetManager::triggerPreset(int index)
{
    jassert(index >= 0 && index < 4);
    if (onApplyPreset) onApplyPreset(presets[index]);
}

void PresetManager::exportToFile()
{
    if (!onBuildXml) return;

    auto chooser = std::make_shared<juce::FileChooser>(
        "Save Preset",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
            .getChildFile("preset.xml"),
        "*.xml");

    chooser->launchAsync(
        juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fc)
        {
            auto file = fc.getResult().withFileExtension("xml");
            if (file.getFullPathName().isEmpty()) return;

            auto xml = onBuildXml();
            if (!xml || !xml->writeTo(file))
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::WarningIcon,
                    "Export Failed", "Could not write preset file.", "OK");
        });
}

void PresetManager::importFromFile()
{
    if (!onApplyXml) return;

    auto chooser = std::make_shared<juce::FileChooser>(
        "Load Preset",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
        "*.xml");

    chooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (!file.existsAsFile()) return;

            auto xml = juce::parseXML(file);
            if (!xml || xml->getTagName() != "MolluskVoiceChangerPreset")
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::WarningIcon,
                    "Import Failed", "Invalid preset file.", "OK");
                return;
            }
            onApplyXml(*xml);
        });
}
