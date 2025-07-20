#!/bin/bash
echo "🔨 Building Terminal Fuzz..."
cd "$(dirname "$0")/build"
make -j4

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    
    # Auto-reload standalone if it's running
    if pgrep -f "Terminal Fuzz.app" > /dev/null; then
        echo "🔄 Hot reloading standalone..."
        killall "Terminal Fuzz" 2>/dev/null
        sleep 0.5
        open "TerminalFuzzPlugin_artefacts/Standalone/Terminal Fuzz.app"
        echo "🎸 Standalone reloaded!"
    # Check if Logic Pro is running
    elif pgrep -x "Logic Pro" > /dev/null; then
        echo "🎵 Logic Pro detected!"
        echo "💡 Your plugin is auto-updated! Just:"
        echo "   1. Go to your Logic project"
        echo "   2. Remove and re-add Terminal Fuzz plugin"
        echo "   3. Or restart playback to refresh"
        echo ""
        echo "🚀 No need to restart Logic - much faster testing!"
    else
        echo "🎵 No apps running. Options:"
        echo "   1. Launch standalone (hot reload enabled)"
        echo "   2. Open Logic Pro (recommended for mixing)"
        echo ""
        read -p "Launch standalone? (y/n): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            echo "🚀 Launching standalone..."
            open "TerminalFuzzPlugin_artefacts/Standalone/Terminal Fuzz.app"
            echo "🎸 Standalone launched! (will hot reload on next build)"
        else
            echo "💡 Run this script again after each build for hot reload"
        fi
    fi
else
    echo "❌ Build failed!"
fi