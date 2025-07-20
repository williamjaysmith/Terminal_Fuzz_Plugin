#pragma once

#include <JuceHeader.h>
#include "ImageKnobComponent.h"

namespace TerminalFuzz {
namespace GUI {

/**
 * @brief Front panel component with standard user controls
 * 
 * This component provides the standard fuzz pedal interface:
 * - Fuzz: Controls the amount of fuzz/gain
 * - Voice: Controls midrange character
 * - Treble: Controls high frequency response
 * - Level: Controls output volume
 */
class FrontPanelComponent : public juce::Component {
public:
    FrontPanelComponent(juce::AudioProcessorValueTreeState& parameters);
    ~FrontPanelComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // LED meter methods
    void updateInputLevelMeter(float inputLevelDb);

private:
    // Parameter state reference
    juce::AudioProcessorValueTreeState& parameters_;
    
    // Control knobs - all using knob1 image knobs
    std::unique_ptr<ImageKnobComponent> fuzzKnob_;
    std::unique_ptr<ImageKnobComponent> voiceKnob_;
    std::unique_ptr<ImageKnobComponent> trebleKnob_;
    std::unique_ptr<ImageKnobComponent> levelKnob_;
    
    // Labels
    std::unique_ptr<juce::Label> fuzzLabel_;
    std::unique_ptr<juce::Label> fuzzValueLabel_;  // Value display
    std::unique_ptr<juce::Label> voiceLabel_;
    std::unique_ptr<juce::Label> voiceValueLabel_;  // Value display
    std::unique_ptr<juce::Label> trebleLabel_;
    std::unique_ptr<juce::Label> trebleValueLabel_;  // Value display
    std::unique_ptr<juce::Label> levelLabel_;
    std::unique_ptr<juce::Label> levelValueLabel_;  // Value display
    
    // LED components for input level meter
    juce::Rectangle<float> redLED_;
    juce::Rectangle<float> greenLED_;
    juce::Rectangle<float> yellowLED_;
    float redLEDBrightness_ = 0.0f;
    float greenLEDBrightness_ = 1.0f; // Default to green as ideal
    float yellowLEDBrightness_ = 0.0f;
    
    // LED target values for smooth transitions
    float redLEDTarget_ = 0.0f;
    float greenLEDTarget_ = 1.0f;
    float yellowLEDTarget_ = 0.0f;
    
    // REMOVED: Auto-gain meter - didn't help with sustain
    
    // Parameter pointers - we'll handle these manually since ImageKnobComponent doesn't use SliderAttachment
    juce::AudioParameterFloat* fuzzParameter_;
    juce::AudioParameterFloat* voiceParameter_;
    juce::AudioParameterFloat* trebleParameter_;
    juce::AudioParameterFloat* levelParameter_;
    
    /**
     * @brief Configure a slider for the front panel
     * @param slider Slider to configure
     * @param label Label to configure
     * @param labelText Text for the label
     */
    void configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText);
    
    /**
     * @brief Configure a label
     * @param label Label to configure
     * @param isValueLabel Whether this is a value display label
     */
    void configureLabel(juce::Label& label, bool isValueLabel = false);
    
    /**
     * @brief Set up callbacks for all knobs
     */
    void setupKnobCallbacks();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FrontPanelComponent)
};

}  // namespace GUI
}  // namespace TerminalFuzz