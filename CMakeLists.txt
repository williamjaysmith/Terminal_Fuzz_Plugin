cmake_minimum_required(VERSION 3.22)

project(TerminalFuzzPlugin VERSION 2.4.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download JUCE automatically if not present
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE/CMakeLists.txt")
    message(STATUS "JUCE not found, downloading...")
    include(FetchContent)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG        8.0.3
    )
    FetchContent_MakeAvailable(JUCE)
else()
    add_subdirectory(external/JUCE)
endif()

# Define the plugin target
juce_add_plugin(TerminalFuzzPlugin
    COMPANY_NAME "Terminal Audio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Term
    PLUGIN_CODE TFuz
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "Terminal Fuzz"
)

# Generate JuceHeader.h
juce_generate_juce_header(TerminalFuzzPlugin)

# Add source files
target_sources(TerminalFuzzPlugin
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginProcessor.h
        src/PluginEditor.cpp
        src/PluginEditor.h
        src/TerminalCircuit.cpp
        src/TerminalCircuit.h
        src/FrontPanelComponent.cpp
        src/FrontPanelComponent.h
        src/PhysicsPanelComponent.cpp
        src/PhysicsPanelComponent.h
        src/PluginParameters.cpp
        src/PluginParameters.h
        src/SteelBackgroundComponent.cpp
        src/SteelBackgroundComponent.h
        src/Common.h
)

# Link libraries
target_link_libraries(TerminalFuzzPlugin
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Compile definitions
target_compile_definitions(TerminalFuzzPlugin
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_DISABLE_JUCE_VERSION_PRINTING=1
)

# Optional test executable (disabled for now)
# add_executable(TerminalFuzzTests
#     test_main.cpp
#     src/TerminalCircuitTests.cpp
#     src/TerminalCircuit.cpp
#     src/TerminalCircuit.h
#     src/TerminalFuzzParameters.cpp
#     src/TerminalFuzzParameters.h
#     src/Common.h
# )

# target_link_libraries(TerminalFuzzTests
#     PRIVATE
#         juce::juce_audio_utils
#         juce::juce_audio_processors
#         juce::juce_dsp
#     PUBLIC
#         juce::juce_recommended_config_flags
#         juce::juce_recommended_lto_flags
#         juce::juce_recommended_warning_flags
# )

# target_compile_definitions(TerminalFuzzTests
#     PUBLIC
#         JUCE_WEB_BROWSER=0
#         JUCE_USE_CURL=0
#         JUCE_STRICT_REFCOUNTEDPOINTER=1
#         JUCE_UNIT_TESTS=1
# )