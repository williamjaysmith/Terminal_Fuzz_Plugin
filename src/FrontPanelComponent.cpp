#include "FrontPanelComponent.h"
#include "PluginParameters.h"

namespace TerminalFuzz {
namespace GUI {

FrontPanelComponent::FrontPanelComponent(juce::AudioProcessorValueTreeState& parameters)
    : parameters_(parameters)
{
    // Create Input Gain control
    inputGainSlider_ = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    inputGainLabel_ = std::make_unique<juce::Label>("", "GAIN");
    configureSlider(*inputGainSlider_, *inputGainLabel_, "GAIN");
    inputGainAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters_, PluginParameters::INPUT_GAIN_ID, *inputGainSlider_);
    
    // Create Fuzz control
    fuzzSlider_ = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    fuzzLabel_ = std::make_unique<juce::Label>("", "FUZZ");
    configureSlider(*fuzzSlider_, *fuzzLabel_, "FUZZ");
    fuzzAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters_, PluginParameters::FUZZ_ID, *fuzzSlider_);
    
    // Create Voice control
    voiceSlider_ = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    voiceLabel_ = std::make_unique<juce::Label>("", "VOICE");
    configureSlider(*voiceSlider_, *voiceLabel_, "VOICE");
    voiceAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters_, PluginParameters::VOICE_ID, *voiceSlider_);
    
    // Create Treble control
    trebleSlider_ = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    trebleLabel_ = std::make_unique<juce::Label>("", "TREBLE");
    configureSlider(*trebleSlider_, *trebleLabel_, "TREBLE");
    trebleAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters_, PluginParameters::TREBLE_ID, *trebleSlider_);
    
    // Create Level control
    levelSlider_ = std::make_unique<juce::Slider>(juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow);
    levelLabel_ = std::make_unique<juce::Label>("", "LEVEL");
    configureSlider(*levelSlider_, *levelLabel_, "LEVEL");
    levelAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        parameters_, PluginParameters::LEVEL_ID, *levelSlider_);
    
    // Manual transistor gain controls moved to physics panel
}

FrontPanelComponent::~FrontPanelComponent() {
}

void FrontPanelComponent::paint(juce::Graphics& g) {
    // Draw front panel background
    g.setColour(juce::Colour(0xff3a3a3a));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 10.0f);
    
    // Draw border
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 10.0f, 2.0f);
    
    // Draw pedal-style graphics
    g.setColour(juce::Colour(0xff4a4a4a));
    auto bounds = getLocalBounds();
    
    // Draw screw holes in corners
    auto screwSize = 8;
    g.fillEllipse(bounds.getX() + 10, bounds.getY() + 10, screwSize, screwSize);
    g.fillEllipse(bounds.getRight() - 18, bounds.getY() + 10, screwSize, screwSize);
    g.fillEllipse(bounds.getX() + 10, bounds.getBottom() - 18, screwSize, screwSize);
    g.fillEllipse(bounds.getRight() - 18, bounds.getBottom() - 18, screwSize, screwSize);
    
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
    auto bounds = getLocalBounds();
    bounds.reduce(20, 20); // Margin from edges
    
    // LED meter positioning (top left corner)
    float ledSize = 8.0f;
    float ledSpacing = 12.0f;
    float ledStartX = 10.0f;
    float ledStartY = 30.0f;
    
    redLED_ = juce::Rectangle<float>(ledStartX, ledStartY, ledSize, ledSize);
    greenLED_ = juce::Rectangle<float>(ledStartX, ledStartY + ledSpacing, ledSize, ledSize);  
    yellowLED_ = juce::Rectangle<float>(ledStartX, ledStartY + ledSpacing * 2, ledSize, ledSize);
    
    // Input gain control at top center
    auto inputGainHeight = bounds.getHeight() / 3;
    auto inputGainWidth = bounds.getWidth() / 3;
    auto inputGainX = bounds.getX() + (bounds.getWidth() - inputGainWidth) / 2;
    inputGainSlider_->setBounds(inputGainX, bounds.getY(), inputGainWidth, inputGainHeight);
    
    // Remaining controls in 2x2 grid below
    auto remainingBounds = bounds.removeFromBottom(bounds.getHeight() * 2 / 3);
    auto controlWidth = remainingBounds.getWidth() / 2;
    auto controlHeight = remainingBounds.getHeight() / 2;
    
    // Top row: Fuzz, Voice
    fuzzSlider_->setBounds(remainingBounds.getX(), remainingBounds.getY(), controlWidth, controlHeight);
    voiceSlider_->setBounds(remainingBounds.getX() + controlWidth, remainingBounds.getY(), controlWidth, controlHeight);
    
    // Bottom row: Treble, Level
    trebleSlider_->setBounds(remainingBounds.getX(), remainingBounds.getY() + controlHeight, controlWidth, controlHeight);
    levelSlider_->setBounds(remainingBounds.getX() + controlWidth, remainingBounds.getY() + controlHeight, controlWidth, controlHeight);
    
    // Manual transistor gain controls moved to physics panel
    
    // Position labels
    auto labelHeight = 20;
    inputGainLabel_->setBounds(inputGainSlider_->getBounds().removeFromTop(labelHeight));
    fuzzLabel_->setBounds(fuzzSlider_->getBounds().removeFromTop(labelHeight));
    voiceLabel_->setBounds(voiceSlider_->getBounds().removeFromTop(labelHeight));
    trebleLabel_->setBounds(trebleSlider_->getBounds().removeFromTop(labelHeight));
    levelLabel_->setBounds(levelSlider_->getBounds().removeFromTop(labelHeight));
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