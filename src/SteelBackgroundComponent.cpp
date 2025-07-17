#include "SteelBackgroundComponent.h"

namespace TerminalFuzz {
namespace GUI {

SteelBackgroundComponent::SteelBackgroundComponent() {
    random.setSeedRandomly();
}

void SteelBackgroundComponent::paint(juce::Graphics& g) {
    auto area = getLocalBounds().toFloat();
    
    // Layer 1: Base steel gradient
    drawSteelTexture(g, area);
    
    // Layer 2: Grain pattern for realism
    drawGrainPattern(g, area);
    
    // Layer 3: Reflection highlights
    drawReflectionHighlights(g, area);
    
    // Layer 4: Subtle shadows for depth
    drawSubtleShadows(g, area);
}

void SteelBackgroundComponent::resized() {
    // Component resized - could regenerate texture if needed
}

void SteelBackgroundComponent::drawSteelTexture(juce::Graphics& g, juce::Rectangle<float> area) {
    // Main steel gradient - darker at edges, slightly lighter in center
    juce::ColourGradient gradient(
        darkSteel, area.getCentreX(), area.getY(),
        baseSteel, area.getCentreX(), area.getCentreY(),
        false
    );
    
    // Add multiple color stops for realistic metal look
    gradient.addColour(0.3, lightSteel.withAlpha(0.1f));
    gradient.addColour(0.7, baseSteel);
    gradient.addColour(1.0, darkSteel);
    
    g.setGradientFill(gradient);
    g.fillRect(area);
    
    // Subtle horizontal gradient for cylindrical look
    juce::ColourGradient horizontal(
        darkSteel.withAlpha(0.3f), area.getX(), area.getCentreY(),
        juce::Colours::transparentBlack, area.getCentreX(), area.getCentreY(),
        false
    );
    horizontal.addColour(1.0, darkSteel.withAlpha(0.2f));
    
    g.setGradientFill(horizontal);
    g.fillRect(area);
}

void SteelBackgroundComponent::drawGrainPattern(juce::Graphics& g, juce::Rectangle<float> area) {
    // Create fine grain pattern like brushed steel
    g.setColour(lightSteel.withAlpha(0.1f));
    
    int grainLines = (int)(area.getHeight() / 2); // Density of grain lines
    
    for (int i = 0; i < grainLines; ++i) {
        float y = area.getY() + (float)i * 2.0f;
        float opacity = 0.05f + random.nextFloat() * 0.1f; // Vary opacity for realism
        
        g.setColour(lightSteel.withAlpha(opacity));
        
        // Slight variations in line length and position
        float startX = area.getX() + random.nextFloat() * 5.0f;
        float endX = area.getRight() - random.nextFloat() * 5.0f;
        
        g.drawLine(startX, y, endX, y, 0.5f);
    }
    
    // Add some vertical variation for cross-hatched steel look
    g.setColour(baseSteel.withAlpha(0.05f));
    int verticalLines = (int)(area.getWidth() / 8);
    
    for (int i = 0; i < verticalLines; ++i) {
        float x = area.getX() + (float)i * 8.0f + random.nextFloat() * 4.0f;
        float opacity = 0.02f + random.nextFloat() * 0.05f;
        
        g.setColour(darkSteel.withAlpha(opacity));
        g.drawLine(x, area.getY(), x, area.getBottom(), 0.3f);
    }
}

void SteelBackgroundComponent::drawReflectionHighlights(juce::Graphics& g, juce::Rectangle<float> area) {
    // Top reflection highlight (like light hitting the surface)
    juce::ColourGradient topReflection(
        reflectionColor, area.getCentreX(), area.getY(),
        juce::Colours::transparentBlack, area.getCentreX(), area.getY() + area.getHeight() * 0.3f,
        false
    );
    
    g.setGradientFill(topReflection);
    g.fillRect(area.removeFromTop(area.getHeight() * 0.3f));
    
    // Subtle side reflections
    auto leftReflection = area.removeFromLeft(area.getWidth() * 0.1f);
    juce::ColourGradient leftGrad(
        reflectionColor.withAlpha(0.1f), leftReflection.getX(), leftReflection.getCentreY(),
        juce::Colours::transparentBlack, leftReflection.getRight(), leftReflection.getCentreY(),
        false
    );
    
    g.setGradientFill(leftGrad);
    g.fillRect(leftReflection);
    
    // Random bright spots for realistic metal imperfections
    g.setColour(lightSteel.withAlpha(0.2f));
    for (int i = 0; i < 20; ++i) {
        float x = area.getX() + random.nextFloat() * area.getWidth();
        float y = area.getY() + random.nextFloat() * area.getHeight();
        float size = 1.0f + random.nextFloat() * 3.0f;
        
        g.fillEllipse(x, y, size, size);
    }
}

void SteelBackgroundComponent::drawSubtleShadows(juce::Graphics& g, juce::Rectangle<float> area) {
    // Bottom shadow for depth
    auto bottomShadow = area;
    bottomShadow.removeFromTop(area.getHeight() * 0.8f);
    
    juce::ColourGradient shadow(
        juce::Colours::transparentBlack, bottomShadow.getCentreX(), bottomShadow.getY(),
        darkSteel.withAlpha(0.4f), bottomShadow.getCentreX(), bottomShadow.getBottom(),
        false
    );
    
    g.setGradientFill(shadow);
    g.fillRect(bottomShadow);
    
    // Edge shadows for 3D effect
    g.setColour(darkSteel.withAlpha(0.3f));
    g.drawRect(area, 2.0f); // Subtle border
    
    // Inner shadow for recessed look
    auto innerArea = area.reduced(3.0f);
    g.setColour(darkSteel.withAlpha(0.2f));
    g.drawRect(innerArea, 1.0f);
}

} // namespace GUI
} // namespace TerminalFuzz