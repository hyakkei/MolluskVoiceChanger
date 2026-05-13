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

#include <JuceHeader.h>
#include "MainComponent.h"
#include "ui/AboutComponent.h"

// ---- アプリケーション ----
class MolluskVoiceChangerApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override    { return JUCE_APPLICATION_NAME_STRING; }
    const juce::String getApplicationVersion() override { return JUCE_APPLICATION_VERSION_STRING; }

    void initialise(const juce::String&) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override { mainWindow = nullptr; }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                             juce::Desktop::getInstance().getDefaultLookAndFeel()
                                 .findColour(juce::DocumentWindow::backgroundColourId),
                             DocumentWindow::minimiseButton | DocumentWindow::closeButton)
        {
            setUsingNativeTitleBar(false);
            setContentOwned(new MainComponent(), true);
            setResizable(false, false);

            aboutButton.setComponentID("about-button");
            aboutButton.onClick = [this] { showAboutBox(); };
            addAndMakeVisible(&aboutButton);

            centreWithSize(getWidth(), getHeight());
            setVisible(true);
        }

        void resized() override
        {
            DocumentWindow::resized();
            const int tbH = getTitleBarHeight();
            const int pad = 2;
#if JUCE_MAC
            const int tbW = getWidth();
            aboutButton.setBounds(tbW - tbH - pad, pad, tbH - pad * 2, tbH - pad * 2);
#else
            aboutButton.setBounds(pad, pad, tbH - pad * 2, tbH - pad * 2);
#endif
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        juce::TextButton aboutButton { "i" };

        void showAboutBox()
        {
            juce::DialogWindow::LaunchOptions opts;
            opts.content.setOwned(new AboutComponent());
            opts.dialogTitle                  = "About";
            opts.dialogBackgroundColour       = juce::Colour(0xFF141414);
            opts.escapeKeyTriggersCloseButton = true;
            opts.useNativeTitleBar            = false;
            opts.resizable                    = false;
            opts.launchAsync();
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(MolluskVoiceChangerApplication)
