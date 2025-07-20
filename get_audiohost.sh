#!/bin/bash
echo "🎯 Getting JUCE AudioPluginHost..."

# Check if AudioPluginHost is already available system-wide
if command -v AudioPluginHost >/dev/null 2>&1; then
    echo "✅ AudioPluginHost already available!"
    which AudioPluginHost
    exit 0
fi

# Download pre-built AudioPluginHost from JUCE
echo "📥 Downloading pre-built AudioPluginHost..."
AUDIOHOST_URL="https://github.com/juce-framework/JUCE/releases/download/8.0.3/AudioPluginHost-macOS.zip"

if command -v curl >/dev/null 2>&1; then
    curl -L -o AudioPluginHost.zip "$AUDIOHOST_URL" 2>/dev/null || {
        echo "❌ Download failed. Building from source..."
        
        # Try building with Xcode command line tools
        echo "🔨 Building AudioPluginHost from JUCE source..."
        cd external/JUCE/extras/AudioPluginHost/Builds/MacOSX
        
        # Use make instead of xcodebuild
        if [ -f Makefile ]; then
            make
        else
            echo "❌ Cannot build AudioPluginHost automatically."
            echo ""
            echo "🎯 SIMPLE SOLUTION: Use Logic Pro with faster workflow:"
            echo "   1. Keep Logic Pro open"
            echo "   2. Run: ./fast_test.sh"
            echo "   3. In Logic: Plugin → Audio Units → Terminal Audio → Terminal Fuzz"
            echo "   4. Plugin auto-reloads when rebuilt!"
            echo ""
            echo "Or try the standalone app with these audio tips:"
            echo "   - Audio Settings → Input Monitoring ON"
            echo "   - Check Plugin Input routing"
        fi
    }
else
    echo "❌ curl not found. Cannot download."
fi