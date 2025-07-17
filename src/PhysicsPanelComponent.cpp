#include "PhysicsPanelComponent.h"
#include "Common.h"
#include "PluginParameters.h"

namespace TerminalFuzz {
namespace GUI {

PhysicsPanelComponent::PhysicsPanelComponent(juce::AudioProcessorValueTreeState& parameters)
    : parameters_(parameters)
{
    // Initialize stock physics values (current working settings)
    stockPhysics_ = DSP::TerminalCircuit::PhysicsControls{};
    
    // REMOVED SYNTH HORN - back to clean physics panel
    // Parasitics fixed at 25, thermal/saturation fixed at working levels
    
    // Tone bypass toggle
    toneBypassButton_ = std::make_unique<juce::ToggleButton>("Bypass Tone Stack");
    toneBypassButton_->setButtonText("BYPASS TONE STACK");
    toneBypassButton_->setTooltip("Skip voice/treble controls for direct Q2→Q3 connection");
    addAndMakeVisible(*toneBypassButton_);
    
    toneBypassAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters_, PluginParameters::TONE_BYPASS_ID, *toneBypassButton_);
    
    // REMOVED: Harmonics and harmonic filtering - not necessary for this circuit
    
    // Bit crushing toggle
    bitCrushingButton_ = std::make_unique<juce::ToggleButton>("Enable Bit Crushing");
    bitCrushingButton_->setButtonText("BIT CRUSHING");
    bitCrushingButton_->setTooltip("Enable sample rate and bit depth reduction for lo-fi character");
    bitCrushingButton_->setToggleState(false, juce::dontSendNotification);
    addAndMakeVisible(*bitCrushingButton_);
    
    bitCrushingAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters_, PluginParameters::BIT_CRUSHING_ID, *bitCrushingButton_);
    
    // REMOVED: Ring mod and gating - not necessary for this circuit
    
    // Advanced compression toggle
    advancedCompressionButton_ = std::make_unique<juce::ToggleButton>("Enable Advanced Compression");
    advancedCompressionButton_->setButtonText("COMPRESSION");
    advancedCompressionButton_->setTooltip("Enable transistor-based compression with feedback loops");
    advancedCompressionButton_->setToggleState(false, juce::dontSendNotification);
    // addAndMakeVisible(*advancedCompressionButton_); // HIDDEN - compression removed
    
    advancedCompressionAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters_, PluginParameters::ADVANCED_COMPRESSION_ID, *advancedCompressionButton_);
    
    // REMOVED: Auto-gain and extreme misbias toggles - user found them ineffective
    
    // REMOVED: All anti-gating controls - user found them ineffective
    
    // Reset button (visible at bottom)
    resetButton_ = std::make_unique<juce::TextButton>("Reset to Stock");
    resetButton_->setButtonText("🔄 RESET TO STOCK PHYSICS");
    resetButton_->setTooltip("Return all physics parameters to current working settings");
    resetButton_->onClick = [this]() { resetToStockPhysics(); };
    addAndMakeVisible(*resetButton_);
    
    setSize(600, 200);  // Reduced height after removing auto-gain/misbias buttons
}

void PhysicsPanelComponent::setupMasterControl(MasterControl& master, 
                                              const juce::String& title,
                                              const juce::String& tooltip,
                                              const juce::String& paramId,
                                              double minValue,
                                              double maxValue,
                                              double defaultValue) {
    master.title = title;
    master.tooltip = tooltip;
    
    // Master slider with custom range
    master.slider = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, 
                                                  juce::Slider::TextBoxBelow);
    master.slider->setRange(minValue, maxValue, 0.1);
    master.slider->setValue(defaultValue);
    master.slider->setSkewFactorFromMidPoint(defaultValue);
    master.slider->setTooltip(tooltip);
    addAndMakeVisible(*master.slider);
    
    // Master label
    master.label = std::make_unique<juce::Label>("", title);
    master.label->setJustificationType(juce::Justification::centred);
    master.label->setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(*master.label);
    
    // Parameter attachment
    master.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters_, paramId, *master.slider);
}

void PhysicsPanelComponent::setupSimplifiedControls() {
    // Simplified interface - just parasitic control
}

void PhysicsPanelComponent::paint(juce::Graphics& g) {
    // Dark background for physics panel
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("🔬 PHYSICS TUNING PANEL", 0, 10, getWidth(), 30, juce::Justification::centred);
    
    // Draw subtitle
    g.setFont(juce::Font(12.0f));
    g.setColour(juce::Colours::lightgrey);
    g.drawText("Authentic FY-2/Terminal physics - Extreme starvation permanent + input gain for sustain", 
               0, 35, getWidth(), 20, juce::Justification::centred);
               
    // Draw status
    g.setFont(juce::Font(11.0f));
    g.setColour(juce::Colours::yellow);
    g.drawText("✓ MAXIMUM BROKEN CHARACTER - Use INPUT knob on front panel for sustain control", 
               0, 55, getWidth(), 20, juce::Justification::centred);
    
    // Simplified interface - no detail window
}

void PhysicsPanelComponent::resized() {
    auto bounds = getLocalBounds();
    bounds.removeFromTop(60);  // Title space
    
    const int knobSize = 80;
    const int spacing = 20;
    
    // Clean physics panel - no knobs, just text
    bounds.removeFromTop(100); // Space for description text
    
    bounds.removeFromTop(spacing);
    
    // Controls at bottom - simplified physics panel
    auto controlsArea = bounds.removeFromBottom(120); // Reduced height after removing auto-gain/misbias
    
    // First row: Tone bypass, Bit crushing
    auto controlRow1 = controlsArea.removeFromTop(45);
    auto halfWidth = controlRow1.getWidth() / 2;
    toneBypassButton_->setBounds(controlRow1.removeFromLeft(halfWidth).reduced(5));
    bitCrushingButton_->setBounds(controlRow1.reduced(5));
    
    controlsArea.removeFromTop(10); // Small gap
    
    // REMOVED: Auto-gain and extreme misbias row - user found them ineffective
    
    // Bottom row: Reset button only (centered)
    auto bottomRow = controlsArea.removeFromTop(45);
    resetButton_->setBounds(bottomRow.reduced(5));
}


void PhysicsPanelComponent::resetToStockPhysics() {
    // Reset essential toggles to default
    toneBypassButton_->setToggleState(false, juce::sendNotification);
    bitCrushingButton_->setToggleState(false, juce::sendNotification); // Default disabled
    // Removed controlled feedback button
    // Starvation is now permanent - no toggle to reset
    
    juce::Logger::writeToLog("🔄 Physics parameters reset to optimal settings");
}

}  // namespace GUI
}  // namespace TerminalFuzz