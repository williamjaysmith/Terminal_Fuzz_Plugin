#pragma once

#include <JuceHeader.h>

namespace TerminalFuzz {

// Build Version
#define TERMINAL_FUZZ_BUILD_VERSION "v3.99-CLEAN-REVERT"
#define BUILD_INFO_STRING "Terminal Fuzz " TERMINAL_FUZZ_BUILD_VERSION " - Build Date: " __DATE__ " " __TIME__

namespace Constants {

// Plugin Information
static constexpr const char* PLUGIN_NAME = "Terminal Fuzz";
static constexpr const char* PLUGIN_VERSION = "1.0.0";
static constexpr const char* COMPANY_NAME = "Terminal Audio";

// Audio Processing
static constexpr int MAX_CHANNELS = 2;
static constexpr double DEFAULT_SAMPLE_RATE = 44100.0;
static constexpr int DEFAULT_BLOCK_SIZE = 512;

// DSP Constants
static constexpr float PI = 3.14159265359f;
static constexpr float TWO_PI = 2.0f * PI;

// GUI Dimensions
static constexpr int PLUGIN_WIDTH = 800;
static constexpr int PLUGIN_HEIGHT = 600;

// Component Sizes
static constexpr int KNOB_SIZE = 80;
static constexpr int KNOB_MARGIN = 20;

// Circuit Constants (from schematic)
static constexpr float SUPPLY_VOLTAGE = 9.0f;  // 9V supply
static constexpr float THERMAL_VOLTAGE = 0.026f;  // kT/q at room temp

}  // namespace Constants
}  // namespace TerminalFuzz