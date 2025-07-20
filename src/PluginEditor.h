#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FrontPanelComponent.h"
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

    // Reference to the processor
    PluginProcessor& audioProcessor_;

    // GUI components
    juce::Image enclosureImage_;
    std::unique_ptr<GUI::FrontPanelComponent> frontPanel_;
    std::unique_ptr<GUI::PhysicsPanelComponent> physicsPanel_;
    
    // Panel toggle button
    std::unique_ptr<juce::TextButton> physicsPanelButton_;
    
    // All knobs positioned directly in plugin editor (no container constraints)
    std::unique_ptr<ImageKnobComponent> inputGainKnob_;
    std::unique_ptr<juce::Label> inputGainLabel_;
    std::unique_ptr<juce::Label> inputGainValueLabel_;
    juce::AudioParameterFloat* inputGainParameter_;
    
    // Main control knobs
    std::unique_ptr<ImageKnobComponent> fuzzKnob_;
    std::unique_ptr<ImageKnobComponent> voiceKnob_;
    std::unique_ptr<ImageKnobComponent> trebleKnob_;
    std::unique_ptr<ImageKnobComponent> levelKnob_;
    
    // Main control labels
    std::unique_ptr<juce::Label> fuzzLabel_;
    std::unique_ptr<juce::Label> fuzzValueLabel_;
    std::unique_ptr<juce::Label> voiceLabel_;
    std::unique_ptr<juce::Label> voiceValueLabel_;
    std::unique_ptr<juce::Label> trebleLabel_;
    std::unique_ptr<juce::Label> trebleValueLabel_;
    std::unique_ptr<juce::Label> levelLabel_;
    std::unique_ptr<juce::Label> levelValueLabel_;
    
    // Main control parameters
    juce::AudioParameterFloat* fuzzParameter_;
    juce::AudioParameterFloat* voiceParameter_;
    juce::AudioParameterFloat* trebleParameter_;
    juce::AudioParameterFloat* levelParameter_;
    
    // Current panel mode
    bool showPhysicsPanel_ = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

}  // namespace TerminalFuzz