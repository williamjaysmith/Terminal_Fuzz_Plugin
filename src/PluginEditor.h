#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PhysicsPanelComponent.h"
#include "ImageKnobComponent.h"

namespace TerminalFuzz {

/**
 * @brief Main plugin editor that manages front/back panel views
 * 
 * This editor provides two views:
 * - Front Panel: Standard user controls (Fuzz, Voice, Treble, Level)
 * - Back Panel: Complete component-level circuit tweaking
 */
class PluginEditor : public juce::AudioProcessorEditor, private juce::Timer {
public:
    PluginEditor(PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    /**
     * @brief Update panel visibility based on current mode
     */
    void updatePanelVisibility();
    
    /**
     * @brief Timer callback to update LED meter
     */
    void timerCallback() override;
    
    /**
     * @brief Configure a label
     */
    void configureLabel(juce::Label& label, bool isValueLabel = false);
    
    /**
     * @brief Set up callbacks for main knobs
     */
    void setupMainKnobCallbacks();
    
    /**
     * @brief Update dynamic info display
     */
    void updateDynamicInfo(const juce::String& text);
    void clearDynamicInfo();
    
    /**
     * @brief Update LED input level indicator
     */
    void updateInputLevelMeter(float inputLevelDb);

    // Reference to the processor
    PluginProcessor& audioProcessor_;

    // GUI components
    juce::Image enclosureImage_;
    std::unique_ptr<GUI::PhysicsPanelComponent> physicsPanel_;
    
    // Panel toggle button
    std::unique_ptr<juce::TextButton> physicsPanelButton_;
    
    // Dynamic info display (shows knob values on hover/interaction)
    std::unique_ptr<juce::Label> dynamicInfoLabel_;
    
    // LED input level indicator (image-based)
    juce::Image yellowLedImage_;
    juce::Image greenLedImage_;
    juce::Image redLedImage_;
    juce::Image yellowLedOffImage_;  // LED off state when bypassed
    juce::Rectangle<float> ledBounds_;
    
    // LED state tracking
    float redLEDBrightness_ = 0.0f;
    float greenLEDBrightness_ = 0.0f;
    float yellowLEDBrightness_ = 0.0f;
    float redLEDTarget_ = 0.0f;
    float greenLEDTarget_ = 0.0f;
    float yellowLEDTarget_ = 0.0f;
    
    // All knobs positioned directly in plugin editor (no container constraints)
    std::unique_ptr<ImageKnobComponent> inputGainKnob_;
    std::unique_ptr<juce::Label> inputGainLabel_;
    std::unique_ptr<juce::Label> inputGainValueLabel_;
    juce::AudioParameterFloat* inputGainParameter_;
    
    // Main control knobs
    std::unique_ptr<ImageKnobComponent> fuzzKnob_;
    std::unique_ptr<ImageKnobComponent> outputVolumeKnob_;
    // Voice/treble knobs removed - tone stack removed
    
    // Main control labels
    std::unique_ptr<juce::Label> fuzzLabel_;
    std::unique_ptr<juce::Label> fuzzValueLabel_;
    std::unique_ptr<juce::Label> outputVolumeLabel_;
    std::unique_ptr<juce::Label> outputVolumeValueLabel_;
    // Voice/treble labels removed - tone stack removed
    
    // Output dB meter display
    std::unique_ptr<juce::Label> outputDbLabel_;
    std::unique_ptr<juce::Label> outputDbValueLabel_;
    
    // Main control parameters
    juce::AudioParameterFloat* fuzzParameter_;
    juce::AudioParameterFloat* outputVolumeParameter_;
    // Voice/treble parameters removed - tone stack removed
    
    // Current panel mode
    bool showPhysicsPanel_ = false;
    
    // Main bypass switch
    std::unique_ptr<juce::ImageButton> mainBypassSwitch_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> mainBypassAttachment_;
    juce::Image switchOnImage_;
    juce::Image switchOffImage_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

}  // namespace TerminalFuzz