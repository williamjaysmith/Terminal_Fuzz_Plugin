#pragma once

#include <JuceHeader.h>
#include "Common.h"

namespace TerminalFuzz {
namespace DSP {

class TerminalCircuit {
public:
    struct TransistorModel {
        float hfe = 100.0f;              
        float vbe_on = 0.7f;             
        float vce_sat = 0.2f;            
        float is = 1e-14f;               
        float vt = 0.026f;
        float vbe_sat = 0.7f;
    };

    struct PhysicsControls {
        bool tone_bypass = false;          
        bool harmonics_enabled = true;     
        bool harmonic_filtering = true;    
        bool bit_crushing_enabled = false;    
        bool frequency_doubling_enabled = false; 
        bool controlled_gating_enabled = false;  
        bool advanced_compression_enabled = false; 
        bool bias_drift_enabled = false;
        bool envelope_bias_enabled = false;
        
        // Anti-gating controls (main approaches)
        bool no_gating_enabled = false;
        bool reduce_starvation_enabled = false;
        bool super_low_vbe_enabled = false;
        bool high_bias_floor_enabled = false;
        bool disable_starvation_gate_enabled = false;
        
        // Additional gating source controls
        bool disable_pickup_loading_enabled = false;
        bool disable_q3_cutouts_enabled = false;
        bool disable_random_dropouts_enabled = false;
        bool disable_ring_mod_level_enabled = false;
        
    };

    struct ComponentValues {
        // Resistor values (in ohms)
        float r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13;
        
        // Capacitor values (in farads)
        float c1, c2, c3, c4, c5, c6, c7, c8, c9, c10;
        
        // Transistor models
        TransistorModel q1_model;  
        TransistorModel q2_model;  
        TransistorModel q3_model;  
        
        // Switch states
        bool bypass_mode;          
        
        // Physics controls
        PhysicsControls physics;   
    };

    TerminalCircuit();
    ~TerminalCircuit() = default;

    void prepare(double sampleRate, int blockSize);
    void processBlock(juce::AudioBuffer<float>& buffer,
                     float inputGainDb,
                     float fuzzAmount,
                     float voiceAmount, 
                     float trebleAmount,
                     float levelAmount,
                     float q1GainDb,
                     float q2GainDb,
                     float q3GainDb,
                     bool q1Bypass,
                     bool q2Bypass,
                     bool q3Bypass,
                     const ComponentValues& componentValues);
    void reset();
    
    // Input level tracking for LED meter
    float getInputLevelDb() const;

private:
    struct FilterState {
        float x1 = 0.0f;
        float y1 = 0.0f;
    };

    struct ChannelState {
        FilterState c1_state;
        FilterState c3_state;
        FilterState voice_state;
        FilterState treble_state;
        FilterState c7_state;
        FilterState output_coupling_state;
        
        // Per-channel bias drift states (was static)
        float ring_mod_phase = 0.0f;
        float input_bias_drift = 0.0f;
        float thermal_bias_drift = 0.0f;
        float random_bias_drift = 0.0f;
        float component_aging = 1.0f;
        float bias_spike_timer = 0.0f;
        float bias_spike = 0.0f;
        float envelope_follower = 0.0f;
        float starvation_bias_spike_timer = 0.0f;
        float starvation_bias_spike = 0.0f;
        float starvation_gate = 1.0f;
        float crackle_phase = 0.0f;
        int sample_hold_counter = 0;
        float held_sample = 0.0f;
        float gate_envelope = 0.0f;
        float sustain_timer = 0.0f;
        int q3_cutout_counter = 0;
    };

    float processSample(float sample, int channel,
                       float inputGainDb, float fuzzAmount, float voiceAmount, 
                       float trebleAmount, float levelAmount,
                       float q1GainDb, float q2GainDb, float q3GainDb,
                       bool q1Bypass, bool q2Bypass, bool q3Bypass,
                       const ComponentValues& cv);

    // Circuit stages
    float inputStage(float sample, int channel, const ComponentValues& cv);
    float q2TransistorStage(float sample, int channel, float q2GainDb, const ComponentValues& cv);
    float applyToneStack(float input, float voiceAmount, float trebleAmount, const ComponentValues& cv);
    float q3TransistorStage(float sample, int channel, float voiceAmount, float trebleAmount, float q3GainDb, const ComponentValues& cv);
    float q1OutputStage(float sample, int channel, float q1GainDb, const ComponentValues& cv);
    float outputStage(float sample, int channel, float levelAmount, const ComponentValues& cv);
    
    // Capacitor modeling
    float applyCouplingCapacitor(float input, float capacitance, float cutoff_frequency, int channel);
    
    // Real BJT Physics Modeling (Ebers-Moll)
    float bjt_ebers_moll(float vin, float vce, const TransistorModel& model, float supply_voltage, 
                         float emitter_resistor, float collector_resistor, float base_bias_resistor);
    float get_starved_supply_voltage() const;
    
    // BJT Parameter Auto-Tuning and Logging
    struct BJT_AutoTuner {
        bool enabled = false;  // DISABLE auto-tuner - it's reducing our scaling!
        int sample_count = 0;
        int test_phase = 0;  // Which parameter we're testing
        float current_scale = 10000.0f;  // Fixed optimal scaling from debug results
        float current_vt_scale = 1.0f;
        float current_hfe_scale = 1.0f;
        float best_scale = 1000.0f;
        float best_fuzz_character = 0.0f;  // Best "fuzziness" score
        float total_distortion = 0.0f;     // Track harmonic content
        float signal_strength = 0.0f;      // Track signal presence
        
        // Comprehensive test ranges for parameter sweeping
        std::vector<float> scale_test_values = {1000.0f, 10000.0f, 100000.0f, 500000.0f, 1000000.0f, 5000000.0f, 10000000.0f, 50000000.0f, 100000000.0f};
        std::vector<float> vt_test_values = {0.01f, 0.1f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f};
        std::vector<float> hfe_test_values = {0.1f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f, 20.0f};
        
        // Helper functions are now standalone (defined in .cpp)
    };
    
    mutable BJT_AutoTuner autoTuner_;  // mutable so const functions can use it

    // Filters
    float highPassFilter(float input, float frequency, FilterState& state);
    float lowPassFilter(float input, float frequency, FilterState& state);

    std::array<ChannelState, Constants::MAX_CHANNELS> channelStates_;
    double sampleRate_ = Constants::DEFAULT_SAMPLE_RATE;
    
    // Input level tracking for LED meter
    struct InputLevelTracker {
        float averageLevel = 0.0f;
        float envelopeFollower = 0.0f;
        int sampleCounter = 0;
        static const int AVERAGING_SAMPLES = 96000; // 2 seconds at 48kHz
    } inputLevel_;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TerminalCircuit)
};

}  // namespace DSP
}  // namespace TerminalFuzz