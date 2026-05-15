cmake_minimum_required(VERSION 3.22)
project(MolluskVoiceChanger VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(JUCE)

juce_add_binary_data(MolluskFonts SOURCES
    assets/fonts/Bungee-Regular.ttf
    assets/fonts/RocknRollOne-Regular.ttf
    assets/fonts/AlfaSlabOne-Regular.ttf
    assets/fonts/RubikGlitch-Regular.ttf
    assets/icons/squid2.svg
    assets/icons/octopus.svg
)

juce_add_gui_app(MolluskVoiceChanger
    PRODUCT_NAME "Mollusk Voice Changer"
    BUNDLE_ID    "com.mollusk.voicechanger"
    ICON_BIG     "assets/icons/app_icon.png"
    ICON_SMALL   "assets/icons/app_icon.png"
    # macOS: request microphone permission at launch
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT    "Mollusk Voice Changer needs microphone access for voice processing."
)

juce_generate_juce_header(MolluskVoiceChanger)

target_sources(MolluskVoiceChanger PRIVATE
    src/Main.cpp
    src/MainComponent.cpp
    src/PresetManager.cpp
    src/ui/AboutComponent.cpp
    src/ui/AdvancedPanel.cpp
    src/ui/BasicPanel.cpp
    src/ui/MolluskLookAndFeel.cpp
    src/dsp/HighPassFilter.cpp
    src/dsp/BandPassFilter.cpp
    src/dsp/PhaserProcessor.cpp
    src/dsp/PitchShifter.cpp
    src/dsp/NoiseGate.cpp
    src/dsp/FormantShifter.cpp
    src/dsp/LFOFilter.cpp
    src/dsp/EffectChain.cpp
    src/dsp/OutputGain.cpp
)


# ================================================================
# VST3 Plugin
# ================================================================

juce_add_plugin(MolluskVoiceChangerPlugin
    PRODUCT_NAME              "Mollusk Voice Changer"
    BUNDLE_ID                 "com.mollusk.voicechanger.plugin"
    PLUGIN_MANUFACTURER_CODE  Hyak
    PLUGIN_CODE               Mvc1
    FORMATS                   VST3
    IS_SYNTH                  FALSE
    NEEDS_MIDI_INPUT          FALSE
    NEEDS_MIDI_OUTPUT         FALSE
    IS_MIDI_EFFECT            FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES           "Fx"
    ICON_BIG                  "assets/icons/app_icon.png"
    ICON_SMALL                "assets/icons/app_icon.png"
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT    "Mollusk Voice Changer needs microphone access for voice processing."
)

juce_generate_juce_header(MolluskVoiceChangerPlugin)

target_sources(MolluskVoiceChangerPlugin PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/PresetManager.cpp
    src/ui/AdvancedPanel.cpp
    src/ui/BasicPanel.cpp
    src/ui/MolluskLookAndFeel.cpp
    src/dsp/HighPassFilter.cpp
    src/dsp/BandPassFilter.cpp
    src/dsp/PhaserProcessor.cpp
    src/dsp/PitchShifter.cpp
    src/dsp/NoiseGate.cpp
    src/dsp/FormantShifter.cpp
    src/dsp/LFOFilter.cpp
    src/dsp/EffectChain.cpp
    src/dsp/OutputGain.cpp
)

if(MSVC)
    target_compile_options(MolluskVoiceChangerPlugin PRIVATE /utf-8)
endif()

target_compile_definitions(MolluskVoiceChangerPlugin PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MolluskVoiceChangerPlugin PRIVATE
    MolluskFonts
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_plugin_client
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# ================================================================

if(MSVC)
    target_compile_options(MolluskVoiceChanger PRIVATE /utf-8)
endif()

target_compile_definitions(MolluskVoiceChanger PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:MolluskVoiceChanger,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:MolluskVoiceChanger,JUCE_VERSION>"
)

target_link_libraries(MolluskVoiceChanger PRIVATE
    MolluskFonts
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)
