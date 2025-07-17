# Terminal Fuzz Plugin Build Instructions

## Prerequisites

- macOS with Xcode Command Line Tools installed
- CMake 3.22 or higher
- JUCE framework (will be downloaded automatically)

## Build Steps

1. **Open Terminal and navigate to the Terminal Fuzz Plugin directory:**

   ```bash
   cd /Users/williamsmith/Desktop/Terminal_Fuzz_Plugin
   ```

2. **Create and enter build directory:**

   ```bash
   mkdir -p build
   cd build
   ```

3. **Configure the build with CMake:**

   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Release
   ```

4. **Build the plugin:**
   ```bash
   make -j4
   ```

## Expected Output Locations

After successful build, the plugin will be automatically copied to:
macyodoev

- **AU Plugin** (for Logic Pro): `~/Library/Audio/Plug-Ins/Components/Terminal Fuzz.component`
- **VST3 Plugin**: `~/Library/Audio/Plug-Ins/VST3/Terminal Fuzz.vst3`
- **Standalone App**: `./Terminal Fuzz_artefacts/Terminal Fuzz.app`

## Using in Logic Pro

1. **Restart Logic Pro** after building to refresh the plugin cache
2. **Create a new audio track** or select an existing one
3. **Add the plugin**: Click the Audio FX slot → Distortion → Terminal Fuzz
4. **Two Interface Modes**:
   - **Front Panel**: Standard fuzz pedal controls (Fuzz, Voice, Treble, Level)
   - **Back Panel**: Component-level circuit tweaking (click "Show Component Tweaks")

## Troubleshooting

If build fails:

- Ensure Xcode Command Line Tools are installed: `xcode-select --install`
- Check CMake version: `cmake --version` (should be 3.22+)
- Clean build: `rm -rf build && mkdir build && cd build && cmake .. && make -j4`

## Plugin Features

- **Faithful Terminal Fuzz Recreation**: Component-by-component modeling
- **Front Panel**: Standard pedal interface
- **Back Panel**: All 13 resistors and 10 capacitors individually adjustable
- **Circuit Analysis**: Detailed explanations of each component's function
- **Real-time Processing**: Low-latency audio processing for live use
