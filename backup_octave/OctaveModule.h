#pragma once

#include <JuceHeader.h>
#include "Common.h"

namespace TerminalFuzz {
namespace DSP {

/**
 * @brief Octave Module - Authentic octave fuzz harmonic generation
 * 
 * This module implements classic octave fuzz characteristics where higher 
 * frequencies generate more prominent octave harmonics through wave rectification
 * and frequency-dependent harmonic enhancement.
 */
class OctaveModule {
public:
    OctaveModule();
    ~OctaveModule() = default;

    void prepare(double sampleRate, int blockSize);
    void processBlock(juce::AudioBuffer<float>& buffer, float octaveAmount, 
                     float q1Gain = 0.0f, float q2Gain = 0.0f, float q3Gain = 0.0f, 
                     float outputVolume = 0.0f);
    void reset();

private:
    struct ChannelState {
        // High-pass filter for frequency detection
        float hp_x1 = 0.0f;
        float hp_y1 = 0.0f;
        
        // Envelope follower for amplitude tracking
        float envelope = 0.0f;
        
        // Frequency detector state
        float freq_detector = 0.0f;
        
        // Wave rectification state
        float rectify_smooth = 0.0f;
        
        // Harmonic generation state
        float harmonic_phase = 0.0f;
        
        // DC blocker
        float dc_x1 = 0.0f;
        float dc_y1 = 0.0f;
        
        // Green Ringer transistor states
        float q1_collector = 0.0f;
        float q1_base = 0.0f;
        float q2_collector = 0.0f;
        float q2_base = 0.0f;
        float q3_collector = 0.0f;
        float q3_base = 0.0f;
        
        // Frequency emphasis filter state
        float freq_emphasis_x1 = 0.0f;
        float freq_emphasis_y1 = 0.0f;
        
        // Zero-crossing octave generation
        float lastSample = 0.0f;
        float octavePhase = 0.0f;
        
        // Phase-locked octave generation
        float input_phase = 0.0f;
        float hilbert_delay[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    };

    // Core octave fuzz processing
    float processSample(float sample, int channel, float octaveAmount, 
                       float q1Gain, float q2Gain, float q3Gain, float outputVolume);
    
    // Frequency-dependent octave generation
    float generateOctaveHarmonics(float sample, int channel, float intensity);
    
    // Wave rectification (creates even harmonics)
    float applyWaveRectification(float sample, float amount);
    
    // Frequency detector (higher freq = more octave)
    float detectFrequencyContent(float sample, int channel);
    
    // High-pass filter for frequency emphasis
    float highPassFilter(float input, int channel);
    
    // DC blocker
    float dcBlocker(float input, int channel);
    
    // Green Ringer transistor stages
    float processQ1Stage(float input, int channel);  // 2N5089 NPN input
    float processQ2Stage(float input, int channel);  // 2N3906 PNP ring modulator
    float processQ3Stage(float input, int channel);  // 2N5089 NPN output buffer
    
    // Frequency-selective processing
    float applyFrequencySelectiveEmphasis(float input, int channel);

    std::array<ChannelState, Constants::MAX_CHANNELS> channelStates_;
    double sampleRate_ = Constants::DEFAULT_SAMPLE_RATE;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OctaveModule)
};

}  // namespace DSP
}  // namespace TerminalFuzz