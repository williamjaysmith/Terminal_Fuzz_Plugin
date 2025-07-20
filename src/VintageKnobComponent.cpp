#include "VintageKnobComponent.h"

VintageKnobComponent::VintageKnobComponent()
{
    setSize(80, 80); // Circular proportions
}

VintageKnobComponent::~VintageKnobComponent()
{
}

juce::Path VintageKnobComponent::createGuitarPickShape(float centerX, float centerY, float width, float height)
{
    juce::Path path;
    
    // Simple circle for knob
    float radius = juce::jmin(width, height) * 0.4f;
    path.addEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2);
    
    return path;
}

void VintageKnobComponent::drawPlasticBorder(juce::Graphics& g, const juce::Path& shape)
{
    // This function is no longer used - keeping for compatibility
}

void VintageKnobComponent::drawBrushedMetal(juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    // Silver brushed metal center
    const float centerX = bounds.getCentreX();
    const float centerY = bounds.getCentreY();
    const float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.3f;
    
    // Base silver color
    g.setColour(juce::Colour(200, 200, 210));
    g.fillEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2);
    
    // Brushed metal circular lines
    g.saveState();
    for (int i = 0; i < 50; i++)
    {
        float ringRadius = radius * 0.2f + (i / 50.0f) * radius * 0.6f;
        float alpha = 0.1f + (i % 3) * 0.05f; // Varying opacity for realistic effect
        
        g.setColour(juce::Colour(180, 180, 190).withAlpha(alpha));
        g.drawEllipse(centerX - ringRadius, centerY - ringRadius, 
                     ringRadius * 2, ringRadius * 2, 0.5f);
    }
    g.restoreState();
    
    // Highlight on top left
    g.setColour(juce::Colour(240, 240, 250).withAlpha(0.4f));
    g.fillEllipse(centerX - radius * 0.7f, centerY - radius * 0.7f, 
                 radius * 0.8f, radius * 0.8f);
}

void VintageKnobComponent::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const float size = juce::jmin(bounds.getWidth(), bounds.getHeight());
    const float centerX = bounds.getCentreX();
    const float centerY = bounds.getCentreY();
    
    // Perfect circle dimensions with thin charcoal border
    const float outerRadius = size * 0.4f;
    const float borderWidth = 4.0f; // 4 pixel thick charcoal border
    const float innerRadius = outerRadius - borderWidth;
    
    // Draw proper drop shadow behind knob (FIXED - doesn't rotate with knob)
    // Light source from top, shadow cast straight down
    const float shadowOffsetX = 0.0f;  // No horizontal offset - straight down
    const float shadowOffsetY = 4.0f;  // Only vertical offset
    const float shadowBlur = 8.0f;
    
    // Create radial gradient for soft drop shadow centered on the shadow position
    const float shadowCenterX = centerX + shadowOffsetX;
    const float shadowCenterY = centerY + shadowOffsetY;
    
    juce::ColourGradient dropShadow(
        juce::Colour(0, 0, 0).withAlpha(0.5f), shadowCenterX, shadowCenterY, // Shadow center - darker
        juce::Colour(0, 0, 0).withAlpha(0.0f), shadowCenterX + outerRadius + shadowBlur, shadowCenterY, // Edge - transparent
        true // radial gradient
    );
    // Create smooth linear falloff - fewer stops for seamless transition
    dropShadow.addColour(0.4, juce::Colour(0, 0, 0).withAlpha(0.3f));  // Still dark close to source
    dropShadow.addColour(0.7, juce::Colour(0, 0, 0).withAlpha(0.1f));  // Mid falloff
    dropShadow.addColour(1.0, juce::Colour(0, 0, 0).withAlpha(0.0f));  // Complete transparency at edge
    
    g.setGradientFill(dropShadow);
    // Draw shadow circle - make sure it's visible and properly sized
    const float shadowDrawRadius = outerRadius + shadowBlur; 
    g.fillEllipse(shadowCenterX - shadowDrawRadius, shadowCenterY - shadowDrawRadius, 
                 shadowDrawRadius * 2, shadowDrawRadius * 2);
    
    // Draw thin charcoal border - perfect circle  
    g.setColour(juce::Colour(40, 40, 40)); // Dark charcoal
    g.fillEllipse(centerX - outerRadius, centerY - outerRadius, outerRadius * 2, outerRadius * 2);
    
    // Draw single silver center that fills to the charcoal border
    g.setColour(juce::Colour(200, 200, 210)); // Light silver
    g.fillEllipse(centerX - innerRadius, centerY - innerRadius, innerRadius * 2, innerRadius * 2);
    
    // Draw soft inner shadow on silver center to make it look recessed (FIXED - doesn't rotate)
    // Subtle gradient that's stronger at top, softer at bottom
    g.saveState();
    
    // Create soft radial gradient for inner shadow with natural falloff
    juce::ColourGradient innerShadow(
        juce::Colour(0, 0, 0).withAlpha(0.0f), centerX, centerY, // Center - transparent
        juce::Colour(0, 0, 0).withAlpha(0.15f), centerX, centerY - innerRadius * 0.8f, // Top edge - subtle shadow
        true // radial gradient
    );
    
    // Add gradient stops for natural lighting
    innerShadow.addColour(0.6, juce::Colour(0, 0, 0).withAlpha(0.08f)); // Mid - very subtle
    innerShadow.addColour(1.0, juce::Colour(0, 0, 0).withAlpha(0.03f)); // Edge - barely visible
    
    g.setGradientFill(innerShadow);
    
    // Draw soft inner shadow that fades naturally
    const float innerShadowInset = 2.0f;
    g.fillEllipse(centerX - innerRadius + innerShadowInset, centerY - innerRadius + innerShadowInset, 
                 (innerRadius - innerShadowInset) * 2, (innerRadius - innerShadowInset) * 2);
    
    g.restoreState();
    
    // Calculate rotation angle from currentValue (0-1)
    // 0.5 = top (12 o'clock), 0 = left (-150°), 1 = right (+150°)
    const float knobRange = 300.0f * (juce::MathConstants<float>::pi / 180.0f); // 300 degrees total range in radians
    // Map 0.5 to straight up (0 rotation), 0 to -150°, 1 to +150°
    const float normalizedValue = (currentValue - 0.5f) * 2.0f; // Convert to -1 to +1 range
    const float rotationAngle = normalizedValue * (knobRange / 2.0f);
    
    // Save graphics state and apply rotation transform (ONLY for red line)
    g.saveState();
    g.addTransform(juce::AffineTransform::rotation(rotationAngle, centerX, centerY));
    
    // Red indicator line - extends from center all the way to the black border (rotates with knob)
    const float indicatorLength = innerRadius; // Full length to the border
    const float startX = centerX;
    const float startY = centerY;
    const float endX = centerX;
    const float endY = centerY - indicatorLength; // Always points "up" in rotated coordinate system
    
    // Thin red line
    g.setColour(juce::Colour(200, 40, 40));
    g.drawLine(startX, startY, endX, endY, 1.5f);
    
    // Restore graphics state (remove rotation)
    g.restoreState();
}

void VintageKnobComponent::mouseDown(const juce::MouseEvent& event)
{
    // Check for double click to reset to default
    if (event.getNumberOfClicks() == 2)
    {
        setValue(0.5f, true); // Reset to center (0.5)
        return;
    }
    
    isDragging = true;
    lastMousePos = event.getPosition();
}

void VintageKnobComponent::mouseDrag(const juce::MouseEvent& event)
{
    updateValueFromMousePosition(event);
}

void VintageKnobComponent::mouseMove(const juce::MouseEvent& event)
{
    if (event.mods.isAnyMouseButtonDown())
    {
        updateValueFromMousePosition(event);
    }
}

void VintageKnobComponent::mouseEnter(const juce::MouseEvent& event)
{
    isHovering = true;
    // No visual change on hover - just for interaction tracking
}

void VintageKnobComponent::mouseExit(const juce::MouseEvent& event)
{
    isHovering = false;
    // No visual change on hover - just for interaction tracking
}

void VintageKnobComponent::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
}

void VintageKnobComponent::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    // Handle mouse wheel / trackpad scrolling
    // Use deltaY for vertical scrolling (most common for knobs)
    const float wheelSensitivity = 0.5f; // Adjust this for feel
    
    float delta = wheel.deltaY * wheelSensitivity;
    
    // Reverse direction if wheel.isReversed is true (natural scrolling)
    if (wheel.isReversed)
        delta = -delta;
    
    float newValue = currentValue + delta;
    setValue(newValue, true);
}

void VintageKnobComponent::updateValueFromMousePosition(const juce::MouseEvent& event)
{
    // Professional knob behavior: vertical drag motion
    const int deltaY = lastMousePos.y - event.getPosition().y;
    const float sensitivity = 0.005f; // Fine control sensitivity
    
    float newValue = currentValue + (deltaY * sensitivity);
    newValue = juce::jlimit(0.0f, 1.0f, newValue);
    
    setValue(newValue, true);
    lastMousePos = event.getPosition();
}

void VintageKnobComponent::setValue(float newValue, bool sendNotification)
{
    newValue = juce::jlimit(0.0f, 1.0f, newValue);
    if (currentValue != newValue)
    {
        currentValue = newValue;
        repaint();
        
        if (sendNotification && onValueChange)
            onValueChange(currentValue);
    }
}