#!/bin/bash
echo "🔨 Building AudioPluginHost..."

# Try to build AudioPluginHost using our existing JUCE setup
cd external/JUCE/extras/AudioPluginHost

# Create a simple CMakeLists that uses our JUCE
cat > CMakeLists_simple.txt << 'EOF'
cmake_minimum_required(VERSION 3.22)
project(AudioPluginHost)

# Use the JUCE from parent
find_package(PkgConfig REQUIRED)
add_subdirectory(../../../external/JUCE JUCE)

juce_add_gui_app(AudioPluginHost
    PRODUCT_NAME "AudioPluginHost"
    COMPANY_NAME "JUCE"
    VERSION "1.0.0"
)

target_sources(AudioPluginHost PRIVATE
    Source/HostStartup.cpp
    Source/Plugins/PluginGraph.cpp
    Source/Plugins/InternalPlugins.cpp
    Source/UI/MainHostWindow.cpp
    Source/UI/GraphEditorPanel.cpp
)

target_link_libraries(AudioPluginHost PRIVATE
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)
EOF

echo "❌ JUCE AudioPluginHost build is complex. Let's use a simpler solution..."
echo ""
echo "🎯 ALTERNATIVE SOLUTION:"
echo "1. Download pre-built AudioPluginHost from JUCE website"
echo "2. Or use Reaper (60-day free trial) - much faster than Logic"
echo "3. Or fix standalone audio routing"
echo ""
echo "Which would you prefer?"