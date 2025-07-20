#include "FrontPanelComponent.h"
#include "PluginParameters.h"

namespace TerminalFuzz {
namespace GUI {

FrontPanelComponent::FrontPanelComponent(juce::AudioProcessorValueTreeState& parameters)
    : parameters_(parameters)
{
    // This component now only handles LED meter - main knobs moved to PluginEditor
}

FrontPanelComponent::~FrontPanelComponent() {
}

void FrontPanelComponent::paint(juce::Graphics& g) {
    // Nothing to paint - LED is now handled in PluginEditor with images
}

void FrontPanelComponent::resized() {
    // Nothing to resize - LED is now handled in PluginEditor
}

void FrontPanelComponent::configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText) {
    addAndMakeVisible(slider);
    addAndMakeVisible(label);
    
    // Configure slider appearance
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffffff7f));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff666666));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffffffff));
    
    // Configure label
    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setFont(juce::Font(12.0f, juce::Font::bold));
}

void FrontPanelComponent::configureLabel(juce::Label& label, bool isValueLabel) {
    label.setJustificationType(juce::Justification::centred);
    if (isValueLabel) {
        label.setFont(juce::Font(11.0f));
        label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    } else {
        label.setFont(juce::Font(12.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colours::white);
    }
}

void FrontPanelComponent::setupKnobCallbacks() {
    // No knob callbacks needed - knobs moved to PluginEditor
}

void FrontPanelComponent::updateInputLevelMeter(float inputLevelDb) {
    // Deprecated - LED is now handled in PluginEditor with images
    juce::ignoreUnused(inputLevelDb);
}

// REMOVED: updateAutoGainMeter method - user found auto-gain ineffective

}  // namespace GUI
}  // namespace TerminalFuzz