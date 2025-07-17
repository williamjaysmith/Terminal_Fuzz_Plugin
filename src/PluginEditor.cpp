#include "PluginEditor.h"
#include "Common.h"
#include "SteelBackgroundComponent.h"

namespace TerminalFuzz {

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor_(p)
{
    // Create steel background first (bottom layer)
    steelBackground_ = std::make_unique<GUI::SteelBackgroundComponent>();
    addAndMakeVisible(*steelBackground_);
    
    // Create panel components
    frontPanel_ = std::make_unique<GUI::FrontPanelComponent>(audioProcessor_.getParameterState());
    addAndMakeVisible(*frontPanel_);
    
    physicsPanel_ = std::make_unique<GUI::PhysicsPanelComponent>(audioProcessor_.getParameterState());
    physicsPanel_->setVisible(false);
    addAndMakeVisible(*physicsPanel_);
    
    // Create physics panel toggle button
    physicsPanelButton_ = std::make_unique<juce::TextButton>("Physics Panel");
    physicsPanelButton_->setButtonText("🔬 PHYSICS PANEL");
    physicsPanelButton_->setTooltip("Tune comprehensive transistor physics");
    physicsPanelButton_->onClick = [this]() {
        showPhysicsPanel_ = !showPhysicsPanel_;
        updatePanelVisibility();
    };
    addAndMakeVisible(*physicsPanelButton_);
    
    // Start with front panel only
    showPhysicsPanel_ = false;
    updatePanelVisibility();
    
    // Set plugin window size (larger for physics panel)
    setSize(800, 600);
    
    // Start timer for LED meter updates (30 FPS)
    startTimer(33);
}

PluginEditor::~PluginEditor() {
    stopTimer();
}

void PluginEditor::paint(juce::Graphics& g) {
    // Background is now handled by SteelBackgroundComponent
    
    // Draw Terminal Fuzz title with version number
    g.setColour(juce::Colours::white.withAlpha(0.9f)); // Slightly transparent for steel background
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("Terminal Fuzz " TERMINAL_FUZZ_BUILD_VERSION, 0, 10, getWidth(), 30, juce::Justification::centred);
    
    // Draw EQD subtitle  
    g.setFont(juce::Font(12.0f));
    g.setColour(juce::Colours::lightgrey.withAlpha(0.8f)); // Softer color for subtitle
    g.drawText("EarthQuaker Devices Recreation", 0, 35, getWidth(), 20, juce::Justification::centred);
}

void PluginEditor::resized() {
    auto bounds = getLocalBounds();
    
    // Steel background covers entire plugin area
    steelBackground_->setBounds(getLocalBounds());
    
    // Reserve space for title and button
    bounds.removeFromTop(60);  // Title space
    
    // Physics panel toggle button at top center
    auto buttonRow = bounds.removeFromTop(40);
    auto buttonWidth = 200;
    auto buttonX = (buttonRow.getWidth() - buttonWidth) / 2;
    physicsPanelButton_->setBounds(buttonX, buttonRow.getY(), buttonWidth, buttonRow.getHeight());
    
    bounds.removeFromTop(10);  // Spacing
    
    // Set panel bounds
    frontPanel_->setBounds(bounds);
    physicsPanel_->setBounds(bounds);
}

void PluginEditor::updatePanelVisibility() {
    // Update button state
    physicsPanelButton_->setToggleState(showPhysicsPanel_, juce::dontSendNotification);
    
    // Show appropriate panel
    frontPanel_->setVisible(!showPhysicsPanel_);
    physicsPanel_->setVisible(showPhysicsPanel_);
    
    // Update button text to show current state
    physicsPanelButton_->setButtonText(showPhysicsPanel_ ? "🎛️ FRONT PANEL" : "🔬 PHYSICS PANEL");
}

void PluginEditor::timerCallback() {
    // Update LED meter with current input level from the circuit
    if (frontPanel_ && frontPanel_->isVisible()) {
        float inputLevelDb = audioProcessor_.getTerminalCircuit().getInputLevelDb();
        frontPanel_->updateInputLevelMeter(inputLevelDb);
    }
}

}  // namespace TerminalFuzz