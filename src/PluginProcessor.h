#pragma once

#include <JuceHeader.h>
#include "PluginParameters.h"
#include "TerminalCircuit.h"

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
    // Circuit access
    const DSP::TerminalCircuit& getTerminalCircuit() const { return terminalCircuit_; }

private:
    /**
     * @brief Update internal parameter values from the parameter tree
     */
    void updateParameters();

    // Parameter management
    juce::AudioProcessorValueTreeState parameters_;
    
    // DSP components
    DSP::TerminalCircuit terminalCircuit_;
    
    // Current parameter values (cached for performance)
    float inputGainDb_ = 0.0f;
    float fuzzAmount_ = 0.5f;
    float voiceAmount_ = 0.5f;
    float trebleAmount_ = 0.5f;
    float levelAmount_ = 0.5f;
    
    // Manual transistor gain controls
    float q1ManualGain_ = 0.0f;
    float q2ManualGain_ = 0.0f;
    float q3ManualGain_ = 0.0f;
    
    // Transistor bypass controls
    bool q1Bypass_ = false;
    bool q2Bypass_ = false;
    bool q3Bypass_ = false;
    
    // Current component values
    DSP::TerminalCircuit::ComponentValues componentValues_;
    
    // Performance monitoring
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                   juce::dsp::IIR::Coefficients<float>> dcBlocker_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};

}  // namespace TerminalFuzz