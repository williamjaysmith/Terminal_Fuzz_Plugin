#include "FuzzCircuit.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace TerminalFuzz { namespace DSP {

FuzzCircuit::FuzzCircuit() : sampleRate_(44100.0) {
    reset();
}

void FuzzCircuit::prepare(double sampleRate, int samplesPerBlock) {
    sampleRate_ = sampleRate;
    reset();
}

void FuzzCircuit::processBlock(juce::AudioBuffer<float>& buffer,
                                  float inputGainDb, float fuzzAmount, float levelAmount,
                                  float q1GainDb, float q2GainDb, float q3GainDb,
                                  bool q1Bypass, bool q2Bypass, bool q3Bypass,
                                  const ComponentValues& componentValues) {
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Input gain already applied in PluginProcessor
    juce::ignoreUnused(inputGainDb);
    
    fuzzAmount = juce::jlimit(0.0f, 1.0f, fuzzAmount);
    levelAmount = juce::jlimit(0.0f, 1.0f, levelAmount);

    for (int channel = 0; channel < numChannels && channel < Constants::MAX_CHANNELS; ++channel) {
        float* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample) {
            float inputSample = channelData[sample];
            if (std::isnan(inputSample) || std::isinf(inputSample)) {
                inputSample = 0.0f;
            }
            
            // Input gain already applied in PluginProcessor - no need to apply again
            
            float processedSample = processSample(
                inputSample, channel,
                inputGainDb, fuzzAmount, levelAmount,
                q1GainDb, q2GainDb, q3GainDb,
                q1Bypass, q2Bypass, q3Bypass,
                componentValues
            );
            
            // DEBUG: Track processSample return to buffer write
            static int sample_routing_debug = 0;
            sample_routing_debug++;
            if (sample_routing_debug <= 3) {
                FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
                if (debug_file) {
                    fprintf(debug_file, "+++ SAMPLE ROUTING #%d +++\n", sample_routing_debug);
                    fprintf(debug_file, "processSample() returned: %.6f\n", processedSample);
                    fprintf(debug_file, "Input was: %.6f\n", inputSample);
                    fprintf(debug_file, "Channel: %d, Sample: %d\n", channel, sample);
                    fclose(debug_file);
                }
            }
            
            if (std::isnan(processedSample) || std::isinf(processedSample)) {
                if (sample_routing_debug <= 3) {
                    FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
                    if (debug_file) {
                        fprintf(debug_file, "NaN/INF DETECTED! Setting to 0.0\n");
                        fclose(debug_file);
                    }
                }
                processedSample = 0.0f;
            }
            
            channelData[sample] = processedSample;
            
            // DEBUG: Verify the buffer write actually happened
            if (sample_routing_debug <= 3) {
                FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
                if (debug_file) {
                    fprintf(debug_file, "Buffer write: channelData[%d] = %.6f\n", sample, processedSample);
                    fprintf(debug_file, "Verification: channelData[%d] now reads: %.6f\n", sample, channelData[sample]);
                    fprintf(debug_file, "+++ END SAMPLE ROUTING +++\n");
                    fclose(debug_file);
                }
            }
            
            // DEBUG: Critical buffer write verification
            static int write_debug = 0;
            write_debug++;
            if (write_debug <= 5) {
                FILE* debug_file = fopen("/Users/williamsmith/Desktop/buffer_write_debug.log", "a");
                if (debug_file) {
                    fprintf(debug_file, "=== BUFFER WRITE #%d ===\n", write_debug);
                    fprintf(debug_file, "Writing to channelData[%d] = %.6f\n", sample, processedSample);
                    fprintf(debug_file, "Verification: channelData[%d] now = %.6f\n", sample, channelData[sample]);
                    fprintf(debug_file, "Channel: %d, Sample: %d\n", channel, sample);
                    fclose(debug_file);
                }
            }
        }
    }
    
    // Force stereo output
    if (buffer.getNumChannels() >= 2) {
        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);
        
        // DEBUG: Check what's in left channel before stereo copy
        static int stereo_debug = 0;
        stereo_debug++;
        if (stereo_debug <= 3) {
            FILE* debug_file = fopen("/Users/williamsmith/Desktop/buffer_write_debug.log", "a");
            if (debug_file) {
                fprintf(debug_file, "=== STEREO COPY #%d ===\n", stereo_debug);
                fprintf(debug_file, "Left channel before copy: [0]=%.6f [1]=%.6f [2]=%.6f\n", 
                       leftChannel[0], leftChannel[1], leftChannel[2]);
                fclose(debug_file);
            }
        }
        
        for (int sample = 0; sample < numSamples; ++sample) {
            rightChannel[sample] = leftChannel[sample];
        }
        
        // DEBUG: Verify stereo copy worked
        if (stereo_debug <= 3) {
            FILE* debug_file = fopen("/Users/williamsmith/Desktop/buffer_write_debug.log", "a");
            if (debug_file) {
                fprintf(debug_file, "After stereo copy: Left[0]=%.6f Right[0]=%.6f\n", 
                       leftChannel[0], rightChannel[0]);
                fprintf(debug_file, "=== END STEREO COPY ===\n");
                fclose(debug_file);
            }
        }
    } else {
        // DEBUG: Check mono buffer final state
        static int mono_debug = 0;
        mono_debug++;
        if (mono_debug <= 3) {
            FILE* debug_file = fopen("/Users/williamsmith/Desktop/buffer_write_debug.log", "a");
            if (debug_file) {
                fprintf(debug_file, "=== MONO BUFFER CHECK #%d ===\n", mono_debug);
                fprintf(debug_file, "Mono buffer: [0]=%.6f [1]=%.6f [2]=%.6f\n", 
                       buffer.getSample(0, 0), buffer.getSample(0, 1), buffer.getSample(0, 2));
                fprintf(debug_file, "Buffer channels: %d\n", buffer.getNumChannels());
                fclose(debug_file);
            }
        }
    }
}

void FuzzCircuit::reset() {
    for (auto& state : channelStates_) {
        state = ChannelState{};
    }
}

float FuzzCircuit::processSample(float sample, int channel,
                                   float inputGainDb, float fuzzAmount, float levelAmount,
                                   float q1GainDb, float q2GainDb, float q3GainDb,
                                   bool q1Bypass, bool q2Bypass, bool q3Bypass,
                                   const ComponentValues& cv) {
    if (std::isnan(sample) || std::isinf(sample)) {
        return 0.0f;
    }
    
    // Bypass mode check - DEBUG: Could this be skipping circuit processing?
    if (cv.bypass_mode) {
        static int bypass_debug = 0;
        bypass_debug++;
        if (bypass_debug <= 3) {
            FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
            if (debug_file) {
                fprintf(debug_file, "!!! BYPASS MODE ACTIVE #%d !!!\n", bypass_debug);
                float levelAmount_hardcoded = 1.0f;  // Always 100%
                fprintf(debug_file, "Input: %.6f, Level: %.6f (hardcoded), Output: %.6f\n", 
                       sample, levelAmount_hardcoded, sample * levelAmount_hardcoded);
                fclose(debug_file);
            }
        }
        float levelAmount_hardcoded = 1.0f;  // Always 100%
        return sample * levelAmount_hardcoded;
    }
    
    // C1 input coupling capacitor (100nF) - AC coupling, blocks DC
    float c1_coupled = applyCouplingCapacitor(sample, cv.c1, 20.0f, channel);  // 20Hz cutoff
    
    // Fuzz control - massive gain for broken terminal character
    float fuzzLevel = 10.0f + fuzzAmount * 490.0f;  // 10x to 500x gain range
    float fuzzedSignal = c1_coupled * fuzzLevel;
    
    // Apply Q2 gain control before Q2 stage
    float q2Gain = juce::Decibels::decibelsToGain(q2GainDb);
    float q2Input = fuzzedSignal * q2Gain;
    
    // Q2 Transistor Stage (main fuzz stage) - with bypass
    float q2Output;
    if (q2Bypass) {
        q2Output = q2Input;  // Bypass Q2 - pass signal through unchanged
    } else {
        q2Output = q2TransistorStage(q2Input, channel, q2GainDb, cv);
    }
    
    // DEBUG: Critical Q2→Q3 transition debugging
    static int debug_count = 0;
    debug_count++;
    if (debug_count <= 5) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "=== Q2→Q3 TRANSITION #%d ===\n", debug_count);
            fprintf(debug_file, "Q2 Input: %.6f\n", fuzzedSignal);
            fprintf(debug_file, "Q2 Output: %.6f\n", q2Output);
            fprintf(debug_file, "Q2 Output RMS: %.6f\n", std::abs(q2Output));
            fprintf(debug_file, "Q2 Output > 0.01? %s\n", (std::abs(q2Output) > 0.01f) ? "YES" : "NO");
            fclose(debug_file);
        }
    }
    
    // TONE STACK REMOVED - Direct Q2→Q3 coupling for pure fuzz module character
    
    // Q2→Q3 Gain Buffer (realistic intermediate stage)
    float q2_to_q3_gain = 2.0f;   // Reduced from 15x to 2x to prevent Q3 overload
    float q3_buffer_input = q2Output * q2_to_q3_gain;
    
    // Add DC bias to Q3 input with slight misbias for vintage character
    float q3_optimal_bias = 0.1f;
    float q3_misbias_offset = 0.02f;  // Slightly over-biased (different from Q2)
    float q3_dc_bias = q3_optimal_bias + q3_misbias_offset;
    float q3_final_input = q3_buffer_input + q3_dc_bias;
    
    // Apply Q3 gain control before Q3 stage
    float q3Gain = juce::Decibels::decibelsToGain(q3GainDb);
    float q3Input = q3_final_input * q3Gain;
    
    // Q3 Transistor Stage (second gain stage) - with bypass
    float q3Output;
    if (q3Bypass) {
        q3Output = q3Input;  // Bypass Q3 - pass signal through unchanged
    } else {
        q3Output = q3TransistorStage(q3Input, channel, q3GainDb, cv);
    }
    
    // Q3→Q1 Buffer (output stage coupling)
    float q3_to_q1_gain = 1.0f;  // 1x gain - Q1 is just output buffer
    float q1_input = q3Output * q3_to_q1_gain;
    
    // Apply Q1 gain control before Q1 stage
    float q1Gain = juce::Decibels::decibelsToGain(q1GainDb);
    float q1Input = q1_input * q1Gain;
    
    // Q1 Transistor Stage (output buffer) - 2N3904 - with bypass
    float q1Output;
    if (q1Bypass) {
        q1Output = q1Input;  // Bypass Q1 - pass signal through unchanged
    } else {
        q1Output = q1OutputStage(q1Input, channel, q1GainDb, cv);
    }
    
    // DEBUG: Q1 buffer stage analysis - IS THIS WHERE WE'RE LOSING SIGNAL?
    if (debug_count <= 5) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "=== Q1 OUTPUT BUFFER STAGE ===\n");
            fprintf(debug_file, "Q1 Input: %.6f\n", q1_input);
            fprintf(debug_file, "Q1 Output: %.6f\n", q1Output);
            fprintf(debug_file, "Q1 Output > 0.01? %s\n", (std::abs(q1Output) > 0.01f) ? "YES" : "NO - PROBLEM!");
            fclose(debug_file);
        }
    }
    
    // DEBUG: Q2→Q3 Buffer analysis
    if (debug_count <= 5) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "=== Q2→Q3 BUFFER STAGE ===\n");
            fprintf(debug_file, "Q2 Output (raw): %.6f\n", q2Output);
            fprintf(debug_file, "Q2→Q3 Gain: %.1fx\n", q2_to_q3_gain);
            fprintf(debug_file, "Q3 Buffer Input: %.6f\n", q3_buffer_input);
            fprintf(debug_file, "Q3 DC Bias: %.6f\n", q3_dc_bias);
            fprintf(debug_file, "Q3 Final Input: %.6f\n", q3_final_input);
            fprintf(debug_file, "Q3 Output: %.6f\n", q3Output);
            fprintf(debug_file, "Q3 Output RMS: %.6f\n", std::abs(q3Output));
            fprintf(debug_file, "Q3 Output > 0.01? %s\n", (std::abs(q3Output) > 0.01f) ? "YES" : "NO");
            fclose(debug_file);
        }
    }
    
    // C8 and C9 output coupling capacitors (10μF each) - Stage 1 and 2
    float c8_coupled = applyCouplingCapacitor(q1Output, cv.c8, 5.0f, channel);  // 5Hz cutoff for low end
    float c9_coupled = applyCouplingCapacitor(c8_coupled, cv.c9, 5.0f, channel);  // Double coupling for extra low end
    
    // Level control hardcoded to 100% (1.0f) - removed from user interface
    float levelAmount_hardcoded = 1.0f;  // Always at 100%
    float outputScale = 5.0f;  // FIXED: Increased from 0.01f to 5.0f for audible levels
    float finalOutput = c9_coupled * levelAmount_hardcoded * outputScale;
    
    // DEBUG: Final output check and buffer routing verification + NaN/Inf checking
    if (debug_count <= 5) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "=== FINAL OUTPUT ROUTING ===\n");
            fprintf(debug_file, "Q3 Output: %.6f\n", q3Output);
            fprintf(debug_file, "Q3 Output NaN? %s\n", std::isnan(q3Output) ? "YES - PROBLEM!" : "NO");
            fprintf(debug_file, "Q3 Output Inf? %s\n", std::isinf(q3Output) ? "YES - PROBLEM!" : "NO");
            fprintf(debug_file, "Level Amount: %.6f (hardcoded to 100%%)\n", levelAmount_hardcoded);
            fprintf(debug_file, "Level Amount NaN? %s\n", std::isnan(levelAmount_hardcoded) ? "YES - PROBLEM!" : "NO");
            fprintf(debug_file, "Level Boost: 10.0x\n");
            fprintf(debug_file, "Final Output (before clamp): %.6f\n", finalOutput);
            fprintf(debug_file, "Final Output NaN? %s\n", std::isnan(finalOutput) ? "YES - PROBLEM!" : "NO");
            fprintf(debug_file, "Final Output Inf? %s\n", std::isinf(finalOutput) ? "YES - PROBLEM!" : "NO");
            
            // Test the limiting - REMOVED: was killing fuzz signal at ±0.2V
            float clampedOutput = finalOutput;  // No limiting in debug - let it be loud!
            fprintf(debug_file, "Final Output (after clamp): %.6f\n", clampedOutput);
            fprintf(debug_file, "Clamped Output NaN? %s\n", std::isnan(clampedOutput) ? "YES - PROBLEM!" : "NO");
            fprintf(debug_file, "Clamping Applied? %s\n", (clampedOutput != finalOutput) ? "YES" : "NO");
            fprintf(debug_file, "Will Return to Buffer: %.6f\n", clampedOutput);
            fprintf(debug_file, "=== END TRANSITION ===\n\n");
            fclose(debug_file);
        }
    }
    
    // Allow proper fuzz range - no limiting for broken terminal character
    // Let the fuzz be loud and broken like the original pedal!
    float processedOutput = finalOutput;
    
    // ADDITIONAL DEBUG: Show exactly what gets returned
    static int return_debug = 0;
    return_debug++;
    if (return_debug <= 3) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "*** PROCESSSAMPLE RETURN #%d: %.6f ***\n", return_debug, processedOutput);
            fclose(debug_file);
        }
    }
    
    return processedOutput;
}

float FuzzCircuit::q2TransistorStage(float sample, int channel, float q2GainDb, const ComponentValues& cv) {
    static int q2_debug_count = 0;
    q2_debug_count++;
    
    // Simple bias for Q2 with slight misbias for vintage character
    float optimal_bias = 0.05f;
    float misbias_offset = -0.015f;  // Slightly under-biased for instability
    float total_bias = optimal_bias + misbias_offset;
    float adjusted_input = sample + total_bias;
    
    // DEBUG: Q2 input analysis
    if (q2_debug_count <= 5) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "--- Q2 STAGE ANALYSIS #%d ---\n", q2_debug_count);
            fprintf(debug_file, "Q2 Raw Input: %.6f\n", sample);
            fprintf(debug_file, "Q2 Bias: %.6f\n", total_bias);
            fprintf(debug_file, "Q2 Adjusted Input: %.6f\n", adjusted_input);
            fclose(debug_file);
        }
    }
    
    // Q2 BJT physics - 2N2369 with exact schematic values
    float vce_estimate = get_starved_supply_voltage() * 0.5f;
    float emitter_resistor = 0.0f;     // Q2 has grounded emitter (no resistor)
    float r2_collector = 22000.0f;     // R2: 22kΩ collector load
    float r4_bias = 47000.0f;          // R4: 47kΩ base bias
    
    // DEBUG: Q2 BJT parameters
    if (q2_debug_count <= 5) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "Q2 VCE Estimate: %.6f\n", vce_estimate);
            fprintf(debug_file, "Q2 Supply: %.6f\n", get_starved_supply_voltage());
            fprintf(debug_file, "Q2 Model VBE_ON: %.6f\n", cv.q2_model.vbe_on);
            fprintf(debug_file, "Q2 Model hFE: %.3f\n", cv.q2_model.hfe);
            fclose(debug_file);
        }
    }
    
    float collector_current = bjt_ebers_moll(adjusted_input, vce_estimate, cv.q2_model, 
                                           get_starved_supply_voltage(), emitter_resistor, 
                                           r2_collector, r4_bias);
    
    // DEBUG: Q2 output verification
    if (q2_debug_count <= 5) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "Q2 BJT Raw Output: %.6f\n", collector_current);
            fprintf(debug_file, "Q2 Output Valid? %s\n", 
                   (std::isnan(collector_current) || std::isinf(collector_current)) ? "NO - NaN/INF!" : "YES");
            fprintf(debug_file, "Q2 Output Zero? %s\n", (collector_current == 0.0f) ? "YES - PROBLEM!" : "NO");
            fprintf(debug_file, "--- END Q2 ANALYSIS ---\n");
            fclose(debug_file);
        }
    }
    
    return collector_current;
}

float FuzzCircuit::q3TransistorStage(float sample, int channel, float q3GainDb, const ComponentValues& cv) {
    static int q3_debug_count = 0;
    q3_debug_count++;
    
    // Q3 BJT physics - 2N2369 with exact schematic values
    float vce_estimate = get_starved_supply_voltage() * 0.4f;
    float emitter_resistor = 0.0f;     // Q3 has grounded emitter (no resistor)
    float r7_collector = 47000.0f;     // R7: 47kΩ collector load
    float r8_bias = 470000.0f;         // R8: 470kΩ base bias
    
    // DEBUG: Q3 bias and input analysis - DETAILED VBE CHECKING
    if (q3_debug_count <= 5) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "--- Q3 BIAS ANALYSIS #%d ---\n", q3_debug_count);
            fprintf(debug_file, "Q3 Input Sample: %.6f\n", sample);
            fprintf(debug_file, "Q3 VCE Estimate: %.6f\n", vce_estimate);
            fprintf(debug_file, "Q3 Supply Voltage: %.6f\n", get_starved_supply_voltage());
            fprintf(debug_file, "Q3 Model VBE_ON: %.6f\n", cv.q3_model.vbe_on);
            fprintf(debug_file, "Q3 Model hFE: %.3f\n", cv.q3_model.hfe);
            
            // Detailed base-emitter voltage analysis
            float vbe_scaling = 0.002f;  // Same as in BJT function
            float vbe_input_factor = sample * vbe_scaling;
            float expected_vbe = cv.q3_model.vbe_on + vbe_input_factor;
            float emitter_voltage = 0.0f;  // Assuming grounded emitter for now
            float base_voltage = expected_vbe + emitter_voltage;
            float vbe_differential = expected_vbe - emitter_voltage;
            
            fprintf(debug_file, "=== Q3 VBE DETAILED ANALYSIS ===\n");
            fprintf(debug_file, "VBE Input Factor: %.6f\n", vbe_input_factor);
            fprintf(debug_file, "Expected VBE: %.6f\n", expected_vbe);
            fprintf(debug_file, "Base Voltage: %.6f\n", base_voltage);
            fprintf(debug_file, "Emitter Voltage: %.6f\n", emitter_voltage);
            fprintf(debug_file, "VBE Differential: %.6f\n", vbe_differential);
            fprintf(debug_file, "Forward Biased (>0.6V)? %s\n", (vbe_differential > 0.6f) ? "YES" : "NO - PROBLEM!");
            fprintf(debug_file, "Forward Biased (>0.7V)? %s\n", (vbe_differential > 0.7f) ? "YES - STRONG" : "WEAK");
            fclose(debug_file);
        }
    }
    
    float output = bjt_ebers_moll(sample, vce_estimate, cv.q3_model, 
                                 get_starved_supply_voltage(), emitter_resistor, 
                                 r7_collector, r8_bias);
    
    // DEBUG: Q3 output verification
    if (q3_debug_count <= 5) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "Q3 BJT Output: %.6f\n", output);
            fprintf(debug_file, "Q3 Output Valid? %s\n", 
                   (std::isnan(output) || std::isinf(output)) ? "NO - NaN/INF!" : "YES");
            fprintf(debug_file, "--- END Q3 ANALYSIS ---\n");
            fclose(debug_file);
        }
    }
    
    return output;
}

float FuzzCircuit::q1OutputStage(float sample, int channel, float q1GainDb, const ComponentValues& cv) {
    // Q1 is 2N3904 output buffer - lower gain, stable operation
    float vce_estimate = get_starved_supply_voltage() * 0.6f;  // 4.5V (higher for buffer)
    float r9_emitter = 470.0f;      // Shared emitter resistor
    float r11_collector = 100000.0f; // 100kΩ collector load
    float r10_bias = 10000.0f;      // 10kΩ base bias
    
    // FIXED: Q1 should be a unity gain buffer, not a high-gain amplifier
    // Simple emitter follower behavior for output buffering
    float buffer_gain = 0.9f;  // Slightly less than unity (typical emitter follower)
    float buffered_output = sample * buffer_gain;
    
    return buffered_output;
}

float FuzzCircuit::get_starved_supply_voltage() const {
    return 7.5f;  // Starved 9V supply
}


float FuzzCircuit::bjt_ebers_moll(float vin, float vce, const TransistorModel& model,
                                     float supply_voltage, float emitter_resistor, 
                                     float collector_resistor, float base_bias_resistor) {
    const float VT = 0.026f;  // Thermal voltage at room temperature
    const float MIN_VOLTAGE = 1e-12f;
    
    // Remove shared static state - each transistor should be independent
    // Debug info will be handled by calling functions (Q1, Q2, Q3)
    
    // VBE calculation with realistic scaling for large signals
    // Real BJTs: VBE never exceeds ~0.8V, regardless of input signal strength
    // Use logarithmic scaling to map large signals to realistic VBE range
    float vbe_base = model.vbe_on;  // Base VBE (0.66V for Q2, 0.70V for Q3)
    float signal_factor = std::abs(vin);
    
    // FIXED: More gradual VBE response - don't hit ceiling immediately
    float vbe_max = vbe_base + 0.15f;  // Reduced ceiling to allow more variation
    float log_scale = 2.0f;  // FIXED: Less aggressive scaling for more input sensitivity
    float vbe_delta = 0.15f * (1.0f - std::exp(-signal_factor / log_scale));  // Match reduced vbe_max
    
    // Apply input polarity with asymmetric bias drift - positive/negative bias differently
    float positive_bias_scale = 1.0f;    // Normal positive response
    float negative_bias_scale = 0.65f;   // Much stronger asymmetry - negative signals bias much less
    
    float vbe_polarity;
    if (vin >= 0.0f) {
        vbe_polarity = positive_bias_scale;
    } else {
        vbe_polarity = -negative_bias_scale;  // Less negative bias = asymmetry without flutter
    }
    
    float vbe = vbe_base + (vbe_delta * vbe_polarity);
    
    // Ensure VBE stays within physically possible range
    vbe = juce::jlimit(0.3f, vbe_max, vbe);
    
    // DEBUG: VBE calculation step by step
    static int vbe_debug_count = 0;
    vbe_debug_count++;
    if (vbe_debug_count <= 10) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "*** VBE DEBUG #%d ***\n", vbe_debug_count);
            fprintf(debug_file, "Input vin: %.6f\n", vin);
            fprintf(debug_file, "signal_factor: %.6f\n", signal_factor);
            fprintf(debug_file, "vbe_delta: %.6f\n", vbe_delta);
            fprintf(debug_file, "vbe_polarity: %.6f\n", vbe_polarity);
            fprintf(debug_file, "vbe_base: %.6f\n", vbe_base);
            fprintf(debug_file, "vbe (final): %.6f\n", vbe);
            fclose(debug_file);
        }
    }
    
    // BJT calculations are now independent for each transistor
    // Debug handled by individual transistor stages (Q1, Q2, Q3)
    
    // Base current using Shockley equation
    float exp_arg_raw = vbe / VT;
    // REMOVED: Exponential clamping was making all transistors identical
    // float exp_arg = (exp_arg_raw > 20.0f) ? 20.0f : exp_arg_raw;
    float exp_arg = exp_arg_raw;  // Let it go wild for broken fuzz character
    float exp_result = std::exp(exp_arg);
    float base_current_raw = model.is * (exp_result - 1.0f);
    float base_current = juce::jlimit(MIN_VOLTAGE, 10.0f, base_current_raw);  // FIXED: Allow much higher base current for fuzz
    
    // DEBUG: Base current calculation
    if (vbe_debug_count <= 10) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "exp_arg: %.6f\n", exp_arg);
            fprintf(debug_file, "exp_result: %.6f\n", exp_result);
            fprintf(debug_file, "base_current_raw: %.6f\n", base_current_raw);
            fprintf(debug_file, "base_current (limited): %.6f\n", base_current);
            fclose(debug_file);
        }
    }
    
    // Base current calculations - independent per transistor
    
    // Collector current with Early effect + GAIN VARIATION for vintage character
    float current_hfe_scale = 1.0f;
    
    // Add ±10% gain variation based on signal level - creates vintage transistor character
    float signal_level_factor = std::abs(vin) * 0.01f;  // Convert signal to 0-1 range
    float gain_variation = 1.0f + (std::sin(signal_level_factor) * 0.1f);  // ±10% variation
    
    // Add collector current sag - heavy current draw causes supply voltage drop
    float current_estimate = base_current * model.hfe * gain_variation;
    float supply_sag_factor = 1.0f;
    if (current_estimate > 0.005f) {  // When drawing >5mA
        float sag_amount = (current_estimate - 0.005f) * 200.0f;  // Sag increases with current
        supply_sag_factor = 1.0f - juce::jlimit(0.0f, 0.25f, sag_amount);  // Max 25% voltage sag
    }
    
    float dynamic_hfe_initial = model.hfe * current_hfe_scale * gain_variation * supply_sag_factor;
    float collector_estimate = base_current * dynamic_hfe_initial;
    
    // REMOVED: hFE rolloff was making all transistors behave identically
    // float dynamic_hfe_final = dynamic_hfe_initial;
    // if (collector_estimate > 0.01f) {
    //     dynamic_hfe_final = dynamic_hfe_initial * (0.01f / collector_estimate);
    // }
    float dynamic_hfe_final = dynamic_hfe_initial;  // Keep full hFE for broken fuzz character
    
    float early_factor = 1.0f + (vce / 100.0f);
    float collector_current = dynamic_hfe_final * base_current * early_factor;
    
    // DEBUG: Collector current calculation
    if (vbe_debug_count <= 10) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "dynamic_hfe_final: %.6f\n", dynamic_hfe_final);
            fprintf(debug_file, "early_factor: %.6f\n", early_factor);
            fprintf(debug_file, "collector_current: %.6f\n", collector_current);
            fclose(debug_file);
        }
    }
    
    // Collector current calculations - independent per transistor
    
    // Voltage calculations
    float vcc = supply_voltage;
    float collector_voltage_drop = collector_current * collector_resistor;
    float actual_vce_raw = vcc - collector_voltage_drop;
    float actual_vce = juce::jmax(model.vce_sat, actual_vce_raw);
    
    float emitter_voltage_drop = collector_current * emitter_resistor;
    float emitter_feedback = emitter_voltage_drop * 0.1f;
    
    // Output scaling - BALANCE FIX: Find sweet spot between too loud and too quiet
    float current_scale = 0.5f;  // Increased from 0.001f to 0.5f for audible distortion
    float output_raw = collector_current * current_scale;
    float output = output_raw - emitter_feedback;
    
    // VOLTAGE-BASED HARD CLIPPING WITH OVERDRIVE (fuzz pedal physics)
    // Calculate collector voltage first, then allow overdrive beyond supply rails
    
    // Calculate collector voltage: Vcc - (Ic * Rc)
    float collector_voltage = vcc - (collector_current * collector_resistor);
    
    // REMOVED: Artificial voltage limits - let BJT physics create natural distortion!
    // Real transistors distort through exponential VBE curves and saturation behavior
    /*
    // REALISTIC BJT voltage limits (actual supply rails)
    // Real transistors can't exceed these physical limits
    float max_collector_voltage = vcc;              // Positive supply rail (7.5V)
    float min_collector_voltage = model.vce_sat;    // VCE saturation (~0.2V)
    
    // HARD CLIP collector voltage with overdrive allowance
    float clipped_collector_voltage = collector_voltage;
    if (collector_voltage > max_collector_voltage) {
        clipped_collector_voltage = max_collector_voltage;
    } else if (collector_voltage < min_collector_voltage) {
        clipped_collector_voltage = min_collector_voltage;
    }
    
    // Apply voltage clipping by converting clipped voltage back to collector current
    // This preserves input variation while hard clipping at voltage rails
    float clipped_collector_current;
    if (collector_voltage != clipped_collector_voltage) {
        // Voltage was clipped - calculate corresponding collector current
        clipped_collector_current = (vcc - clipped_collector_voltage) / collector_resistor;
    } else {
        // No clipping - use original collector current
        clipped_collector_current = collector_current;
    }
    
    // Use realistic voltage clipping (fixed supply rail limits)
    output = clipped_collector_current * current_scale - emitter_feedback;
    */
    
    // USE NATURAL BJT PHYSICS - let transistors CLIP through their exponential curves!
    
    // DEBUG: Final output calculation
    if (vbe_debug_count <= 10) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/q2_q3_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "collector_current (natural): %.6f\n", collector_current);
            fprintf(debug_file, "current_scale: %.6f\n", current_scale);
            fprintf(debug_file, "emitter_feedback: %.6f\n", emitter_feedback);
            fprintf(debug_file, "output (final): %.6f\n", output);
            fprintf(debug_file, "*** END VBE DEBUG ***\n");
            fclose(debug_file);
        }
    }
    
    // Saturation behavior is now handled in voltage clipping above
    // Add asymmetric saturation character when voltage hits limits
    if (collector_voltage <= model.vce_sat) {
        // In saturation - add buzzy, compressed character
        if (output > 0.0f) {
            output *= 1.2f;  // Positive peaks saturate harder
        } else {
            output *= 0.8f;  // Negative peaks compress more
        }
    }
    
    // Output calculations - independent per transistor
    
    // REMOVED: Hard limiting was killing Q3 output variation
    // return juce::jlimit(-10.0f, 10.0f, output);
    return output;
}

// TONE STACK FUNCTION REMOVED - Direct Q2→Q3 coupling for pure fuzz character

float FuzzCircuit::applyCouplingCapacitor(float input, float capacitance, float cutoff_frequency, int channel) {
    // Simple first-order high-pass filter implementation for coupling capacitors
    // Cutoff frequency = 1 / (2π * R * C)
    // For coupling caps, the series resistance is typically 10kΩ to 100kΩ
    
    if (channel >= Constants::MAX_CHANNELS) channel = 0;
    
    // Calculate alpha for high-pass filter
    float rc = 1.0f / (2.0f * M_PI * cutoff_frequency);
    float dt = 1.0f / static_cast<float>(sampleRate_);
    float alpha = rc / (rc + dt);
    
    // Apply high-pass filter (removes DC, preserves AC)
    // Using a simple state variable to track the filter
    static float filter_state[Constants::MAX_CHANNELS] = {0.0f};
    static float prev_input[Constants::MAX_CHANNELS] = {0.0f};
    
    float output = alpha * (filter_state[channel] + input - prev_input[channel]);
    
    // Update state
    filter_state[channel] = output;
    prev_input[channel] = input;
    
    return output;
}

}} // namespace TerminalFuzz::DSP