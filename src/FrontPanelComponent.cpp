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
    // Background is now handled by the enclosure image in PluginEditor
    
    // Draw LED input level meter (top left corner)
    float ledSize = 8.0f;
    float ledSpacing = 12.0f;
    float startX = 30.0f;
    float startY = 30.0f;
    
    // Red LED (top)
    juce::Colour redColour = juce::Colours::red.withAlpha(redLEDBrightness_);
    g.setColour(redColour);
    g.fillEllipse(redLED_);
    g.setColour(juce::Colours::darkred);
    g.drawEllipse(redLED_, 1.0f);
    
    // Green LED (middle)  
    juce::Colour greenColour = juce::Colours::green.withAlpha(greenLEDBrightness_);
    g.setColour(greenColour);
    g.fillEllipse(greenLED_);
    g.setColour(juce::Colours::darkgreen);
    g.drawEllipse(greenLED_, 1.0f);
    
    // Yellow LED (bottom)
    juce::Colour yellowColour = juce::Colours::yellow.withAlpha(yellowLEDBrightness_);
    g.setColour(yellowColour);
    g.fillEllipse(yellowLED_);
    g.setColour(juce::Colours::orange);
    g.drawEllipse(yellowLED_, 1.0f);
    
    // REMOVED: Auto-gain meter drawing - didn't help with sustain
}

void FrontPanelComponent::resized() {
    auto bounds = getLocalBounds().toFloat();
    
    // Use proportional positioning based on window size to match background image
    float windowWidth = bounds.getWidth();
    float windowHeight = bounds.getHeight();
    
    // LED meter positioning (proportional to window)
    float ledSize = windowWidth * 0.015f; // 1.5% of window width
    float ledSpacing = windowWidth * 0.025f; // 2.5% of window width
    float ledStartX = windowWidth * 0.02f; // 2% from left
    float ledStartY = windowHeight * 0.08f; // 8% from top
    
    redLED_ = juce::Rectangle<float>(ledStartX, ledStartY, ledSize, ledSize);
    greenLED_ = juce::Rectangle<float>(ledStartX, ledStartY + ledSpacing, ledSize, ledSize);  
    yellowLED_ = juce::Rectangle<float>(ledStartX, ledStartY + ledSpacing * 2, ledSize, ledSize);
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
    // LED ranges: Green = -14 to -10dB, Red = above -10dB, Yellow = below -14dB
    
    // Set target values based on input level
    if (inputLevelDb > -10.0f) {
        // Too hot - red LED
        redLEDTarget_ = 1.0f;
        greenLEDTarget_ = juce::jmax(0.0f, 1.0f - (inputLevelDb + 10.0f) / 5.0f); // Fade out green
        yellowLEDTarget_ = 0.0f;
    } else if (inputLevelDb >= -14.0f) {
        // Ideal range - green LED
        redLEDTarget_ = 0.0f;
        greenLEDTarget_ = 1.0f;
        yellowLEDTarget_ = 0.0f;
    } else {
        // Too quiet - yellow LED
        redLEDTarget_ = 0.0f;
        greenLEDTarget_ = juce::jmax(0.0f, 1.0f - (-14.0f - inputLevelDb) / 5.0f); // Fade out green  
        yellowLEDTarget_ = 1.0f;
    }
    
    // Smooth transitions (1/4 second fade time at 30 FPS = ~7.5 frames)
    float fadeSpeed = 4.0f / 30.0f;  // 1/4 second fade
    
    // Move current values toward targets
    redLEDBrightness_ += (redLEDTarget_ - redLEDBrightness_) * fadeSpeed;
    greenLEDBrightness_ += (greenLEDTarget_ - greenLEDBrightness_) * fadeSpeed;
    yellowLEDBrightness_ += (yellowLEDTarget_ - yellowLEDBrightness_) * fadeSpeed;
    
    repaint(); // Trigger redraw to update LED display
}

// REMOVED: updateAutoGainMeter method - user found auto-gain ineffective

}  // namespace GUI
}  // namespace TerminalFuzz