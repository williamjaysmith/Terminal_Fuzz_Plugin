#pragma once

#include <JuceHeader.h>
#include "PluginParameters.h"
#include "FuzzCircuit.h"

namespace TerminalFuzz {

/**
 * @brief Main plugin processor for Terminal Fuzz
 * 
 * This class manages the audio processing, parameter state, and coordinates
 * between the DSP circuit implementation and the GUI.
 */
class PluginProcessor : public juce::AudioProcessor {
public:
    PluginProcessor();
    ~PluginProcessor() override;

    //==============================================================================
    // AudioProcessor interface
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
    #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    #endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    // Editor interface
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    // Plugin info
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    

    //==============================================================================
    // Programs
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    // State management
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter access
    juce::AudioProcessorValueTreeState& getParameterState() { return parameters_; }
    const juce::AudioProcessorValueTreeState& getParameterState() const { return parameters_; }
    
    //==============================================================================
    // Fuzz Module access
    const DSP::FuzzCircuit& getFuzzCircuit() const { return fuzzCircuit_; }
    
    // Input level tracking for LED meter
    float getInputLevelDb() const;

private:
    /**
     * @brief Update internal parameter values from the parameter tree
     */
    void updateParameters();

    // Parameter management
    juce::AudioProcessorValueTreeState parameters_;
    
    // DSP Modules
    DSP::FuzzCircuit fuzzCircuit_;  // Main Fuzz Module
    
    // Current parameter values (cached for performance)
    float inputGainDb_ = 0.0f;
    float fuzzAmount_ = 0.5f;
    float voiceAmount_ = 0.5f;
    float trebleAmount_ = 0.5f;
    // levelAmount_ removed - hardcoded to 100% in Fuzz Module
    
    // Manual transistor gain controls
    float q1ManualGain_ = 0.0f;
    float q2ManualGain_ = 0.0f;
    float q3ManualGain_ = 0.0f;
    
    // Transistor bypass controls
    bool q1Bypass_ = false;
    bool q2Bypass_ = false;
    bool q3Bypass_ = false;
    
    // Current component values
    DSP::FuzzCircuit::ComponentValues componentValues_;
    
    // Performance monitoring
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                   juce::dsp::IIR::Coefficients<float>> dcBlocker_;
    
    // Post-processing lowpass filter (18dB/octave = 3-pole)
    // Implemented as cascaded 2-pole + 1-pole filters
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                   juce::dsp::IIR::Coefficients<float>> lowpassFilter2Pole_;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                   juce::dsp::IIR::Coefficients<float>> lowpassFilter1Pole_;
    float lowpassFreq_ = 2000.0f;
    
    // Main bypass state
    bool mainBypass_ = false;
    
    // Input level tracking for LED meter (moved from TerminalCircuit)
    struct InputLevelTracker {
        float averageLevel = 0.0f;
        float envelopeFollower = 0.0f;
        int sampleCounter = 0;
        static const int AVERAGING_SAMPLES = 96000; // 2 seconds at 48kHz
    } inputLevel_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};

}  // namespace TerminalFuzz