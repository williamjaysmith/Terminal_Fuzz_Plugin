#!/bin/bash

# Build script for Terminal Fuzz Plugin
echo "Building Terminal Fuzz Plugin..."

cd /Users/williamsmith/Desktop/Terminal_Fuzz_Plugin

# Create build directory
mkdir -p build_terminal
cd build_terminal

# Configure with cmake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the plugin
make -j4

echo "Build complete! Plugin files should be in:"
echo "AU: ~/Library/Audio/Plug-Ins/Components/Terminal Fuzz.component"
echo "VST3: ~/Library/Audio/Plug-Ins/VST3/Terminal Fuzz.vst3"
echo "Standalone: ./Terminal Fuzz_artefacts/Terminal Fuzz.app"