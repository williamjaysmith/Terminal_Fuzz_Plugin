#include "OutputModule.h"

namespace TerminalFuzz {
namespace DSP {

OutputModule::OutputModule() {
    reset();
}

void OutputModule::prepare(const juce::dsp::ProcessSpec& spec) {
    sampleRate_ = spec.sampleRate;
    
    // Calculate RMS smoothing alpha based on sample rate
    // Target time constant of ~3ms for fast response
    float timeConstant = 0.003f; // 3ms
    rmsAlpha_ = 1.0f - std::exp(-1.0f / (timeConstant * static_cast<float>(sampleRate_)));
    
    reset();
}

void OutputModule::reset() {
    peakLevel_ = 0.0f;
    peakLevelDB_ = -100.0f;
    rmsLevel_ = 0.0f;
    rmsLevelDB_ = -100.0f;
    isClipping_ = false;
}

void OutputModule::processBlock(juce::AudioBuffer<float>& buffer) {
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    if (numSamples == 0) return;
    
    // Apply volume control
    if (volumeDB_ != 0.0f) {
        for (int channel = 0; channel < numChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample) {
                channelData[sample] *= volumeLinear_;
            }
        }
    }
    
    // Calculate peak and RMS levels
    float blockPeak = 0.0f;
    float blockRMSSum = 0.0f;
    
    for (int channel = 0; channel < numChannels; ++channel) {
        const auto* channelData = buffer.getReadPointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample) {
            float sampleAbs = std::abs(channelData[sample]);
            
            // Peak detection
            if (sampleAbs > blockPeak) {
                blockPeak = sampleAbs;
            }
            
            // RMS calculation
            blockRMSSum += sampleAbs * sampleAbs;
        }
    }
    
    // Update peak level (with aggressive silence detection)
    if (blockPeak > peakLevel_) {
        peakLevel_ = blockPeak;
    } else {
        // Very aggressive decay - if current block is much quieter, drop quickly
        if (blockPeak < peakLevel_ * 0.1f) { // If new peak is less than 10% of current peak
            peakLevel_ = blockPeak; // Immediately drop to current level
        } else {
            // Normal decay for gradual changes
            float decayPerSample = peakDecayRate_ / static_cast<float>(sampleRate_);
            float decayLinear = dbToLinear(-decayPerSample);
            peakLevel_ *= decayLinear;
        }
    }
    
    // Update RMS level (with aggressive silence detection)
    float blockRMS = std::sqrt(blockRMSSum / (numSamples * numChannels));
    
    // If block is very quiet (silence), drop RMS immediately
    if (blockRMS < rmsLevel_ * 0.05f) { // If new RMS is less than 5% of current RMS
        rmsLevel_ = blockRMS; // Immediately drop to silence
    } else {
        // Normal smoothing for gradual changes
        rmsLevel_ = rmsLevel_ * (1.0f - rmsAlpha_) + blockRMS * rmsAlpha_;
    }
    
    // Convert to dB
    peakLevelDB_ = linearToDb(peakLevel_);
    rmsLevelDB_ = linearToDb(rmsLevel_);
    
    // Check for clipping (above -0.1dB to give some headroom)
    isClipping_ = peakLevelDB_ > -0.1f;
    
    // Ensure minimum values
    if (peakLevelDB_ < -100.0f) peakLevelDB_ = -100.0f;
    if (rmsLevelDB_ < -100.0f) rmsLevelDB_ = -100.0f;
}

void OutputModule::setVolumeDB(float volumeDB) {
    volumeDB_ = juce::jlimit(-100.0f, 100.0f, volumeDB);
    volumeLinear_ = dbToLinear(volumeDB_);
}

void OutputModule::resetPeakHold() {
    peakLevel_ = 0.0f;
    peakLevelDB_ = -100.0f;
}

}  // namespace DSP
}  // namespace TerminalFuzz