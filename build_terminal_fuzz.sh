#!/bin/bash

# Terminal Fuzz Plugin Build Script
# Run this from Terminal to build the plugin

echo "=========================================="
echo "Building Terminal Fuzz Plugin"
echo "=========================================="

# Navigate to the plugin directory
cd /Users/williamsmith/Desktop/Terminal_Fuzz_Plugin

# Check if directory exists
if [ ! -d "/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin" ]; then
    echo "ERROR: Terminal_Fuzz_Plugin directory not found!"
    exit 1
fi

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring build with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Check if CMake succeeded
if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed!"
    exit 1
fi

# Build the plugin
echo "Building plugin..."
make -j4

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo "=========================================="
    echo "BUILD SUCCESSFUL!"
    echo "=========================================="
    echo ""
    echo "Plugin locations:"
    echo "AU (Logic Pro): ~/Library/Audio/Plug-Ins/Components/Terminal Fuzz.component"
    echo "VST3: ~/Library/Audio/Plug-Ins/VST3/Terminal Fuzz.vst3"
    echo "Standalone: ./Terminal Fuzz_artefacts/Terminal Fuzz.app"
    echo ""
    echo "To use in Logic Pro:"
    echo "1. Restart Logic Pro"
    echo "2. Insert plugin: Audio FX → Distortion → Terminal Fuzz"
    echo "3. Toggle between Front/Back panels for standard or component-level controls"
else
    echo "ERROR: Build failed!"
    exit 1
fi