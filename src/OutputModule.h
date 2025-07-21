#pragma once

#include <JuceHeader.h>
#include "Common.h"

namespace TerminalFuzz {
namespace DSP {

/**
 * @brief Final output volume control and dB metering module
 * 
 * Provides clean volume control and peak metering to prevent digital clipping
 */
class OutputModule {
public:
    OutputModule();
    ~OutputModule() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void processBlock(juce::AudioBuffer<float>& buffer);
    
    // Volume control
    void setVolumeDB(float volumeDB);
    float getVolumeDB() const { return volumeDB_; }
    
    // Peak metering
    float getPeakLevelDB() const { return peakLevelDB_; }
    float getRMSLevelDB() const { return rmsLevelDB_; }
    bool isClipping() const { return isClipping_; }
    
    // Reset peak hold
    void resetPeakHold();

private:
    // Volume control
    float volumeDB_ = 0.0f;
    float volumeLinear_ = 1.0f;
    
    // Peak detection
    float peakLevel_ = 0.0f;
    float peakLevelDB_ = -100.0f;
    float rmsLevel_ = 0.0f;
    float rmsLevelDB_ = -100.0f;
    bool isClipping_ = false;
    
    // Peak hold and smoothing (fast response for real-time monitoring)
    float peakHoldTime_ = 50.0f; // ms - much shorter hold
    float peakDecayRate_ = 120.0f; // dB/second - much faster decay
    float rmsAlpha_ = 0.05f; // RMS smoothing - faster response
    
    // Sample rate
    double sampleRate_ = 44100.0;
    
    // Convert linear to dB
    static float linearToDb(float linear) {
        return linear > 0.0f ? 20.0f * std::log10(linear) : -100.0f;
    }
    
    // Convert dB to linear
    static float dbToLinear(float db) {
        return std::pow(10.0f, db * 0.05f);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OutputModule)
};

}  // namespace DSP
}  // namespace TerminalFuzz