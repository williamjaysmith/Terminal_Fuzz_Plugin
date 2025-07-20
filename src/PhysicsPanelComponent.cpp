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
    
    // Post-processing lowpass filter
    lowpassFilterKnob_ = std::make_unique<juce::Slider>(juce::Slider::RotaryVerticalDrag, juce::Slider::TextBoxBelow);
    lowpassFilterKnob_->setTextValueSuffix(" Hz");
    lowpassFilterKnob_->setTooltip("Post-pedal lowpass filter (18dB/octave)");
    addAndMakeVisible(*lowpassFilterKnob_);
    
    lowpassFilterLabel_ = std::make_unique<juce::Label>("", "LOWPASS");
    lowpassFilterLabel_->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*lowpassFilterLabel_);
    
    lowpassFilterAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters_, PluginParameters::LOWPASS_FREQ_ID, *lowpassFilterKnob_);
    
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
    
    // Manual transistor gain debugging controls with descriptive names
    q1DebugSlider_ = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    q1DebugLabel_ = std::make_unique<juce::Label>("", "Q1 OUTPUT BUFFER\n(2N3904)");
    q1DebugSlider_->setTooltip("Manual gain control before Q1 output buffer stage");
    q1BypassButton_ = std::make_unique<juce::ToggleButton>("Q1 BYPASS");
    q1BypassButton_->setButtonText("BYPASS");
    q1BypassButton_->setTooltip("Bypass Q1 output buffer stage");
    addAndMakeVisible(*q1DebugSlider_);
    addAndMakeVisible(*q1DebugLabel_);
    addAndMakeVisible(*q1BypassButton_);
    q1DebugAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters_, PluginParameters::Q1_MANUAL_GAIN_ID, *q1DebugSlider_);
    q1BypassAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters_, PluginParameters::Q1_BYPASS_ID, *q1BypassButton_);
    
    q2DebugSlider_ = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    q2DebugLabel_ = std::make_unique<juce::Label>("", "Q2 MAIN FUZZ\n(2N2369)");
    q2DebugSlider_->setTooltip("Manual gain control before Q2 main fuzz stage");
    q2BypassButton_ = std::make_unique<juce::ToggleButton>("Q2 BYPASS");
    q2BypassButton_->setButtonText("BYPASS");
    q2BypassButton_->setTooltip("Bypass Q2 main fuzz stage");
    addAndMakeVisible(*q2DebugSlider_);
    addAndMakeVisible(*q2DebugLabel_);
    addAndMakeVisible(*q2BypassButton_);
    q2DebugAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters_, PluginParameters::Q2_MANUAL_GAIN_ID, *q2DebugSlider_);
    q2BypassAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters_, PluginParameters::Q2_BYPASS_ID, *q2BypassButton_);
    
    q3DebugSlider_ = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    q3DebugLabel_ = std::make_unique<juce::Label>("", "Q3 CLEAN GAIN\n(2N2369)");
    q3DebugSlider_->setTooltip("Manual gain control before Q3 clean gain stage");
    q3BypassButton_ = std::make_unique<juce::ToggleButton>("Q3 BYPASS");
    q3BypassButton_->setButtonText("BYPASS");
    q3BypassButton_->setTooltip("Bypass Q3 clean gain stage");
    addAndMakeVisible(*q3DebugSlider_);
    addAndMakeVisible(*q3DebugLabel_);
    addAndMakeVisible(*q3BypassButton_);
    q3DebugAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters_, PluginParameters::Q3_MANUAL_GAIN_ID, *q3DebugSlider_);
    q3BypassAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        parameters_, PluginParameters::Q3_BYPASS_ID, *q3BypassButton_);
    
    // REMOVED: All anti-gating controls - user found them ineffective
    
    // Reset button removed
    
    setSize(600, 600);  // Full plugin window size
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
    bounds.removeFromTop(100);  // Title and subtitle space
    
    const int knobSize = 80;   // Reasonable knob size
    const int buttonHeight = 30;
    const int spacing = 30;    // Good spacing
    
    // Top section: Lowpass filter, Tone bypass, and Bit crushing
    auto topSection = bounds.removeFromTop(200);
    topSection.reduce(60, 20);  // Side margins
    
    // Lowpass filter knob (left side)
    auto filterArea = topSection.removeFromLeft(180);
    lowpassFilterLabel_->setBounds(filterArea.removeFromTop(25));
    lowpassFilterKnob_->setBounds(filterArea.removeFromTop(knobSize + 30).withSizeKeepingCentre(knobSize, knobSize + 30));
    
    topSection.removeFromLeft(spacing);  // Gap between filter and buttons
    
    // Buttons area (right side) - centered vertically
    auto buttonsArea = topSection;
    auto buttonWidth = (buttonsArea.getWidth() - spacing) / 2;
    auto buttonY = (buttonsArea.getHeight() - buttonHeight) / 2;
    
    toneBypassButton_->setBounds(buttonsArea.removeFromLeft(buttonWidth)
                                .withY(buttonY).withHeight(buttonHeight).reduced(10, 0));
    buttonsArea.removeFromLeft(spacing);
    bitCrushingButton_->setBounds(buttonsArea
                                 .withY(buttonY).withHeight(buttonHeight).reduced(10, 0));
    
    bounds.removeFromTop(spacing);  // Gap between sections
    
    // Bottom section: Manual transistor gain controls - use remaining space
    auto debugArea = bounds.removeFromTop(250);
    debugArea.reduce(60, 0);  // Side margins
    
    auto controlWidth = debugArea.getWidth() / 3;
    
    // Q2 knob, button, and label
    auto q2Area = debugArea.removeFromLeft(controlWidth);
    q2DebugLabel_->setBounds(q2Area.removeFromTop(30));
    q2DebugLabel_->setJustificationType(juce::Justification::centred);
    
    // Knob with extra space for text value below
    auto q2KnobArea = q2Area.removeFromTop(knobSize + 40);  // Extra space for value text
    q2DebugSlider_->setBounds(q2KnobArea.withSizeKeepingCentre(knobSize, knobSize + 30));
    
    q2Area.removeFromTop(10);  // Small gap
    q2BypassButton_->setBounds(q2Area.removeFromTop(buttonHeight).reduced(20, 0));
    
    // Q3 knob, button, and label  
    auto q3Area = debugArea.removeFromLeft(controlWidth);
    q3DebugLabel_->setBounds(q3Area.removeFromTop(30));
    q3DebugLabel_->setJustificationType(juce::Justification::centred);
    
    // Knob with extra space for text value below
    auto q3KnobArea = q3Area.removeFromTop(knobSize + 40);  // Extra space for value text
    q3DebugSlider_->setBounds(q3KnobArea.withSizeKeepingCentre(knobSize, knobSize + 30));
    
    q3Area.removeFromTop(10);  // Small gap
    q3BypassButton_->setBounds(q3Area.removeFromTop(buttonHeight).reduced(20, 0));
    
    // Q1 knob, button, and label
    auto q1Area = debugArea;
    q1DebugLabel_->setBounds(q1Area.removeFromTop(30));
    q1DebugLabel_->setJustificationType(juce::Justification::centred);
    
    // Knob with extra space for text value below
    auto q1KnobArea = q1Area.removeFromTop(knobSize + 40);  // Extra space for value text
    q1DebugSlider_->setBounds(q1KnobArea.withSizeKeepingCentre(knobSize, knobSize + 30));
    
    q1Area.removeFromTop(10);  // Small gap
    q1BypassButton_->setBounds(q1Area.removeFromTop(buttonHeight).reduced(20, 0));
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