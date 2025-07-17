#pragma once

#include <JuceHeader.h>

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
    
    // Control knobs
    std::unique_ptr<juce::Slider> inputGainSlider_;
    std::unique_ptr<juce::Slider> fuzzSlider_;
    std::unique_ptr<juce::Slider> voiceSlider_;
    std::unique_ptr<juce::Slider> trebleSlider_;
    std::unique_ptr<juce::Slider> levelSlider_;
    
    // Labels
    std::unique_ptr<juce::Label> inputGainLabel_;
    std::unique_ptr<juce::Label> fuzzLabel_;
    std::unique_ptr<juce::Label> voiceLabel_;
    std::unique_ptr<juce::Label> trebleLabel_;
    std::unique_ptr<juce::Label> levelLabel_;
    
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
    
    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> fuzzAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voiceAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trebleAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelAttachment_;
    
    /**
     * @brief Configure a slider for the front panel
     * @param slider Slider to configure
     * @param label Label to configure
     * @param labelText Text for the label
     */
    void configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& labelText);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FrontPanelComponent)
};

}  // namespace GUI
}  // namespace TerminalFuzz