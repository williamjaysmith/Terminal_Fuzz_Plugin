#pragma once

#include <JuceHeader.h>
#include "Common.h"

namespace TerminalFuzz {

/**
 * @brief Centralized parameter management for the Terminal Fuzz plugin
 * 
 * This class provides a clean interface for managing plugin parameters,
 * including their creation, validation, and type-safe access.
 */
class PluginParameters {
public:
    // Front Panel Parameter IDs (user controls)
    static constexpr const char* INPUT_GAIN_ID = "input_gain";
    static constexpr const char* FUZZ_ID = "fuzz";
    static constexpr const char* VOICE_ID = "voice";
    static constexpr const char* TREBLE_ID = "treble";
    static constexpr const char* LEVEL_ID = "level";
    
    // Back Panel Parameter IDs (circuit component tweaking)
    static constexpr const char* R1_ID = "r1";    // 2.2M input bias
    static constexpr const char* R2_ID = "r2";    // 22k
    static constexpr const char* R3_ID = "r3";    // 1M coupling
    static constexpr const char* R4_ID = "r4";    // 47k
    static constexpr const char* R5_ID = "r5";    // 10k voice
    static constexpr const char* R6_ID = "r6";    // 15k voice
    static constexpr const char* R7_ID = "r7";    // 47k Q1 output
    static constexpr const char* R8_ID = "r8";    // 470k Q1 bias
    static constexpr const char* R9_ID = "r9";    // 470R Q1 emitter
    static constexpr const char* R10_ID = "r10";  // 10k Q1 collector
    static constexpr const char* R11_ID = "r11";  // 100k
    static constexpr const char* R12_ID = "r12";  // 4.7k power limit
    static constexpr const char* R13_ID = "r13";  // 1M input to ground
    
    static constexpr const char* C1_ID = "c1";    // 100n input coupling
    static constexpr const char* C2_ID = "c2";    // 1n
    static constexpr const char* C3_ID = "c3";    // 47n coupling (critical)
    static constexpr const char* C4_ID = "c4";    // 2n2
    static constexpr const char* C5_ID = "c5";    // 3n3 treble
    static constexpr const char* C6_ID = "c6";    // 1n voice
    static constexpr const char* C7_ID = "c7";    // 100n
    static constexpr const char* C8_ID = "c8";    // 10u output coupling
    static constexpr const char* C9_ID = "c9";    // 10u output coupling
    static constexpr const char* C10_ID = "c10";  // 100u power filtering
    
    // Transistor Parameters (gain/bias characteristics)
    static constexpr const char* Q1_GAIN_ID = "q1_gain";     // 2N3904 output buffer gain
    static constexpr const char* Q1_BIAS_ID = "q1_bias";     // 2N3904 bias point
    static constexpr const char* Q2_GAIN_ID = "q2_gain";     // 2N2369 fuzz stage gain
    static constexpr const char* Q2_BIAS_ID = "q2_bias";     // 2N2369 bias point
    static constexpr const char* Q3_GAIN_ID = "q3_gain";     // 2N2369 second stage gain
    static constexpr const char* Q3_BIAS_ID = "q3_bias";     // 2N2369 bias point
    
    // Transistor Manual Gain Controls (+/-100dB)
    static constexpr const char* Q1_MANUAL_GAIN_ID = "q1_manual_gain";  // Manual Q1 gain control
    static constexpr const char* Q2_MANUAL_GAIN_ID = "q2_manual_gain";  // Manual Q2 gain control
    static constexpr const char* Q3_MANUAL_GAIN_ID = "q3_manual_gain";  // Manual Q3 gain control
    
    // Transistor Bypass Controls (debugging)
    static constexpr const char* Q1_BYPASS_ID = "q1_bypass";  // Bypass Q1 stage
    static constexpr const char* Q2_BYPASS_ID = "q2_bypass";  // Bypass Q2 stage  
    static constexpr const char* Q3_BYPASS_ID = "q3_bypass";  // Bypass Q3 stage
    
    // Diode Parameters
    static constexpr const char* D1_THRESHOLD_ID = "d1_threshold";  // 1N5817 forward voltage
    static constexpr const char* D2_BRIGHTNESS_ID = "d2_brightness"; // LED brightness
    
    // Switch Parameters
    static constexpr const char* BYPASS_MODE_ID = "bypass_mode";     // 3PDT bypass switch
    
    // Panel mode switch
    static constexpr const char* PANEL_MODE_ID = "panelMode";
    
    // Post-processing filter
    static constexpr const char* LOWPASS_FREQ_ID = "lowpass_freq";  // Post-pedal lowpass filter frequency
    
    // Main bypass switch
    static constexpr const char* MAIN_BYPASS_ID = "main_bypass";    // Bypass the entire fuzz circuit
    
    // Physics Control Parameter IDs
    // PARASITIC EFFECTS SECTION
    static constexpr const char* PARASITIC_MASTER_ID = "parasitic_master";
    static constexpr const char* CBE_LOADING_ID = "cbe_loading";
    static constexpr const char* MILLER_FEEDBACK_ID = "miller_feedback";
    static constexpr const char* HF_INSTABILITY_ID = "hf_instability";
    
    // THERMAL DRIVE SECTION
    static constexpr const char* THERMAL_MASTER_ID = "thermal_master";
    static constexpr const char* JUNCTION_HEAT_ID = "junction_heat";
    static constexpr const char* Q2Q3_COUPLING_ID = "q2q3_coupling";
    static constexpr const char* THERMAL_SUSTAIN_ID = "thermal_sustain";
    
    // BIAS DRIFT SECTION
    static constexpr const char* BIAS_MASTER_ID = "bias_master";
    static constexpr const char* DRIFT_RATE_ID = "drift_rate";
    static constexpr const char* DRIFT_DEPTH_ID = "drift_depth";
    static constexpr const char* SUPPLY_SAG_ID = "supply_sag";
    
    // SATURATION SECTION
    static constexpr const char* SATURATION_MASTER_ID = "saturation_master";
    static constexpr const char* VBE_CURVE_ID = "vbe_curve";
    static constexpr const char* INTERMOD_ID = "intermod";
    static constexpr const char* ASYMMETRY_ID = "asymmetry";
    
    // TONE STACK BYPASS
    static constexpr const char* TONE_BYPASS_ID = "tone_bypass";
    
    // REMOVED: Auto-gain and extreme misbias controls - didn't help sustain
    
    // HARMONIC GENERATION TOGGLE
    static constexpr const char* HARMONICS_ENABLED_ID = "harmonics_enabled";
    
    // HARMONIC FILTERING TOGGLE
    static constexpr const char* HARMONIC_FILTERING_ID = "harmonic_filtering";
    
    // NEW EFFECT TOGGLES
    static constexpr const char* BIT_CRUSHING_ID = "bit_crushing_enabled";
    static constexpr const char* FREQUENCY_DOUBLING_ID = "frequency_doubling_enabled";
    static constexpr const char* CONTROLLED_GATING_ID = "controlled_gating_enabled";
    static constexpr const char* ADVANCED_COMPRESSION_ID = "advanced_compression_enabled";
    static constexpr const char* BIAS_DRIFT_ID = "bias_drift_enabled";
    static constexpr const char* ENVELOPE_BIAS_ID = "envelope_bias_enabled";
    
    // INDIVIDUAL GATING EFFECT TOGGLES
    static constexpr const char* STARVATION_GATE_ID = "starvation_gate_enabled";
    static constexpr const char* RANDOM_DROPOUTS_ID = "random_dropouts_enabled";
    static constexpr const char* Q3_CUTOUTS_ID = "q3_cutouts_enabled";
    static constexpr const char* CRACKLING_ID = "crackling_enabled";
    
    // REMOVED: All anti-gating parameter IDs - user found them ineffective
    

    /**
     * @brief Create the parameter layout for the AudioProcessor
     * @return AudioProcessorValueTreeState::ParameterLayout The parameter layout
     */
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Front panel parameter getters
    static float getInputGain(const juce::AudioProcessorValueTreeState& apvts);
    static float getFuzz(const juce::AudioProcessorValueTreeState& apvts);
    static float getVoice(const juce::AudioProcessorValueTreeState& apvts);
    static float getTreble(const juce::AudioProcessorValueTreeState& apvts);
    static float getLevel(const juce::AudioProcessorValueTreeState& apvts);
    
    // Back panel component getters (resistors in ohms, capacitors in farads)
    static float getR1(const juce::AudioProcessorValueTreeState& apvts);
    static float getR2(const juce::AudioProcessorValueTreeState& apvts);
    static float getR3(const juce::AudioProcessorValueTreeState& apvts);
    static float getR4(const juce::AudioProcessorValueTreeState& apvts);
    static float getR5(const juce::AudioProcessorValueTreeState& apvts);
    static float getR6(const juce::AudioProcessorValueTreeState& apvts);
    static float getR7(const juce::AudioProcessorValueTreeState& apvts);
    static float getR8(const juce::AudioProcessorValueTreeState& apvts);
    static float getR9(const juce::AudioProcessorValueTreeState& apvts);
    static float getR10(const juce::AudioProcessorValueTreeState& apvts);
    static float getR11(const juce::AudioProcessorValueTreeState& apvts);
    static float getR12(const juce::AudioProcessorValueTreeState& apvts);
    static float getR13(const juce::AudioProcessorValueTreeState& apvts);
    
    static float getC1(const juce::AudioProcessorValueTreeState& apvts);
    static float getC2(const juce::AudioProcessorValueTreeState& apvts);
    static float getC3(const juce::AudioProcessorValueTreeState& apvts);
    static float getC4(const juce::AudioProcessorValueTreeState& apvts);
    static float getC5(const juce::AudioProcessorValueTreeState& apvts);
    static float getC6(const juce::AudioProcessorValueTreeState& apvts);
    static float getC7(const juce::AudioProcessorValueTreeState& apvts);
    static float getC8(const juce::AudioProcessorValueTreeState& apvts);
    static float getC9(const juce::AudioProcessorValueTreeState& apvts);
    static float getC10(const juce::AudioProcessorValueTreeState& apvts);
    
    // Transistor parameter getters
    static float getQ1Gain(const juce::AudioProcessorValueTreeState& apvts);
    static float getQ1Bias(const juce::AudioProcessorValueTreeState& apvts);
    static float getQ2Gain(const juce::AudioProcessorValueTreeState& apvts);
    static float getQ2Bias(const juce::AudioProcessorValueTreeState& apvts);
    static float getQ3Gain(const juce::AudioProcessorValueTreeState& apvts);
    static float getQ3Bias(const juce::AudioProcessorValueTreeState& apvts);
    
    // Manual gain control getters (+/-100dB)
    static float getQ1ManualGain(const juce::AudioProcessorValueTreeState& apvts);
    static float getQ2ManualGain(const juce::AudioProcessorValueTreeState& apvts);
    static float getQ3ManualGain(const juce::AudioProcessorValueTreeState& apvts);
    
    // Transistor bypass getters
    static bool getQ1Bypass(const juce::AudioProcessorValueTreeState& apvts);
    static bool getQ2Bypass(const juce::AudioProcessorValueTreeState& apvts);
    static bool getQ3Bypass(const juce::AudioProcessorValueTreeState& apvts);
    
    // Diode parameter getters
    static float getD1Threshold(const juce::AudioProcessorValueTreeState& apvts);
    static float getD2Brightness(const juce::AudioProcessorValueTreeState& apvts);
    
    // Switch parameter getters
    static bool getBypassMode(const juce::AudioProcessorValueTreeState& apvts);
    
    // Panel mode getter
    static bool getPanelMode(const juce::AudioProcessorValueTreeState& apvts);
    
    // Post-processing filter getter
    static float getLowpassFreq(const juce::AudioProcessorValueTreeState& apvts);
    
    // Main bypass getter
    static bool getMainBypass(const juce::AudioProcessorValueTreeState& apvts);
    
    // Physics parameter getters
    static float getParasiticMaster(const juce::AudioProcessorValueTreeState& apvts);
    static float getCbeLoading(const juce::AudioProcessorValueTreeState& apvts);
    static float getMillerFeedback(const juce::AudioProcessorValueTreeState& apvts);
    static float getHfInstability(const juce::AudioProcessorValueTreeState& apvts);
    
    static float getThermalMaster(const juce::AudioProcessorValueTreeState& apvts);
    static float getJunctionHeat(const juce::AudioProcessorValueTreeState& apvts);
    static float getQ2Q3Coupling(const juce::AudioProcessorValueTreeState& apvts);
    static float getThermalSustain(const juce::AudioProcessorValueTreeState& apvts);
    
    static float getBiasMaster(const juce::AudioProcessorValueTreeState& apvts);
    static float getDriftRate(const juce::AudioProcessorValueTreeState& apvts);
    static float getDriftDepth(const juce::AudioProcessorValueTreeState& apvts);
    static float getSupplySag(const juce::AudioProcessorValueTreeState& apvts);
    
    static float getSaturationMaster(const juce::AudioProcessorValueTreeState& apvts);
    static float getVbeCurve(const juce::AudioProcessorValueTreeState& apvts);
    static float getIntermod(const juce::AudioProcessorValueTreeState& apvts);
    static float getAsymmetry(const juce::AudioProcessorValueTreeState& apvts);
    
    static bool getToneBypass(const juce::AudioProcessorValueTreeState& apvts);
    static bool getHarmonicsEnabled(const juce::AudioProcessorValueTreeState& apvts);
    static bool getHarmonicFiltering(const juce::AudioProcessorValueTreeState& apvts);
    static bool getBitCrushingEnabled(const juce::AudioProcessorValueTreeState& apvts);
    static bool getFrequencyDoublingEnabled(const juce::AudioProcessorValueTreeState& apvts);
    static bool getControlledGatingEnabled(const juce::AudioProcessorValueTreeState& apvts);
    static bool getAdvancedCompressionEnabled(const juce::AudioProcessorValueTreeState& apvts);
    static bool getBiasDriftEnabled(const juce::AudioProcessorValueTreeState& apvts);
    static bool getEnvelopeBiasEnabled(const juce::AudioProcessorValueTreeState& apvts);
    
    // Individual gating effect getters
    static bool getStarvationGateEnabled(const juce::AudioProcessorValueTreeState& apvts);
    static bool getRandomDropoutsEnabled(const juce::AudioProcessorValueTreeState& apvts);
    static bool getQ3CutoutsEnabled(const juce::AudioProcessorValueTreeState& apvts);
    static bool getCracklingEnabled(const juce::AudioProcessorValueTreeState& apvts);
    
    // REMOVED: All anti-gating parameter getters - user found them ineffective
    

private:
    static std::unique_ptr<juce::RangedAudioParameter> createFloatParameter(
        const juce::String& parameterID,
        const juce::String& parameterName,
        float minValue,
        float maxValue,
        float defaultValue,
        const juce::String& suffix = {}
    );
    
    static std::unique_ptr<juce::RangedAudioParameter> createBoolParameter(
        const juce::String& parameterID,
        const juce::String& parameterName,
        bool defaultValue = false
    );
};

}  // namespace TerminalFuzz