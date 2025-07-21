#include "OctaveModule.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace TerminalFuzz { namespace DSP {

OctaveModule::OctaveModule() : sampleRate_(44100.0) {
    reset();
}

void OctaveModule::prepare(double sampleRate, int samplesPerBlock) {
    juce::ignoreUnused(samplesPerBlock);
    sampleRate_ = sampleRate;
    reset();
}

void OctaveModule::processBlock(juce::AudioBuffer<float>& buffer, float octaveAmount, 
                                float q1Gain, float q2Gain, float q3Gain, float outputVolume) {
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // DEBUG: Log octave amount
    static int debug_count = 0;
    debug_count++;
    if (debug_count <= 10 || (debug_count % 1000 == 0)) {
        FILE* debug_file = fopen("/tmp/octave_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "OCTAVE DEBUG #%d: octaveAmount = %.3f\n", debug_count, octaveAmount);
            fclose(debug_file);
        }
    }
    
    // Clamp octave amount to valid range
    octaveAmount = juce::jlimit(0.0f, 1.0f, octaveAmount);
    
    for (int channel = 0; channel < numChannels && channel < Constants::MAX_CHANNELS; ++channel) {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample) {
            float inputSample = channelData[sample];
            
            if (std::isnan(inputSample) || std::isinf(inputSample)) {
                inputSample = 0.0f;
            }
            
            float processedSample = processSample(inputSample, channel, octaveAmount, q1Gain, q2Gain, q3Gain, outputVolume);
            
            if (std::isnan(processedSample) || std::isinf(processedSample)) {
                processedSample = 0.0f;
            }
            
            channelData[sample] = processedSample;
        }
    }
    
    // Force stereo output (mono to stereo conversion)
    if (buffer.getNumChannels() >= 2) {
        float* leftChannel = buffer.getWritePointer(0);
        float* rightChannel = buffer.getWritePointer(1);
        
        for (int sample = 0; sample < numSamples; ++sample) {
            rightChannel[sample] = leftChannel[sample];
        }
    }
}

void OctaveModule::reset() {
    for (auto& state : channelStates_) {
        state = ChannelState{};
    }
}

float OctaveModule::processSample(float sample, int channel, float octaveAmount, 
                                  float q1Gain, float q2Gain, float q3Gain, float outputVolume) {
    // COMPREHENSIVE DEBUG LOGGING
    static int process_count = 0;
    process_count++;
    bool should_log = (process_count <= 10) || (process_count % 1000 == 0);
    
    FILE* debug_file = nullptr;
    if (should_log) {
        debug_file = fopen("/tmp/octave_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "\n=== OCTAVE PROCESS #%d ===\n", process_count);
            fprintf(debug_file, "INPUT: sample=%.6f, octaveAmount=%.3f\n", sample, octaveAmount);
            fprintf(debug_file, "GAINS: Q1=%.1fdB, Q2=%.1fdB, Q3=%.1fdB\n", q1Gain, q2Gain, q3Gain);
        }
    }
    
    if (octaveAmount < 0.005f) {  // Lower threshold for bypass
        if (process_count <= 5) {
            FILE* debug_file = fopen("/tmp/octave_debug.log", "a");
            if (debug_file) {
                fprintf(debug_file, "BYPASSING - octaveAmount too low: %.3f\n", octaveAmount);
                fclose(debug_file);
            }
        }
        return sample;  // Bypass if octave amount is negligible
    }
    
    // CLEAN MODERN OCTAVE GENERATION - No circuit modeling, pure DSP
    
    // Use raw input signal 
    float clean_input = sample;
    
    if (should_log && debug_file) {
        fprintf(debug_file, "STEP 1 - Clean input: %.6f\n", clean_input);
    }
    
    // SIMPLE STRONG OCTAVE - Proven approach that works
    
    auto& state = channelStates_[channel];
    
    // Method 1: Full-wave rectification (classic octave foundation)
    float rectified = std::abs(clean_input);
    
    // Method 2: Frequency doubling through controlled clipping
    // This creates stronger harmonics than simple rectification
    float clipped = clean_input;
    if (clipped > 0.3f) clipped = 0.3f + (clipped - 0.3f) * 0.1f;
    if (clipped < -0.3f) clipped = -0.3f + (clipped + 0.3f) * 0.1f;
    float clipped_rect = std::abs(clipped);
    
    // Method 3: Zero-crossing emphasis for cleaner octave
    float zero_cross_boost = 1.0f;
    if (std::abs(clean_input) < 0.1f) {
        zero_cross_boost = 2.0f;  // Boost signal near zero crossings
    }
    
    // Combine methods for strong, clean octave
    float octave_raw = rectified * 2.0f * zero_cross_boost +     // Boosted rectification  
                      clipped_rect * 3.0f;                       // Clipped harmonics
    
    // Normalize and prevent excessive levels
    octave_raw = std::tanh(octave_raw * 0.5f);
    
    state.lastSample = clean_input;
    
    // Apply single gain control (limit to reasonable range)
    float octave_gain_db = juce::jlimit(-20.0f, 20.0f, q1Gain);  // ±20dB max
    float octave_gain_linear = std::pow(10.0f, octave_gain_db / 20.0f);
    
    // Frequency-dependent emphasis (higher frequencies = stronger octave)
    float freq_content = detectFrequencyContent(clean_input, channel);
    float freq_multiplier = 1.0f + (freq_content * 0.5f);  // Up to 1.5x boost for high freq
    
    // Final octave signal with controlled gain and frequency emphasis
    float combined_octave = octave_raw * octave_gain_linear * freq_multiplier;
    
    if (should_log && debug_file) {
        fprintf(debug_file, "STEP 2 - Optimized octave: raw=%.6f -> gained=%.6f\n", 
               octave_raw, combined_octave);
    }
    
    // Final clean octave signal
    float octave_signal = combined_octave;
    
    if (should_log && debug_file) {
        fprintf(debug_file, "STEP 3 - Final octave signal: %.6f\n", octave_signal);
    }
    
    // Final safety check on octave signal
    if (std::isnan(octave_signal) || std::isinf(octave_signal)) {
        octave_signal = 0.0f;
    }
    octave_signal = juce::jlimit(-1.0f, 1.0f, octave_signal);  // Hard limit to prevent clipping
    
    // Clean mix with proper levels - no crazy gain needed
    float mixed_signal = sample * (1.0f - octaveAmount) + (octave_signal * octaveAmount);
    
    if (should_log && debug_file) {
        fprintf(debug_file, "STEP 10 - Final mix: dry=%.6f, octave=%.6f, amount=%.3f -> %.6f\n", 
               sample, octave_signal, octaveAmount, mixed_signal);
    }
    
    // Final safety check
    if (std::isnan(mixed_signal) || std::isinf(mixed_signal)) {
        mixed_signal = sample;  // Fall back to dry signal
    }
    mixed_signal = juce::jlimit(-1.0f, 1.0f, mixed_signal);  // Hard limit
    
    // Apply clean octave output volume control AFTER mixing (pure makeup gain)
    float outputVolumeLinear = std::pow(10.0f, outputVolume / 20.0f);  // dB to linear
    float output = mixed_signal * outputVolumeLinear;
    
    if (should_log && debug_file) {
        fprintf(debug_file, "FINAL OUTPUT: %.6f\n", output);
        fclose(debug_file);
    }
    
    // Final output safety check
    if (std::isnan(output) || std::isinf(output)) {
        output = 0.0f;
    }
    output = juce::jlimit(-1.0f, 1.0f, output);
    
    return output;
}

float OctaveModule::generateOctaveHarmonics(float sample, int channel, float intensity) {
    // GREEN RINGER TOPOLOGY - Authentic Ring Modulation Octave Generation
    // Based on Dan Armstrong Green Ringer / EQD Tentacle circuit analysis
    
    // Stage 1: Q1 (2N5089 NPN) - High-gain input amplifier
    float q1_output = processQ1Stage(sample, channel);
    
    // Stage 2: Q2 (2N3906 PNP) - Ring modulator stage
    float q2_output = processQ2Stage(q1_output, channel);
    
    // Ring modulation: Mix NPN and PNP outputs to create sum/difference frequencies
    float ring_mod = (q1_output * 0.7f) + (q2_output * -0.5f);  // PNP inverts
    
    // Stage 3: Q3 (2N5089 NPN) - Output buffer
    float octave_signal = processQ3Stage(ring_mod, channel);
    
    // Apply intensity scaling
    octave_signal *= intensity;
    
    // Final limiting for authentic character
    octave_signal = std::tanh(octave_signal * 2.0f) * 0.6f;
    
    return octave_signal;
}

float OctaveModule::applyWaveRectification(float sample, float amount) {
    // Full-wave rectification creates strong even harmonics
    float rectified = std::abs(sample);
    
    // Preserve original polarity with controlled rectification
    float sign = (sample >= 0.0f) ? 1.0f : -1.0f;
    float mixed = sample + (rectified * amount * sign * 0.6f);
    
    return mixed;
}

float OctaveModule::detectFrequencyContent(float sample, int channel) {
    auto& state = channelStates_[channel];
    
    // Simple frequency detector using high-frequency emphasis
    float freq_response = std::abs(sample - state.freq_detector);
    state.freq_detector = sample * 0.1f + state.freq_detector * 0.9f;
    
    // Smooth the frequency response
    state.rectify_smooth = freq_response * 0.05f + state.rectify_smooth * 0.95f;
    
    // Higher frequencies return higher values (0.0 to 1.0)
    float freq_intensity = juce::jlimit(0.0f, 1.0f, state.rectify_smooth * 8.0f);
    
    return freq_intensity;
}

float OctaveModule::highPassFilter(float input, int channel) {
    auto& state = channelStates_[channel];
    
    // High-pass filter at ~80Hz - preserve guitar low end while removing DC/rumble
    float cutoff_freq = 80.0f;
    float rc = 1.0f / (2.0f * static_cast<float>(M_PI) * cutoff_freq);
    float dt = 1.0f / static_cast<float>(sampleRate_);
    float alpha = rc / (rc + dt);
    
    float output = alpha * (state.hp_y1 + input - state.hp_x1);
    
    state.hp_x1 = input;
    state.hp_y1 = output;
    
    // Gentle presence boost to help octave tracking without thinning tone
    float presence_boost = 1.0f + (std::abs(output) * 0.1f);
    output *= presence_boost;
    
    return output;
}

float OctaveModule::dcBlocker(float input, int channel) {
    auto& state = channelStates_[channel];
    
    // DC blocking filter
    float alpha = 0.995f;  // High-pass at ~4Hz
    float output = input - state.dc_x1 + alpha * state.dc_y1;
    
    state.dc_x1 = input;
    state.dc_y1 = output;
    
    return output;
}

float OctaveModule::processQ1Stage(float input, int channel) {
    auto& state = channelStates_[channel];
    
    // Q1: 2N5089 NPN with high gain for strong signal (Green Ringer reference path)
    float amplified = input * 3.5f;  // Higher gain for pronounced ring mod
    
    // Soft saturation to create harmonics (analog behavior)
    float saturated = std::tanh(amplified * 1.2f) * 0.8f;
    
    // Store state for debugging
    state.q1_collector = saturated;
    state.q1_base = input;
    
    return saturated;
}

float OctaveModule::processQ2Stage(float input, int channel) {
    auto& state = channelStates_[channel];
    
    // Q2: 2N3906 PNP with different saturation characteristics (creates phase difference)
    float amplified = input * 2.8f;  // Different gain than Q1 for asymmetry
    
    // Harder saturation with different curve (PNP vs NPN characteristics)
    float saturated = std::tanh(amplified * 1.8f) * 0.6f;
    
    // Add subtle phase shift through simple delay
    float delayed = (saturated + state.q2_base) * 0.5f;
    state.q2_base = saturated * 0.1f + state.q2_base * 0.9f;  // Low-pass delay
    
    // Store state for debugging
    state.q2_collector = delayed;
    
    return delayed;
}

float OctaveModule::processQ3Stage(float input, int channel) {
    auto& state = channelStates_[channel];
    
    // Q3: Clean output buffer
    float buffered = input * 0.9f;  // Unity gain buffer
    
    // Store state for debugging
    state.q3_collector = buffered;
    state.q3_base = input;
    
    return buffered;
}

float OctaveModule::applyFrequencySelectiveEmphasis(float input, int channel) {
    auto& state = channelStates_[channel];
    
    // TENTACLE FREQUENCY CHARACTERISTIC
    // Works best with neck pickup and playing above 12th fret
    // Emphasize frequencies around 400-2000Hz (guitar fundamentals)
    
    // High-pass filter at ~300Hz to remove low-end mud
    float cutoff_freq = 300.0f;
    float rc = 1.0f / (2.0f * static_cast<float>(M_PI) * cutoff_freq);
    float dt = 1.0f / static_cast<float>(sampleRate_);
    float alpha = rc / (rc + dt);
    
    // High-pass filtering
    float hp_output = alpha * (state.freq_emphasis_y1 + input - state.freq_emphasis_x1);
    
    // Update filter state
    state.freq_emphasis_x1 = input;
    state.freq_emphasis_y1 = hp_output;
    
    // Add presence boost around 1-2kHz (where octave tracking works best)
    float presence_freq = 1500.0f;
    float presence_q = 2.0f;
    
    // Simple resonant peak
    float omega = 2.0f * static_cast<float>(M_PI) * presence_freq / static_cast<float>(sampleRate_);
    float sin_omega = std::sin(omega);
    float cos_omega = std::cos(omega);
    float resonance = sin_omega / (2.0f * presence_q);
    
    // Boost presence frequencies
    float presence_boost = 1.0f + (resonance * std::abs(hp_output) * 0.5f);
    
    // Apply frequency-selective emphasis
    float emphasized = hp_output * presence_boost;
    
    // Soft limiting to prevent overload
    emphasized = std::tanh(emphasized * 1.1f) * 0.9f;
    
    return emphasized;
}

}}  // namespace TerminalFuzz::DSP