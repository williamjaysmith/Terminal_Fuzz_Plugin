#pragma once

#include <JuceHeader.h>
#include "TerminalCircuit.h"

namespace TerminalFuzz {
namespace GUI {

/**
 * @brief Collapsible physics tuning panel for the back of the pedal
 * 
 * Features master knobs with expandable detail controls and descriptive labels
 */
class PhysicsPanelComponent : public juce::Component {
public:
    PhysicsPanelComponent(juce::AudioProcessorValueTreeState& parameters);
    ~PhysicsPanelComponent() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Reset all physics to current stock settings
    void resetToStockPhysics();

private:
    // Simplified master control structure (no expand buttons or detail controls)
    struct MasterControl {
        juce::String title;
        juce::String tooltip;
        std::unique_ptr<juce::Slider> slider;
        std::unique_ptr<juce::Label> label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    
    // Clean physics panel - no user controls
    
    // Post-processing lowpass filter
    std::unique_ptr<juce::Slider> lowpassFilterKnob_;
    std::unique_ptr<juce::Label> lowpassFilterLabel_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowpassFilterAttachment_;
    
    // Tone bypass, harmonics toggle, and reset
    std::unique_ptr<juce::ToggleButton> toneBypassButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> toneBypassAttachment_;
    std::unique_ptr<juce::ToggleButton> harmonicsButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> harmonicsAttachment_;
    std::unique_ptr<juce::ToggleButton> harmonicFilteringButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> harmonicFilteringAttachment_;
    std::unique_ptr<juce::ToggleButton> bitCrushingButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bitCrushingAttachment_;
    std::unique_ptr<juce::ToggleButton> frequencyDoublingButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> frequencyDoublingAttachment_;
    std::unique_ptr<juce::ToggleButton> controlledGatingButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> controlledGatingAttachment_;
    std::unique_ptr<juce::ToggleButton> advancedCompressionButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> advancedCompressionAttachment_;
    std::unique_ptr<juce::ToggleButton> envelopeBiasButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> envelopeBiasAttachment_;
    
    // REMOVED: Auto-gain and misbias controls - user found them ineffective
    
    // REMOVED: All anti-gating controls - user found them ineffective
    
    // Reset button removed
    
    // Setup helpers
    void setupMasterControl(MasterControl& master, 
                          const juce::String& title,
                          const juce::String& tooltip,
                          const juce::String& paramId,
                          double minValue,
                          double maxValue,
                          double defaultValue);
                          
    // Simplified setup
    void setupSimplifiedControls();
    
    juce::AudioProcessorValueTreeState& parameters_;
    
    // Stock physics values for reset
    DSP::TerminalCircuit::PhysicsControls stockPhysics_;
    
    // Manual transistor gain debugging controls
    std::unique_ptr<juce::Slider> q1DebugSlider_;
    std::unique_ptr<juce::Label> q1DebugLabel_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> q1DebugAttachment_;
    std::unique_ptr<juce::ToggleButton> q1BypassButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> q1BypassAttachment_;
    
    std::unique_ptr<juce::Slider> q2DebugSlider_;
    std::unique_ptr<juce::Label> q2DebugLabel_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> q2DebugAttachment_;
    std::unique_ptr<juce::ToggleButton> q2BypassButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> q2BypassAttachment_;
    
    std::unique_ptr<juce::Slider> q3DebugSlider_;
    std::unique_ptr<juce::Label> q3DebugLabel_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> q3DebugAttachment_;
    std::unique_ptr<juce::ToggleButton> q3BypassButton_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> q3BypassAttachment_;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PhysicsPanelComponent)
};

}  // namespace GUI
}  // namespace TerminalFuzz