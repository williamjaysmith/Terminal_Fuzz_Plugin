#include "PluginParameters.h"

namespace TerminalFuzz {

juce::AudioProcessorValueTreeState::ParameterLayout PluginParameters::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // ===== MAIN PEDAL CONTROLS (only these appear in Logic automation) =====
    
    // Input Gain
    params.push_back(createFloatParameter(
        INPUT_GAIN_ID, "Input Gain", -20.0f, 100.0f, 0.0f, " dB"
    ));
    
    // Fuzz Amount
    params.push_back(createFloatParameter(
        FUZZ_ID, "Fuzz", 0.0f, 100.0f, 50.0f, "%"
    ));
    
    // Q1 Output Buffer Control
    params.push_back(createFloatParameter(
        Q1_MANUAL_GAIN_ID, "Q1 Output Buffer", -100.0f, 100.0f, 0.0f, " dB"
    ));
    
    // Q2 Main Fuzz Control
    params.push_back(createFloatParameter(
        Q2_MANUAL_GAIN_ID, "Q2 Main Fuzz", -100.0f, 100.0f, 0.0f, " dB"
    ));
    
    // Q3 Clean Gain Control
    params.push_back(createFloatParameter(
        Q3_MANUAL_GAIN_ID, "Q3 Clean Gain", -100.0f, 100.0f, 0.0f, " dB"
    ));
    
    // Lowpass Filter
    params.push_back(createFloatParameter(
        LOWPASS_FREQ_ID, "Lowpass Filter", 1000.0f, 10000.0f, 2000.0f, " Hz"
    ));
    
    // Output Volume Control
    params.push_back(createFloatParameter(
        OUTPUT_VOLUME_ID, "Output Volume", -100.0f, 100.0f, 0.0f, " dB"
    ));
    
    // Main Bypass
    params.push_back(createBoolParameter(
        MAIN_BYPASS_ID, "Bypass", false
    ));

    // All other parameters now return hardcoded defaults (not exposed to Logic)
    return {params.begin(), params.end()};
}

std::unique_ptr<juce::RangedAudioParameter> PluginParameters::createFloatParameter(
    const juce::String& parameterID,
    const juce::String& parameterName,
    float minValue,
    float maxValue,
    float defaultValue,
    const juce::String& suffix
) {
    auto range = juce::NormalisableRange<float>(minValue, maxValue);
    
    return std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{parameterID, 1},
        parameterName,
        range,
        defaultValue,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([suffix](float value, int) {
                return juce::String(value, 1) + suffix;
            })
    );
}

std::unique_ptr<juce::RangedAudioParameter> PluginParameters::createBoolParameter(
    const juce::String& parameterID,
    const juce::String& parameterName,
    bool defaultValue
) {
    return std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{parameterID, 1},
        parameterName,
        defaultValue,
        juce::AudioParameterBoolAttributes()
            .withStringFromValueFunction([](bool value, int) {
                return value ? "Back Panel" : "Front Panel";
            })
    );
}

// Main pedal control getters (exposed to Logic)
float PluginParameters::getInputGain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(INPUT_GAIN_ID)->load();
}

float PluginParameters::getFuzz(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(FUZZ_ID)->load() / 100.0f;
}

float PluginParameters::getOutputVolume(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(OUTPUT_VOLUME_ID)->load();
}

// Manual gain control getters (exposed to Logic)
float PluginParameters::getQ1ManualGain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q1_MANUAL_GAIN_ID)->load();
}

float PluginParameters::getQ2ManualGain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q2_MANUAL_GAIN_ID)->load();
}

float PluginParameters::getQ3ManualGain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q3_MANUAL_GAIN_ID)->load();
}

float PluginParameters::getLowpassFreq(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(LOWPASS_FREQ_ID)->load();
}

bool PluginParameters::getMainBypass(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(MAIN_BYPASS_ID)->load() > 0.5f;
}

// Stub getters - return hardcoded defaults (not exposed to Logic)
float PluginParameters::getVoice(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 0.5f; // Default - tone stack removed
}

float PluginParameters::getTreble(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 0.5f; // Default - tone stack removed
}

// All remaining getters return hardcoded defaults (circuit values abstracted away)

// Resistor values (in ohms)
float PluginParameters::getR1(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 2200000.0f; // 2.2MΩ
}

float PluginParameters::getR2(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 22000.0f; // 22kΩ
}

float PluginParameters::getR3(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1000000.0f; // 1MΩ
}

float PluginParameters::getR4(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 47000.0f; // 47kΩ
}

float PluginParameters::getR5(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 10000.0f; // 10kΩ
}

float PluginParameters::getR6(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 15000.0f; // 15kΩ
}

float PluginParameters::getR7(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 47000.0f; // 47kΩ
}

float PluginParameters::getR8(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 470000.0f; // 470kΩ
}

float PluginParameters::getR9(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 470.0f; // 470Ω
}

float PluginParameters::getR10(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 10000.0f; // 10kΩ
}

float PluginParameters::getR11(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 100000.0f; // 100kΩ
}

float PluginParameters::getR12(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 4700.0f; // 4.7kΩ
}

float PluginParameters::getR13(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1000000.0f; // 1MΩ
}

// Capacitor values (in farads)
float PluginParameters::getC1(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 100e-9f; // 100nF
}

float PluginParameters::getC2(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1e-9f; // 1nF
}

float PluginParameters::getC3(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 47e-9f; // 47nF
}

float PluginParameters::getC4(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 2.2e-9f; // 2.2nF
}

float PluginParameters::getC5(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 3.3e-9f; // 3.3nF
}

float PluginParameters::getC6(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1e-9f; // 1nF
}

float PluginParameters::getC7(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 100e-9f; // 100nF
}

float PluginParameters::getC8(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 10e-6f; // 10μF
}

float PluginParameters::getC9(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 10e-6f; // 10μF
}

float PluginParameters::getC10(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 100e-6f; // 100μF
}

// Transistor parameters
float PluginParameters::getQ1Gain(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getQ1Bias(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 0.7f;
}

float PluginParameters::getQ2Gain(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 25.0f;
}

float PluginParameters::getQ2Bias(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 0.6f;
}

float PluginParameters::getQ3Gain(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 15.0f;
}

float PluginParameters::getQ3Bias(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 0.6f;
}

// Transistor bypass controls
bool PluginParameters::getQ1Bypass(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

bool PluginParameters::getQ2Bypass(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

bool PluginParameters::getQ3Bypass(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

// Diode parameters
float PluginParameters::getD1Threshold(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 0.25f;
}

float PluginParameters::getD2Brightness(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 0.8f;
}

// Switch parameters
bool PluginParameters::getBypassMode(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

bool PluginParameters::getPanelMode(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

// Physics parameters
float PluginParameters::getParasiticMaster(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 25.0f;
}

float PluginParameters::getCbeLoading(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getMillerFeedback(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getHfInstability(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getThermalMaster(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getJunctionHeat(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getQ2Q3Coupling(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getThermalSustain(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getBiasMaster(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getDriftRate(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getDriftDepth(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getSupplySag(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getSaturationMaster(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getVbeCurve(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getIntermod(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

float PluginParameters::getAsymmetry(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return 1.0f;
}

bool PluginParameters::getToneBypass(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

bool PluginParameters::getHarmonicsEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return true;
}

bool PluginParameters::getHarmonicFiltering(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return true;
}

bool PluginParameters::getBitCrushingEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

bool PluginParameters::getFrequencyDoublingEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

bool PluginParameters::getControlledGatingEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

bool PluginParameters::getAdvancedCompressionEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

bool PluginParameters::getBiasDriftEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

bool PluginParameters::getEnvelopeBiasEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    juce::ignoreUnused(apvts);
    return false;
}

// REMOVED: All anti-gating parameter getters - user found them ineffective

}  // namespace TerminalFuzz