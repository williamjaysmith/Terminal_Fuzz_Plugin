#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FrontPanelComponent.h"
#include "PhysicsPanelComponent.h"
#include "SteelBackgroundComponent.h"

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

    // Reference to the processor
    PluginProcessor& audioProcessor_;

    // GUI components
    std::unique_ptr<GUI::SteelBackgroundComponent> steelBackground_;
    std::unique_ptr<GUI::FrontPanelComponent> frontPanel_;
    std::unique_ptr<GUI::PhysicsPanelComponent> physicsPanel_;
    
    // Panel toggle button
    std::unique_ptr<juce::TextButton> physicsPanelButton_;
    
    // Current panel mode
    bool showPhysicsPanel_ = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

}  // namespace TerminalFuzz