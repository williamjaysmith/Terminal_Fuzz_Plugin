#pragma once

#include <JuceHeader.h>

namespace TerminalFuzz {
namespace GUI {

class SteelBackgroundComponent : public juce::Component {
public:
    SteelBackgroundComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    // Create realistic steel texture with gradients and reflections
    void drawSteelTexture(juce::Graphics& g, juce::Rectangle<float> area);
    void drawGrainPattern(juce::Graphics& g, juce::Rectangle<float> area);
    void drawReflectionHighlights(juce::Graphics& g, juce::Rectangle<float> area);
    void drawSubtleShadows(juce::Graphics& g, juce::Rectangle<float> area);
    
    // Steel color palette
    juce::Colour baseSteel = juce::Colour(0xff2a2a2a);        // Dark charcoal base
    juce::Colour lightSteel = juce::Colour(0xff404040);       // Lighter steel for highlights
    juce::Colour darkSteel = juce::Colour(0xff1a1a1a);       // Darker for shadows
    juce::Colour reflectionColor = juce::Colour(0x30ffffff);  // Subtle white reflections
    
    juce::Random random;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SteelBackgroundComponent)
};

} // namespace GUI
} // namespace TerminalFuzz