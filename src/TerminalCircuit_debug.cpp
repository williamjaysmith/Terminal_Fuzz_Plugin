#include "TerminalCircuit.h"
#include <fstream>
#include <sstream>
#include <cmath>

// Forward declarations for debug functions
void writeAutoTunerLog(const std::string& message);
float calculateFuzzCharacter(float sample);
void logParameterTest(float param_value, float fuzz_score, const std::string& param_name);

namespace TerminalFuzz {
namespace DSP {

TerminalCircuit::TerminalCircuit() {
    reset();
}

void TerminalCircuit::prepare(double sampleRate, int blockSize) {
    juce::ignoreUnused(blockSize);
    sampleRate_ = sampleRate;
    reset();
}

void TerminalCircuit::processBlock(juce::AudioBuffer<float>& buffer,
                                  float inputGainDb,
                                  float fuzzAmount,
                                  float voiceAmount,
                                  float trebleAmount,
                                  float levelAmount,
                                  const ComponentValues& componentValues) {
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Audio processing - no debug logging in audio thread

    // Convert input gain from dB to linear multiplier
    float inputGainLinear = juce::Decibels::decibelsToGain(inputGainDb);
    
    fuzzAmount = juce::jlimit(0.0f, 1.0f, fuzzAmount);
    voiceAmount = juce::jlimit(0.0f, 1.0f, voiceAmount);
    trebleAmount = juce::jlimit(0.0f, 1.0f, trebleAmount);
    levelAmount = juce::jlimit(0.0f, 1.0f, levelAmount);

    for (int channel = 0; channel < numChannels && channel < Constants::MAX_CHANNELS; ++channel) {
        float* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample) {
            float inputSample = channelData[sample];
            if (std::isnan(inputSample) || std::isinf(inputSample)) {
            }
            
            // DEBUG: Check first few samples of first channel for non-zero audio
            if (processBlock_count <= 3 && channel == 0 && sample < 5) {
            
            // Apply input gain before circuit processing
            inputSample *= inputGainLinear;
            
            // EXHAUSTIVE AUDIO PATH LOGGING - EVERY SINGLE STEP
            if (processBlock_count <= 3 && channel == 0 && sample < 5) {
                    fclose(debug_file);
                }
            }
            
            // Track input level for LED meter (only on first channel to avoid double counting)
            if (channel == 0) {
                float inputLevel = std::abs(inputSample);
                
                // Envelope follower for smoothing - faster LED response
                float attack = 0.95f;   // Much faster attack (was 0.99)
                float release = 0.992f; // Much faster release (was 0.9995)
                if (inputLevel > inputLevel_.envelopeFollower) {
                    inputLevel_.envelopeFollower = attack * inputLevel_.envelopeFollower + (1.0f - attack) * inputLevel;
                } else {
                    inputLevel_.envelopeFollower = release * inputLevel_.envelopeFollower + (1.0f - release) * inputLevel;
                }
                
                // Update running average every few samples for efficiency
                inputLevel_.sampleCounter++;
                if (inputLevel_.sampleCounter >= 256) { // Update every 256 samples
                    inputLevel_.averageLevel = inputLevel_.envelopeFollower;
                    inputLevel_.sampleCounter = 0;
                }
            }
            
            // REMOVED: Auto-gain logic - didn't help with sustain
            
            float processedSample = processSample(
                inputSample, channel,
                inputGainDb, fuzzAmount, voiceAmount, trebleAmount, levelAmount,
                componentValues
            );
            
            // EXHAUSTIVE OUTPUT VERIFICATION - EVERY SINGLE STEP
            if (processBlock_count <= 3 && channel == 0 && sample < 5) {
                    if (std::isnan(processedSample) || std::isinf(processedSample)) {
                        fprintf(debug_file, "!!! CRITICAL ERROR: NaN/INF detected! Setting to 0.0f !!!\n");
                        processedSample = 0.0f;
                    }
                    
                    fprintf(debug_file, "About to write to channelData[%d] = %.9f\n", sample, processedSample);
                    fprintf(debug_file, "=== END FINAL VERIFICATION ===\n\n");
                    fclose(debug_file);
                }
            }
            
            channelData[sample] = processedSample;
            
            // VERIFY THE ACTUAL WRITE OCCURRED
            if (processBlock_count <= 3 && channel == 0 && sample < 5) {
        }
    }
    
    // CRITICAL STEREO FIX: Force stereo output regardless of input channels
    // DEBUG: Log channel configuration
    if (processBlock_count <= 3) {
        }
    }
    
    // FORCE STEREO: Always try to write to channel 1 even if only 1 input channel
    if (buffer.getNumChannels() >= 2) {
        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);
        
        for (int sample = 0; sample < numSamples; ++sample) {
            rightChannel[sample] = leftChannel[sample];  // Duplicate left to right
        }
        
        // DEBUG: Verify stereo duplication
        if (processBlock_count <= 3) {
                fclose(debug_file);
            }
        }
    } else {
        // DEBUG: Log if we can't create stereo
        if (processBlock_count <= 3) {
    }
}

void TerminalCircuit::reset() {
    for (auto& state : channelStates_) {
        state = ChannelState{};
    }
}

float TerminalCircuit::processSample(float sample, int channel,
                                   float inputGainDb, float fuzzAmount, float voiceAmount,
                                   float trebleAmount, float levelAmount,
                                   const ComponentValues& cv) {
    if (std::isnan(sample) || std::isinf(sample)) {
    }
    
    // EXHAUSTIVE PARAMETER AND INPUT VERIFICATION
    static int processSample_count = 0;
    processSample_count++;
    if (processSample_count <= 5) {
            fprintf(debug_file, "  inputGainDb: %.3f\n", inputGainDb);
            fprintf(debug_file, "  fuzzAmount: %.6f\n", fuzzAmount);
            fprintf(debug_file, "  voiceAmount: %.6f\n", voiceAmount);
            fprintf(debug_file, "  trebleAmount: %.6f\n", trebleAmount);
            fprintf(debug_file, "  levelAmount: %.6f\n", levelAmount);
            fprintf(debug_file, "Circuit State:\n");
            fprintf(debug_file, "  cv.bypass_mode: %s\n", cv.bypass_mode ? "TRUE (BYPASSED)" : "FALSE (ACTIVE)");
            fprintf(debug_file, "Sample Rate: %.1f Hz\n", sampleRate_);
            if (cv.bypass_mode) {
                fprintf(debug_file, ">>> CIRCUIT BYPASSED - will return sample * levelAmount <<<\n");
            } else {
                fprintf(debug_file, ">>> CIRCUIT ACTIVE - will process through BJT chain <<<\n");
            }
            fclose(debug_file);
        }
    }
    
    if (cv.bypass_mode) {
        return sample * levelAmount;
    }
    
    // TEST: Q2-ONLY CIRCUIT TO ISOLATE ISSUE
    float inputSignal = sample * 1.0f;
    
    // DEBUG: Track signal through each stage - ALWAYS LOG FIRST 5 SAMPLES
    if (processSample_count <= 5) {
    
    // Fuzz control with DOUBLE power for sustain without losing character
    float fuzzLevel = 10.0f + fuzzAmount * 490.0f;  // 10x to 500x gain range
    float fuzzedSignal = inputSignal * fuzzLevel;
    
    float q2Output = q2TransistorStage(fuzzedSignal, channel, cv);
    
    // DEBUG: Track signal through each stage
    if (processSample_count <= 5) {
    
    // Q3 Gain Stage (2N2369) - second gain stage  
    float q3Output = q3TransistorStage(q2Output, channel, voiceAmount, trebleAmount, cv);
    
    // Apply level control and boost to maintain audible levels
    float finalOutput = q3Output * levelAmount * 10.0f; // Keep 10x boost for audible levels
    
    // DEBUG: Track Q2+Q3 output
    if (processSample_count <= 5) {
            } else {
                fprintf(debug_file, "OUTPUT LEVEL: %.6f - TOO QUIET! ❌\n", finalOutput);
            }
            fclose(debug_file);
        }
    }
    
    return finalOutput;
}

float TerminalCircuit::inputStage(float sample, int channel, const ComponentValues& cv) {
    auto& state = channelStates_[channel];
    
    // C1 input coupling (100nF) - high-pass filter
    float input_fc = 25.0f;  // ~25Hz cutoff for input coupling
    sample = highPassFilter(sample, input_fc, state.c1_state);
    
    // Guitar pickup loading simulation (bypassable)
    if (!cv.physics.disable_pickup_loading_enabled) {
        float heavy_loading = 0.65f;
        float pickup_interaction = std::abs(sample) * 0.25f;
        float total_loading = heavy_loading - pickup_interaction;
        total_loading = juce::jlimit(0.4f, 0.85f, total_loading);
        sample *= total_loading;
    }
    
    // Ring modulation for broken vintage character (if physics enabled)
    if (cv.physics.harmonics_enabled) {
        float ring_freq;
        if (cv.physics.disable_ring_mod_level_enabled) {
            ring_freq = 220.0f;  // Fixed frequency - no level dependence
        } else {
            ring_freq = 120.0f + std::abs(sample) * 200.0f;  // Frequency varies with input
        }
        
        state.ring_mod_phase += (ring_freq * 2.0f * 3.14159f) / sampleRate_;
        if (state.ring_mod_phase > 6.28318f) state.ring_mod_phase -= 6.28318f;
        
        float ring_mod = std::sin(state.ring_mod_phase) * 0.15f;
        sample = sample * (1.0f + ring_mod);  // Ring modulate the input
    }
    
    // Additional high-pass filtering for vintage character
    if (cv.physics.harmonic_filtering) {
        float character_fc = 45.0f;  // Remove some bass for vintage sound
        sample = highPassFilter(sample, character_fc, state.c7_state);
    }
    
    // Input stage bias drift simulation
    state.input_bias_drift += (((rand() % 1000) / 1000.0f) - 0.5f) * 0.00001f;  // Slow random drift
    state.input_bias_drift = juce::jlimit(-0.02f, 0.02f, state.input_bias_drift);
    sample += state.input_bias_drift;
    
    return sample;
}

float TerminalCircuit::q2TransistorStage(float sample, int channel, const ComponentValues& cv) {
    auto& state = channelStates_[channel];
    
    // Restored Q2 (2N2369) with proper gain staging
    float vbe_threshold = cv.q2_model.vbe_on;  // 0.7V
    
    // ANTI-GATING APPROACH 3: Super low VBE threshold
    if (cv.physics.super_low_vbe_enabled) {
        vbe_threshold *= 0.14f;  // 0.1V - MUCH more aggressive than before
    }
    
    float gain_factor = cv.q2_model.hfe * 0.01f;  // Transistor gain
    
    // Clean BJT operation - no artificial bias drift needed
    // Real BJT physics provides authentic behavior through proper modeling
    float total_bias = 0.05f;  // Simple forward bias
    
    // Simple bias logging for pure BJT operation
    static int bias_log_count = 0;
    bias_log_count++;
    if (bias_log_count <= 5) {
    }
    
    float adjusted_input = sample + total_bias;
    
    // EXHAUSTIVE FINAL INPUT CALCULATION LOGGING
    static int input_calc_count = 0;
    input_calc_count++;
    if (input_calc_count <= 5) {
            fclose(debug_file);
        }
    }
    
    // A/B TEST: Real BJT Physics vs Simple Tanh Saturation
    float collector_current;
    
    // TESTING STRATEGY: 
    // 1. First test Q2 BJT physics vs tanh (use_real_bjt = true/false)
    // 2. If Q2 sounds good, then enable Q3 BJT (use_real_bjt_q3 = true)  
    // 3. Keep Q1 simple for now since it's just output buffer
    
    // OPTION A: Real BJT Physics using Ebers-Moll equations (NEW)
    bool use_real_bjt = true;  // ENABLED: Let's debug this BJT physics!
    
    if (use_real_bjt) {
        // DEBUG: Simple file output to verify we're entering BJT path
        static bool logged_entry = false;
        if (!logged_entry) {
        }
        
        // Calculate VCE estimate (rough approximation for BJT modeling)
        float vce_estimate = get_starved_supply_voltage() * 0.5f;  // Mid-rail estimate
        
        // EXHAUSTIVE Q2 VARIABLE TRACKING - EVERY SINGLE CALCULATION
        static int q2_var_count = 0;
        q2_var_count++;
        if (q2_var_count <= 5) {
                fprintf(debug_file, "  Q2 VCE_estimate = %.9f * 0.5f = %.9f\n", get_starved_supply_voltage(), vce_estimate);
                fprintf(debug_file, "Q2 Component Values:\n");
                fprintf(debug_file, "  cv.r4 (bias) = %.0f ohms\n", cv.r4);
                fprintf(debug_file, "  cv.r2 (collector) = %.0f ohms\n", cv.r2);
                fprintf(debug_file, "  cv.r9 (emitter) = %.0f ohms\n", cv.r9);
                fprintf(debug_file, "Q2 Model Parameters:\n");
                fprintf(debug_file, "  cv.q2_model.hfe = %.3f\n", cv.q2_model.hfe);
                fprintf(debug_file, "  cv.q2_model.vbe_on = %.6f\n", cv.q2_model.vbe_on);
                fprintf(debug_file, "  cv.q2_model.is = %.2e\n", cv.q2_model.is);
                fprintf(debug_file, "About to call bjt_ebers_moll() for Q2...\n");
                fclose(debug_file);
            }
        }

        // Use real BJT physics with authentic Terminal component values
        float r4_bias = cv.r4;    // 47kΩ base bias resistor (from schematic)
        float r2_collector = cv.r2;  // 22kΩ collector load resistor (from schematic) 
        float r9_emitter = cv.r9;    // 470Ω emitter resistor (from schematic)
        
        // Call real Ebers-Moll BJT model for authentic transistor behavior
        collector_current = bjt_ebers_moll(adjusted_input, vce_estimate, cv.q2_model, 
                                          get_starved_supply_voltage(), r9_emitter, 
                                          r2_collector, r4_bias);

        // EXHAUSTIVE Q2 OUTPUT VERIFICATION
        static int q2_out_count = 0;
        q2_out_count++;
        if (q2_out_count <= 5) {
                    fprintf(debug_file, "Q2 Gain Factor: %.9f / %.9f = %.6fx\n", collector_current, adjusted_input, collector_current / adjusted_input);
                }
                fprintf(debug_file, "Q2 Output absolute value: %.9f\n", std::abs(collector_current));
                fprintf(debug_file, "Q2 Output is %s\n", (std::abs(collector_current) > 0.001f) ? "SIGNIFICANT" : "VERY SMALL");
                fclose(debug_file);
            }
        }
        
        // Pure BJT physics - no legacy effects needed
    } else {
        // OPTION B: Original Tanh Saturation (PRESERVED FOR COMPARISON)
        // ANTI-GATING APPROACH 1: NO GATING - completely bypass cutoff logic
        if (cv.physics.no_gating_enabled) {
            // Always allow current flow - never cut off
            float overdrive = adjusted_input - (vbe_threshold * 0.1f);  // Very low threshold
            collector_current = std::tanh(overdrive * 2.0f) * gain_factor;
        } else if (adjusted_input < vbe_threshold) {
            collector_current = 0.0f;  // Cut off
        } else {
            // Saturated operation for fuzz character
            float overdrive = adjusted_input - vbe_threshold;
            collector_current = std::tanh(overdrive * 2.0f) * gain_factor;
        
            // BROKEN TRANSISTOR MODELING - Authentic Terminal/FY-2 character
            // This technique models:
            // 1. Bias starvation from failing power supply or dried electrolytic caps
            // 2. Asymmetric clipping from component aging and thermal drift  
            // 3. Random bias spikes from loose connections or failing components
            // 4. Input-dependent gating from severely starved bias points
            
            // Base broken character (always applied for authenticity)
            collector_current *= 1.5f;  // Push into moderate distortion
            
            // PERMANENT EXTREME STARVATION EFFECTS (always enabled for maximum broken character)
            // Add severe additional distortion
            collector_current *= 1.8f;  // Push much harder into distortion
            
            // Random dropouts (like dying transistor) - bypassable
            if (!cv.physics.disable_random_dropouts_enabled && (rand() % 200) < 3) {  // 1.5% chance
                collector_current *= 0.2f;  // Moderate dropout
            }
            
            // Crackling/sputtering effect
            state.crackle_phase += 0.1f;
            if (state.crackle_phase > 6.28f) state.crackle_phase = 0.0f;
            float crackle = std::sin(state.crackle_phase * 23.0f) * 0.3f;
            collector_current += crackle * std::abs(collector_current);
        }
    } // End of tanh vs BJT A/B test section
    
    // REMOVED ALL LEGACY EFFECTS - Pure BJT physics provides authentic character
    // No harmonics, bit crushing, gating, compression, or asymmetric clipping needed
    // The real Ebers-Moll BJT equations handle all the authentic distortion
    
    return collector_current;  // Pure BJT output
}

float TerminalCircuit::q3TransistorStage(float sample, int channel, float voiceAmount, float trebleAmount, const ComponentValues& cv) {
    auto& state = channelStates_[channel];
    
    // BYPASS ALL FILTERING FOR Q3 TEST - these were killing 72% of signal
    // C3 coupling (Q2 to Q3) - DISABLED
    // sample = highPassFilter(sample, coupling_fc, state.c3_state);
    
    // Voice control - DISABLED  
    // sample = lowPassFilter(sample, voice_fc, state.voice_state);
    
    // Treble control - DISABLED
    // sample = sample * (1.0f - treble_amount) + treble_signal * treble_amount;
    
    // A/B TEST: Q3 Real BJT Physics vs Simple Gain
    float output;
    
    // OPTION A: Real BJT Physics for Q3 (NEW) 
    bool use_real_bjt_q3 = true;  // ENABLED: Full BJT physics on all transistors
    
    if (use_real_bjt_q3) {
        // Calculate VCE estimate for Q3
        float vce_estimate = get_starved_supply_voltage() * 0.4f;  // Q3 runs lower VCE
        
        // Use real BJT physics with Q3 component values from schematic
        float r8_bias = cv.r8;       // 470kΩ base bias resistor (from schematic)
        float r7_collector = cv.r7;  // 47kΩ collector load resistor (from schematic)
        float r9_emitter = cv.r9;    // 470Ω emitter resistor (shared with Q1)
        
        // Call real Ebers-Moll BJT model for Q3 with mismatched characteristics
        output = bjt_ebers_moll(sample, vce_estimate, cv.q3_model, 
                               get_starved_supply_voltage(), r9_emitter, 
                               r7_collector, r8_bias);
    } else {
        // OPTION B: Original Simple Gain (PRESERVED FOR COMPARISON)
        output = sample * 1.2f;  // Basic gain
        
        // PERMANENT Q3 misbias effects (always enabled for broken character)
        // Q3 bias starvation - different pattern than Q2, but less severe gain reduction
        output *= 1.5f;  // FIXED: Boost instead of cut (was 0.6f killing signal!)
    }
    
    // Q3 cutouts (like intermittent connection) - bypassable
    // COMMENTED OUT FOR BJT TESTING - this was killing 90% of signal!
    /*if (!cv.physics.disable_q3_cutouts_enabled) {
        state.q3_cutout_counter++;
        if ((state.q3_cutout_counter % 1000) < 50) {  // Brief cutouts
            output *= 0.1f;
        }
    }*/
    
    // Asymmetric clipping due to bias failure
    // COMMENTED OUT FOR BJT TESTING - this was hard limiting at 0.2f!
    /*if (output > 0.2f) {
        output = 0.2f + (output - 0.2f) * 0.05f;  // Severe positive clipping
    }*/
    
    // REMOVED legacy tanh/clipping - let pure BJT physics dominate for Q3
    // The real BJT physics already provides the character we need
    
    return output;  // Pure BJT output without legacy processing
}

float TerminalCircuit::q1OutputStage(float sample, int channel, const ComponentValues& cv) {
    juce::ignoreUnused(channel);
    
    // Q1 BJT Physics Option (2N3904 Output Buffer)
    bool use_real_bjt_q1 = true;  // Enable BJT physics for complete authenticity
    
    float output;
    
    if (use_real_bjt_q1) {
        // Calculate VCE estimate for Q1 output stage
        float vce_estimate = get_starved_supply_voltage() * 0.5f;  // Q1 runs moderate VCE
        
        // EXHAUSTIVE Q1 VARIABLE TRACKING - EVERY SINGLE CALCULATION
        static int processSample_count_q1 = 0;
        processSample_count_q1++;
        if (processSample_count_q1 <= 5) {
                fprintf(debug_file, "  Q1 VCE_estimate = %.9f * 0.5f = %.9f\n", get_starved_supply_voltage(), vce_estimate);
                fprintf(debug_file, "Q1 Component Values:\n");
                fprintf(debug_file, "  cv.r10 (bias) = %.0f ohms\n", cv.r10);
                fprintf(debug_file, "  cv.r11 (collector) = %.0f ohms\n", cv.r11);
                fprintf(debug_file, "  cv.r9 (emitter) = %.0f ohms\n", cv.r9);
                fprintf(debug_file, "Q1 Model Parameters:\n");
                fprintf(debug_file, "  cv.q1_model.hfe = %.3f\n", cv.q1_model.hfe);
                fprintf(debug_file, "  cv.q1_model.vbe_on = %.6f\n", cv.q1_model.vbe_on);
                fprintf(debug_file, "  cv.q1_model.is = %.2e\n", cv.q1_model.is);
                fprintf(debug_file, "About to call bjt_ebers_moll() for Q1...\n");
                fclose(debug_file);
            }
        }
        
        // Use real BJT physics with Q1 component values from schematic  
        float r10_bias = cv.r10;     // 10kΩ base bias resistor (from schematic)
        float r11_collector = cv.r11; // 100kΩ collector load resistor (from schematic)
        float r9_emitter = cv.r9;    // 470Ω emitter resistor (shared with Q3)
        
        // Call real Ebers-Moll BJT model for Q1 (2N3904 characteristics)
        output = bjt_ebers_moll(sample, vce_estimate, cv.q1_model, 
                               get_starved_supply_voltage(), r9_emitter, 
                               r11_collector, r10_bias);

        // EXHAUSTIVE Q1 OUTPUT VERIFICATION (before gain adjustment)
        if (processSample_count_q1 <= 5) {
                    fprintf(debug_file, "Q1 Raw Gain Factor: %.9f / %.9f = %.6fx\n", output, sample, output / sample);
                }
                fprintf(debug_file, "Q1 Output absolute value: %.9f\n", std::abs(output));
                fprintf(debug_file, "Q1 Output is %s\n", (std::abs(output) > 0.001f) ? "SIGNIFICANT" : "VERY SMALL");
                fprintf(debug_file, "About to apply 4.0x output drive gain...\n");
                fclose(debug_file);
            }
        }
        
        // Q1 output buffer gain adjustment - provide OUTPUT DRIVE, not just buffering
        // Real Terminal Fuzz Q1 provides substantial gain for audible output
        output *= 4.0f;  // BOOST for proper output drive (was 0.3f killing signal!)

        // FINAL Q1 OUTPUT VERIFICATION (after gain adjustment)
        if (processSample_count_q1 <= 5) {
                fclose(debug_file);
            }
        }
    } else {
        // Simple unity gain buffer (original)
        output = sample * 1.0f;
    }
    
    return output;
}

float TerminalCircuit::outputStage(float sample, int channel, float levelAmount, const ComponentValues& cv) {
    juce::ignoreUnused(channel, cv);
    
    // Output stage with proper gain for audible levels  
    float base_gain = 3.0f;  // Base output gain for audible fuzz levels
    float output = sample * levelAmount * base_gain;
    
    // DETAILED OUTPUT STAGE LOGGING - first 5 samples only
    static int output_stage_count = 0;
    output_stage_count++;
    if (output_stage_count <= 5) {
            fprintf(debug_file, "Before limiting: %.6f\n", output);
            fclose(debug_file);
        }
    }
    
    // Final output limiting with headroom for fuzz character
    output = juce::jlimit(-2.0f, 2.0f, output);  // Allow higher levels for fuzz
    
    // LOG FINAL OUTPUT LEVEL
    if (output_stage_count <= 5) {
    }
    
    return output;
}

// BJT Auto-Tuner Helper Functions (simplified to avoid compilation issues)
void writeAutoTunerLog(const std::string& message) {
    static std::ofstream logFile;
    if (!logFile.is_open()) {
        std::string logPath = "/Users/williamsmith/Desktop/terminal_fuzz_bjt_tuning.log";
        logFile.open(logPath, std::ios::app);
        if (logFile.is_open()) {
            logFile << "=== TERMINAL FUZZ BJT AUTO-TUNING LOG ===" << std::endl;
            logFile << "Testing BJT parameters to find optimal fuzz character" << std::endl;
            logFile << "Looking for: Strong signal + Good distortion + Not silent" << std::endl;
            logFile << std::endl;
        }
    }
    
    if (logFile.is_open()) {
        logFile << message << std::endl;
        logFile.flush();  // Immediate write
    }
}

float calculateFuzzCharacter(float sample) {
    // Score how "fuzzy" the output is - we WANT clipping and distortion!
    float abs_sample = std::abs(sample);
    
    // Signal strength (avoid silence)
    float strength_score = juce::jlimit(0.0f, 1.0f, abs_sample * 10.0f);
    
    // Distortion character (reward clipping and non-linearity)
    float distortion_score = 0.0f;
    if (abs_sample > 0.5f) {
        distortion_score += 0.5f;  // Good - we're clipping
    }
    if (abs_sample > 0.8f) {
        distortion_score += 0.3f;  // Even better - hard clipping
    }
    
    // Bonus for asymmetric behavior (like real broken transistors)
    static float last_sample = 0.0f;
    if ((sample > 0 && last_sample < 0) || (sample < 0 && last_sample > 0)) {
        distortion_score += 0.2f;  // Zero crossings = good fuzz
    }
    last_sample = sample;
    
    return strength_score + distortion_score;  // Total fuzz character score
}

void logParameterTest(float param_value, float fuzz_score, const std::string& param_name) {
    std::stringstream msg;
    msg << param_name << " = " << param_value << ", Fuzz Score = " << fuzz_score;
    writeAutoTunerLog(msg.str());
}

// Real BJT Physics Modeling using Ebers-Moll equations with DEBUG LOGGING
float TerminalCircuit::bjt_ebers_moll(float vin, float vce, const TransistorModel& model, 
                                      float supply_voltage, float emitter_resistor, 
                                      float collector_resistor, float base_bias_resistor) {
    static int debug_count = 0;
    debug_count++;
    
    const float VT = 0.026f * autoTuner_.current_vt_scale;  // Auto-tuned thermal voltage
    const float MIN_VOLTAGE = 1e-6f;  // Prevent numerical issues
    
    // Starved supply voltage for authentic Terminal character (7-8V instead of 9V)
    float vcc = get_starved_supply_voltage();
    
    // DETAILED BJT CALCULATION LOGGING - every single step
    static int bjt_detail_count = 0;
    bjt_detail_count++;
    
    // UNIFORM BJT SCALING - same scaling for all transistors like real hardware
    // Real transistors don't adapt - same VBE response regardless of input level
    float vbe_scaling = 0.002f;  // Uniform scaling for all transistors (Q1, Q2, Q3)
    
    float vbe_input_factor = vin * vbe_scaling;
    float vbe_raw = model.vbe_on + vbe_input_factor;
    float vbe = juce::jlimit(0.3f, 0.9f, vbe_raw);  // Tighter VBE range for stability
    
    // Base current using Ebers-Moll equation: Ib = Is * (exp(Vbe/VT) - 1)
    float exp_arg_raw = vbe / VT;
    float exp_arg = (exp_arg_raw > 20.0f) ? 20.0f : exp_arg_raw;  // Prevent exp overflow
    float exp_result = std::exp(exp_arg);
    float base_current_raw = model.is * (exp_result - 1.0f);
    float base_current = juce::jlimit(MIN_VOLTAGE, 0.1f, base_current_raw);  // Practical limits
    
    // CRITICAL DEBUGGING - Log UNIFORM BJT calculation
    if (bjt_detail_count <= 5) {
            fprintf(debug_file, "  model.vbe_on = %.6f\n", model.vbe_on);
            fprintf(debug_file, "  vbe_raw = vbe_on + factor = %.6f + %.6f = %.6f\n", model.vbe_on, vbe_input_factor, vbe_raw);
            fprintf(debug_file, "  vbe_final = jlimit(0.3, 0.9, %.6f) = %.6f\n", vbe_raw, vbe);
            fprintf(debug_file, "  exp_arg_raw = vbe/VT = %.6f/%.6f = %.6f\n", vbe, VT, exp_arg_raw);
            fprintf(debug_file, "  exp_arg_clamped = %.6f\n", exp_arg);
            fprintf(debug_file, "  exp_result = exp(%.6f) = %.6f\n", exp_arg, exp_result);
            fprintf(debug_file, "  base_current_raw = is*(exp-1) = %.9e*(%.6f-1) = %.9e\n", model.is, exp_result, base_current_raw);
            fprintf(debug_file, "  base_current_final = %.9e\n", base_current);
            fclose(debug_file);
        }
    }
    
    // hFE varies with collector current (like real transistors)
    float dynamic_hfe_initial = model.hfe * autoTuner_.current_hfe_scale;  // Auto-tuned hFE
    float collector_current_estimate = base_current * dynamic_hfe_initial;
    
    // hFE rolloff at high currents (realistic behavior)
    float dynamic_hfe_final = dynamic_hfe_initial;
    if (collector_current_estimate > 0.01f) {
        dynamic_hfe_final *= (0.01f / collector_current_estimate);  // Beta rolloff
    }
    
    // Collector current: Ic = hFE * Ib (with Early effect approximation)
    float early_factor = 1.0f + (vce / 100.0f);  // Simple Early effect (Va ≈ 100V)
    float collector_current = dynamic_hfe_final * base_current * early_factor;
    
    // CONTINUE DETAILED LOGGING for collector current calculation
    if (bjt_detail_count <= 5) {
            if (collector_current_estimate > 0.01f) {
                fprintf(debug_file, "  hFE rolloff: %.6f * (0.01/%.9e) = %.6f\n", dynamic_hfe_initial, collector_current_estimate, dynamic_hfe_final);
            } else {
                fprintf(debug_file, "  No hFE rolloff (estimate <= 0.01)\n");
            }
            fprintf(debug_file, "  vce = %.6f\n", vce);
            fprintf(debug_file, "  early_factor = 1 + vce/100 = 1 + %.6f/100 = %.6f\n", vce, early_factor);
            fprintf(debug_file, "  collector_current = hfe*base*early = %.6f*%.9e*%.6f = %.9e\n", dynamic_hfe_final, base_current, early_factor, collector_current);
            fclose(debug_file);
        }
    }
    
    // Voltage drop across collector resistor
    float collector_voltage_drop = collector_current * collector_resistor;
    float actual_vce_raw = vcc - collector_voltage_drop;
    float actual_vce = std::max(model.vce_sat, actual_vce_raw);  // Can't go below saturation
    
    // Emitter resistor feedback (causes instability in FY-2 circuits)
    float emitter_voltage_drop = collector_current * emitter_resistor;
    float emitter_feedback = emitter_voltage_drop * 0.1f;  // Partial feedback
    
    // Output is collector current scaled and with emitter feedback instability
    float current_scale = 2000.0f;  // Reduced from 10,000 for smoother fuzz (less crackling)
    float output_raw = collector_current * current_scale;  // Fixed gentler scale
    float output = output_raw - emitter_feedback;  // Negative feedback creates instability
    
    // LOG ALL SURROUNDING PHYSICS - voltages, drops, feedback, scaling
    if (bjt_detail_count <= 5) {
            fprintf(debug_file, "  model.vce_sat = %.6f\n", model.vce_sat);
            fprintf(debug_file, "  actual_vce = max(%.6f, %.6f) = %.6f\n", model.vce_sat, actual_vce_raw, actual_vce);
            fprintf(debug_file, "  emitter_resistor = %.0f\n", emitter_resistor);
            fprintf(debug_file, "  emitter_voltage_drop = Ic*Re = %.9e * %.0f = %.6f\n", collector_current, emitter_resistor, emitter_voltage_drop);
            fprintf(debug_file, "  emitter_feedback = drop * 0.1 = %.6f * 0.1 = %.6f\n", emitter_voltage_drop, emitter_feedback);
            fprintf(debug_file, "  current_scale = %.0f\n", current_scale);
            fprintf(debug_file, "  output_raw = Ic * scale = %.9e * %.0f = %.6f\n", collector_current, current_scale, output_raw);
            fprintf(debug_file, "  output_final = raw - feedback = %.6f - %.6f = %.6f\n", output_raw, emitter_feedback, output);
            fprintf(debug_file, "=== END DETAILED BJT CALCULATION ===\n\n");
            fclose(debug_file);
        }
    }
    
    // CRITICAL: Check for NaN/Inf generation in BJT calculation
    if (std::isnan(output) || std::isinf(output)) {
            fclose(debug_file);
        }
    }
    
    // Saturation limiting (hard clipping when starved)
    if (actual_vce <= model.vce_sat) {
        output *= 0.1f;  // Severe limiting in saturation
    }
    
    // COMPREHENSIVE DEBUG LOGGING - Print detailed calculations
    if (debug_count <= 20) {
            fprintf(debug_file, "  Supply: Vcc=%.6f (starved from 9V)\n", vcc);
            fprintf(debug_file, "  BJT Calc: VBE=%.6f, Ib=%.9e, Ic=%.9e\n", vbe, base_current, collector_current);
            fprintf(debug_file, "  Scaling: raw_Ic=%.9e * scale=%.0f = %.6f\n", collector_current, autoTuner_.current_scale, (collector_current * autoTuner_.current_scale));
            fprintf(debug_file, "  Final Output: %.9f (clamped to ±10)\n", output);
            fprintf(debug_file, "  Status: %s\n", (std::isnan(output) ? "NaN!" : std::isinf(output) ? "INF!" : "OK"));
            
            // Test if different scaling values would work better
            if (debug_count <= 5) {
                float test_scales[] = {1000.0f, 10000.0f, 100000.0f, 1000000.0f, 10000000.0f};
                fprintf(debug_file, "  SCALE SWEEP:\n");
                for (int i = 0; i < 5; i++) {
                    float test_output = collector_current * test_scales[i];
                    test_output = juce::jlimit(-10.0f, 10.0f, test_output);
                    fprintf(debug_file, "    scale=%g -> output=%.6f %s\n", 
                           test_scales[i], test_output, 
                           (test_output > 0.01f || test_output < -0.01f) ? "AUDIBLE" : "quiet");
                }
            }
            fprintf(debug_file, "---\n");
            fclose(debug_file);
        }
    }
    
    // Auto-tuning: Test parameters and log results
    if (autoTuner_.enabled && autoTuner_.sample_count % 4800 == 0) {  // Test every 0.1 seconds at 48kHz
        float fuzz_score = calculateFuzzCharacter(output);
        
        if (autoTuner_.test_phase < static_cast<int>(autoTuner_.scale_test_values.size())) {
            // Test scaling values
            autoTuner_.current_scale = autoTuner_.scale_test_values[autoTuner_.test_phase];
            logParameterTest(autoTuner_.current_scale, fuzz_score, "Output_Scale");
            
            if (fuzz_score > autoTuner_.best_fuzz_character) {
                autoTuner_.best_fuzz_character = fuzz_score;
                autoTuner_.best_scale = autoTuner_.current_scale;
            }
            
            autoTuner_.test_phase++;
        } else {
            // Testing complete - log best results
            std::stringstream final_msg;
            final_msg << "=== BEST BJT PARAMETERS FOUND ===" << std::endl;
            final_msg << "Best Output Scale: " << autoTuner_.best_scale << std::endl;
            final_msg << "Best Fuzz Score: " << autoTuner_.best_fuzz_character << std::endl;
            writeAutoTunerLog(final_msg.str());
            autoTuner_.enabled = false;  // Stop testing
        }
    }
    autoTuner_.sample_count++;
    
    return juce::jlimit(-10.0f, 10.0f, output);  // Allow much higher output for fuzz
}

float TerminalCircuit::get_starved_supply_voltage() const {
    // Authentic Terminal/FY-2 battery sag: 7-8V instead of fresh 9V
    float base_voltage = 7.5f;  // Mid-point of starved range
    
    // Add slight variation to simulate aging battery
    float sag_variation = std::sin(static_cast<float>(rand()) / RAND_MAX * 0.01f) * 0.3f;
    
    return juce::jlimit(7.0f, 8.0f, base_voltage + sag_variation);
}

float TerminalCircuit::highPassFilter(float input, float frequency, FilterState& state) {
    if (frequency <= 0.0f) return input;
    
    float rc = 1.0f / (2.0f * juce::MathConstants<float>::pi * frequency);
    float dt = 1.0f / sampleRate_;
    float alpha = rc / (rc + dt);
    
    float output = alpha * (state.y1 + input - state.x1);
    state.x1 = input;
    state.y1 = output;
    
    return output;
}

float TerminalCircuit::lowPassFilter(float input, float frequency, FilterState& state) {
    if (frequency <= 0.0f) return 0.0f;
    
    float rc = 1.0f / (2.0f * juce::MathConstants<float>::pi * frequency);
    float dt = 1.0f / sampleRate_;
    float alpha = dt / (rc + dt);
    
    float output = alpha * input + (1.0f - alpha) * state.y1;
    state.y1 = output;
    
    return output;
}

float TerminalCircuit::getInputLevelDb() const {
    if (inputLevel_.averageLevel <= 0.0f) {
        return -96.0f; // Very quiet, below meter range
    }
    return juce::Decibels::gainToDecibels(inputLevel_.averageLevel);
}

} // namespace DSP
} // namespace TerminalFuzz
