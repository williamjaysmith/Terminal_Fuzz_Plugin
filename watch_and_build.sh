#!/bin/bash
echo "🔍 Watching for file changes in src/ directory..."
echo "Edit any .cpp or .h file and it will auto-rebuild!"
echo "Press Ctrl+C to stop watching"
echo ""

# Check if fswatch is installed
if ! command -v fswatch &> /dev/null; then
    echo "⚠️  fswatch not found. Installing with Homebrew..."
    if command -v brew &> /dev/null; then
        brew install fswatch
    else
        echo "❌ Homebrew not found. Please install fswatch manually:"
        echo "   brew install fswatch"
        exit 1
    fi
fi

# Watch src directory for changes and trigger rebuild
fswatch -o src/ | while read; do
    echo ""
    echo "📝 File change detected! Rebuilding..."
    ./fast_test.sh
done