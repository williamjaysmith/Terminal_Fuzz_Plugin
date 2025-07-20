#include "PluginParameters.h"

namespace TerminalFuzz {

juce::AudioProcessorValueTreeState::ParameterLayout PluginParameters::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Front Panel Controls (user interface)
    params.push_back(createFloatParameter(
        INPUT_GAIN_ID, "Input Gain", -20.0f, 100.0f, 0.0f, " dB"
    ));
    
    params.push_back(createFloatParameter(
        FUZZ_ID, "Fuzz", 0.0f, 100.0f, 50.0f, "%"
    ));
    
    params.push_back(createFloatParameter(
        VOICE_ID, "Voice", 0.0f, 100.0f, 50.0f, "%"
    ));
    
    params.push_back(createFloatParameter(
        TREBLE_ID, "Treble", 0.0f, 100.0f, 50.0f, "%"
    ));
    
    // Level parameter removed - hardcoded to 100% in Fuzz Module

    // Back Panel Controls - Resistors (in kilohms for readability)
    params.push_back(createFloatParameter(
        R1_ID, "R1 Input Bias", 1000.0f, 4700.0f, 2200.0f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R2_ID, "R2", 10.0f, 47.0f, 22.0f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R3_ID, "R3 Coupling", 470.0f, 2200.0f, 1000.0f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R4_ID, "R4", 22.0f, 100.0f, 47.0f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R5_ID, "R5 Voice", 4.7f, 22.0f, 10.0f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R6_ID, "R6 Voice", 6.8f, 33.0f, 15.0f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R7_ID, "R7 Q1 Output", 22.0f, 100.0f, 47.0f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R8_ID, "R8 Q1 Bias", 220.0f, 1000.0f, 470.0f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R9_ID, "R9 Q1 Emitter", 220.0f, 1000.0f, 470.0f, " Ω"
    ));
    
    params.push_back(createFloatParameter(
        R10_ID, "R10 Q1 Collector", 4.7f, 22.0f, 10.0f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R11_ID, "R11", 47.0f, 220.0f, 100.0f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R12_ID, "R12 Power Limit", 2.2f, 10.0f, 4.7f, " kΩ"
    ));
    
    params.push_back(createFloatParameter(
        R13_ID, "R13 Input Ground", 470.0f, 2200.0f, 1000.0f, " kΩ"
    ));

    // Back Panel Controls - Capacitors (in nanofarads/microfarads for readability)
    params.push_back(createFloatParameter(
        C1_ID, "C1 Input Coupling", 47.0f, 220.0f, 100.0f, " nF"
    ));
    
    params.push_back(createFloatParameter(
        C2_ID, "C2", 0.47f, 2.2f, 1.0f, " nF"
    ));
    
    params.push_back(createFloatParameter(
        C3_ID, "C3 Coupling (Critical)", 4.7f, 100.0f, 47.0f, " nF"
    ));
    
    params.push_back(createFloatParameter(
        C4_ID, "C4", 1.0f, 4.7f, 2.2f, " nF"
    ));
    
    params.push_back(createFloatParameter(
        C5_ID, "C5 Treble", 1.5f, 6.8f, 3.3f, " nF"
    ));
    
    params.push_back(createFloatParameter(
        C6_ID, "C6 Voice", 0.47f, 2.2f, 1.0f, " nF"
    ));
    
    params.push_back(createFloatParameter(
        C7_ID, "C7", 47.0f, 220.0f, 100.0f, " nF"
    ));
    
    params.push_back(createFloatParameter(
        C8_ID, "C8 Output Coupling", 4.7f, 22.0f, 10.0f, " μF"
    ));
    
    params.push_back(createFloatParameter(
        C9_ID, "C9 Output Coupling", 4.7f, 22.0f, 10.0f, " μF"
    ));
    
    params.push_back(createFloatParameter(
        C10_ID, "C10 Power Filter", 47.0f, 220.0f, 100.0f, " μF"
    ));

    // Transistor parameters
    params.push_back(createFloatParameter(
        Q1_GAIN_ID, "Q1 2N3904 Gain", 0.5f, 3.0f, 1.0f, "x"
    ));
    
    params.push_back(createFloatParameter(
        Q1_BIAS_ID, "Q1 2N3904 Bias", 0.3f, 1.2f, 0.7f, "V"
    ));
    
    params.push_back(createFloatParameter(
        Q2_GAIN_ID, "Q2 2N2369 Gain", 5.0f, 100.0f, 25.0f, "x"
    ));
    
    params.push_back(createFloatParameter(
        Q2_BIAS_ID, "Q2 2N2369 Bias", 0.3f, 1.2f, 0.6f, "V"
    ));
    
    params.push_back(createFloatParameter(
        Q3_GAIN_ID, "Q3 2N2369 Gain", 3.0f, 50.0f, 15.0f, "x"
    ));
    
    params.push_back(createFloatParameter(
        Q3_BIAS_ID, "Q3 2N2369 Bias", 0.3f, 1.2f, 0.6f, "V"
    ));
    
    // Manual Gain Controls (+/-100dB)
    params.push_back(createFloatParameter(
        Q1_MANUAL_GAIN_ID, "Q1 Manual Gain", -100.0f, 100.0f, 0.0f, " dB"
    ));
    
    params.push_back(createFloatParameter(
        Q2_MANUAL_GAIN_ID, "Q2 Manual Gain", -100.0f, 100.0f, 0.0f, " dB"
    ));
    
    params.push_back(createFloatParameter(
        Q3_MANUAL_GAIN_ID, "Q3 Manual Gain", -100.0f, 100.0f, 0.0f, " dB"
    ));
    
    // Transistor Bypass Controls (for debugging)
    params.push_back(createBoolParameter(
        Q1_BYPASS_ID, "Q1 Bypass", false
    ));
    
    params.push_back(createBoolParameter(
        Q2_BYPASS_ID, "Q2 Bypass", false
    ));
    
    params.push_back(createBoolParameter(
        Q3_BYPASS_ID, "Q3 Bypass", false
    ));

    // Diode parameters
    params.push_back(createFloatParameter(
        D1_THRESHOLD_ID, "D1 1N5817 Threshold", 0.15f, 0.45f, 0.25f, "V"
    ));
    
    params.push_back(createFloatParameter(
        D2_BRIGHTNESS_ID, "D2 LED Brightness", 0.0f, 1.0f, 0.8f, ""
    ));

    // Switch parameters
    params.push_back(createBoolParameter(
        BYPASS_MODE_ID, "3PDT Bypass", false
    ));

    // Panel mode switch (front vs back panel)
    params.push_back(createBoolParameter(
        PANEL_MODE_ID, "Panel Mode", false  // false = front, true = back
    ));

    // Post-processing lowpass filter (1kHz to 10kHz, default 2kHz)
    params.push_back(createFloatParameter(
        LOWPASS_FREQ_ID, "Lowpass Filter", 1000.0f, 10000.0f, 2000.0f, " Hz"
    ));

    // Main bypass switch (default false = pedal engaged)
    params.push_back(createBoolParameter(
        MAIN_BYPASS_ID, "Main Bypass", false
    ));

    // Physics Control Parameters (wide ranges for extreme tuning)
    
    // SIMPLIFIED PHYSICS MASTERS - Custom ranges for optimal effects
    params.push_back(createFloatParameter(
        PARASITIC_MASTER_ID, "Parasitic Master", 1.0f, 50.0f, 25.0f, "x"
    ));
    params.push_back(createFloatParameter(
        THERMAL_MASTER_ID, "Thermal Master", 1.0f, 100.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        BIAS_MASTER_ID, "Bias Master", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        SATURATION_MASTER_ID, "Saturation Master", 0.0f, 100.0f, 1.0f, "x"
    ));
    
    // Keep detail parameters for compatibility but hide them
    params.push_back(createFloatParameter(
        CBE_LOADING_ID, "Cbe Loading", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        MILLER_FEEDBACK_ID, "Miller Feedback", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        HF_INSTABILITY_ID, "HF Instability", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        JUNCTION_HEAT_ID, "Junction Heat", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        Q2Q3_COUPLING_ID, "Q2↔Q3 Coupling", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        THERMAL_SUSTAIN_ID, "Thermal Sustain", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        DRIFT_RATE_ID, "Drift Rate", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        DRIFT_DEPTH_ID, "Drift Depth", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        SUPPLY_SAG_ID, "Supply Sag", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        VBE_CURVE_ID, "VBE Curve", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        INTERMOD_ID, "Intermod", -50.0f, 50.0f, 1.0f, "x"
    ));
    params.push_back(createFloatParameter(
        ASYMMETRY_ID, "Asymmetry", -50.0f, 50.0f, 1.0f, "x"
    ));
    
    // TONE STACK BYPASS
    params.push_back(createBoolParameter(
        TONE_BYPASS_ID, "Tone Bypass", false
    ));
    
    // HARMONIC GENERATION TOGGLE
    params.push_back(createBoolParameter(
        HARMONICS_ENABLED_ID, "Harmonics", true
    ));
    
    // HARMONIC FILTERING TOGGLE
    params.push_back(createBoolParameter(
        HARMONIC_FILTERING_ID, "Harmonic Filtering", true
    ));
    
    // NEW EFFECT TOGGLES
    params.push_back(createBoolParameter(
        BIT_CRUSHING_ID, "Bit Crushing", false
    ));
    
    params.push_back(createBoolParameter(
        FREQUENCY_DOUBLING_ID, "Frequency Doubling", false
    ));
    
    params.push_back(createBoolParameter(
        CONTROLLED_GATING_ID, "Controlled Gating", false
    ));
    
    params.push_back(createBoolParameter(
        ADVANCED_COMPRESSION_ID, "Advanced Compression", false
    ));
    
    params.push_back(createBoolParameter(
        BIAS_DRIFT_ID, "Bias Drift", false
    ));
    
    params.push_back(createBoolParameter(
        ENVELOPE_BIAS_ID, "Envelope Bias", false
    ));
    
    // REMOVED: All anti-gating controls - user found them ineffective

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

// Front panel parameter getters
float PluginParameters::getInputGain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(INPUT_GAIN_ID)->load();
}

float PluginParameters::getFuzz(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(FUZZ_ID)->load() / 100.0f;
}

float PluginParameters::getVoice(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(VOICE_ID)->load() / 100.0f;
}

float PluginParameters::getTreble(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(TREBLE_ID)->load() / 100.0f;
}

// Level parameter removed - hardcoded to 100% in Fuzz Module

// Back panel component getters (convert to base units: ohms and farads)
float PluginParameters::getR1(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R1_ID)->load() * 1000.0f;  // kΩ to Ω
}

float PluginParameters::getR2(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R2_ID)->load() * 1000.0f;
}

float PluginParameters::getR3(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R3_ID)->load() * 1000.0f;
}

float PluginParameters::getR4(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R4_ID)->load() * 1000.0f;
}

float PluginParameters::getR5(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R5_ID)->load() * 1000.0f;
}

float PluginParameters::getR6(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R6_ID)->load() * 1000.0f;
}

float PluginParameters::getR7(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R7_ID)->load() * 1000.0f;
}

float PluginParameters::getR8(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R8_ID)->load() * 1000.0f;
}

float PluginParameters::getR9(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R9_ID)->load();  // Already in ohms
}

float PluginParameters::getR10(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R10_ID)->load() * 1000.0f;
}

float PluginParameters::getR11(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R11_ID)->load() * 1000.0f;
}

float PluginParameters::getR12(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R12_ID)->load() * 1000.0f;
}

float PluginParameters::getR13(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(R13_ID)->load() * 1000.0f;
}

float PluginParameters::getC1(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(C1_ID)->load() * 1e-9f;  // nF to F
}

float PluginParameters::getC2(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(C2_ID)->load() * 1e-9f;
}

float PluginParameters::getC3(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(C3_ID)->load() * 1e-9f;
}

float PluginParameters::getC4(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(C4_ID)->load() * 1e-9f;
}

float PluginParameters::getC5(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(C5_ID)->load() * 1e-9f;
}

float PluginParameters::getC6(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(C6_ID)->load() * 1e-9f;
}

float PluginParameters::getC7(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(C7_ID)->load() * 1e-9f;
}

float PluginParameters::getC8(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(C8_ID)->load() * 1e-6f;  // μF to F
}

float PluginParameters::getC9(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(C9_ID)->load() * 1e-6f;
}

float PluginParameters::getC10(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(C10_ID)->load() * 1e-6f;
}

// Transistor parameter getters
float PluginParameters::getQ1Gain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q1_GAIN_ID)->load();
}

float PluginParameters::getQ1Bias(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q1_BIAS_ID)->load();
}

float PluginParameters::getQ2Gain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q2_GAIN_ID)->load();
}

float PluginParameters::getQ2Bias(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q2_BIAS_ID)->load();
}

float PluginParameters::getQ3Gain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q3_GAIN_ID)->load();
}

float PluginParameters::getQ3Bias(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q3_BIAS_ID)->load();
}

// Manual gain control getters
float PluginParameters::getQ1ManualGain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q1_MANUAL_GAIN_ID)->load();
}

float PluginParameters::getQ2ManualGain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q2_MANUAL_GAIN_ID)->load();
}

float PluginParameters::getQ3ManualGain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q3_MANUAL_GAIN_ID)->load();
}

// Transistor bypass getters
bool PluginParameters::getQ1Bypass(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q1_BYPASS_ID)->load() > 0.5f;
}

bool PluginParameters::getQ2Bypass(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q2_BYPASS_ID)->load() > 0.5f;
}

bool PluginParameters::getQ3Bypass(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q3_BYPASS_ID)->load() > 0.5f;
}

// Diode parameter getters
float PluginParameters::getD1Threshold(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(D1_THRESHOLD_ID)->load();
}

float PluginParameters::getD2Brightness(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(D2_BRIGHTNESS_ID)->load();
}

// Switch parameter getters
bool PluginParameters::getBypassMode(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(BYPASS_MODE_ID)->load() > 0.5f;
}

bool PluginParameters::getPanelMode(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(PANEL_MODE_ID)->load() > 0.5f;
}

float PluginParameters::getLowpassFreq(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(LOWPASS_FREQ_ID)->load();
}

bool PluginParameters::getMainBypass(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(MAIN_BYPASS_ID)->load() > 0.5f;
}

// Physics parameter getters
float PluginParameters::getParasiticMaster(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(PARASITIC_MASTER_ID)->load();
}

float PluginParameters::getCbeLoading(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(CBE_LOADING_ID)->load();
}

float PluginParameters::getMillerFeedback(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(MILLER_FEEDBACK_ID)->load();
}

float PluginParameters::getHfInstability(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(HF_INSTABILITY_ID)->load();
}

float PluginParameters::getThermalMaster(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(THERMAL_MASTER_ID)->load();
}

float PluginParameters::getJunctionHeat(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(JUNCTION_HEAT_ID)->load();
}

float PluginParameters::getQ2Q3Coupling(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(Q2Q3_COUPLING_ID)->load();
}

float PluginParameters::getThermalSustain(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(THERMAL_SUSTAIN_ID)->load();
}

float PluginParameters::getBiasMaster(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(BIAS_MASTER_ID)->load();
}

float PluginParameters::getDriftRate(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(DRIFT_RATE_ID)->load();
}

float PluginParameters::getDriftDepth(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(DRIFT_DEPTH_ID)->load();
}

float PluginParameters::getSupplySag(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(SUPPLY_SAG_ID)->load();
}

float PluginParameters::getSaturationMaster(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(SATURATION_MASTER_ID)->load();
}

float PluginParameters::getVbeCurve(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(VBE_CURVE_ID)->load();
}

float PluginParameters::getIntermod(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(INTERMOD_ID)->load();
}

float PluginParameters::getAsymmetry(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(ASYMMETRY_ID)->load();
}

bool PluginParameters::getToneBypass(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(TONE_BYPASS_ID)->load() > 0.5f;
}

bool PluginParameters::getHarmonicsEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(HARMONICS_ENABLED_ID)->load() > 0.5f;
}

bool PluginParameters::getHarmonicFiltering(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(HARMONIC_FILTERING_ID)->load() > 0.5f;
}

bool PluginParameters::getBitCrushingEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(BIT_CRUSHING_ID)->load() > 0.5f;
}

bool PluginParameters::getFrequencyDoublingEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(FREQUENCY_DOUBLING_ID)->load() > 0.5f;
}

bool PluginParameters::getControlledGatingEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(CONTROLLED_GATING_ID)->load() > 0.5f;
}

bool PluginParameters::getAdvancedCompressionEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(ADVANCED_COMPRESSION_ID)->load() > 0.5f;
}

bool PluginParameters::getBiasDriftEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(BIAS_DRIFT_ID)->load() > 0.5f;
}

bool PluginParameters::getEnvelopeBiasEnabled(const juce::AudioProcessorValueTreeState& apvts) {
    return apvts.getRawParameterValue(ENVELOPE_BIAS_ID)->load() > 0.5f;
}

// REMOVED: All anti-gating parameter getters - user found them ineffective

}  // namespace TerminalFuzz