#include "ImageKnobComponent.h"

ImageKnobComponent::ImageKnobComponent()
{
    setSize(80, 80);
}

ImageKnobComponent::~ImageKnobComponent()
{
}

bool ImageKnobComponent::loadImage(const juce::File& imageFile)
{
    knobImage = juce::ImageCache::getFromFile(imageFile);
    return knobImage.isValid();
}

void ImageKnobComponent::paint(juce::Graphics& g)
{
    if (!knobImage.isValid())
        return;
    
    const auto bounds = getLocalBounds().toFloat();
    const float centerX = bounds.getCentreX();
    const float centerY = bounds.getCentreY();
    
    // Calculate rotation angle from currentValue (0-1)
    // 0.5 = top (12 o'clock), 0 = left (-150°), 1 = right (+150°)
    const float knobRange = 300.0f * (juce::MathConstants<float>::pi / 180.0f); // 300 degrees total range in radians
    // Map 0.5 to straight up (0 rotation), 0 to -150°, 1 to +150°
    const float normalizedValue = (currentValue - 0.5f) * 2.0f; // Convert to -1 to +1 range
    const float rotationAngle = normalizedValue * (knobRange / 2.0f);
    
    // Save graphics state and apply rotation transform
    g.saveState();
    g.addTransform(juce::AffineTransform::rotation(rotationAngle, centerX, centerY));
    
    // Draw the knob image centered and scaled to fit
    const float targetSize = juce::jmin(bounds.getWidth(), bounds.getHeight());
    const float scale = targetSize / (float)juce::jmax(knobImage.getWidth(), knobImage.getHeight());
    const float scaledWidth = knobImage.getWidth() * scale;
    const float scaledHeight = knobImage.getHeight() * scale;
    const float x = centerX - scaledWidth * 0.5f;
    const float y = centerY - scaledHeight * 0.5f;
    
    g.drawImage(knobImage, x, y, scaledWidth, scaledHeight, 
                0, 0, knobImage.getWidth(), knobImage.getHeight());
    
    // Restore graphics state (remove rotation)
    g.restoreState();
}

void ImageKnobComponent::mouseDown(const juce::MouseEvent& event)
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

void ImageKnobComponent::mouseDrag(const juce::MouseEvent& event)
{
    updateValueFromMousePosition(event);
}

void ImageKnobComponent::mouseMove(const juce::MouseEvent& event)
{
    if (event.mods.isAnyMouseButtonDown())
    {
        updateValueFromMousePosition(event);
    }
}

void ImageKnobComponent::mouseEnter(const juce::MouseEvent& event)
{
    isHovering = true;
}

void ImageKnobComponent::mouseExit(const juce::MouseEvent& event)
{
    isHovering = false;
}

void ImageKnobComponent::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
}

void ImageKnobComponent::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
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

void ImageKnobComponent::updateValueFromMousePosition(const juce::MouseEvent& event)
{
    // Professional knob behavior: vertical drag motion
    const int deltaY = lastMousePos.y - event.getPosition().y;
    const float sensitivity = 0.005f; // Fine control sensitivity
    
    float newValue = currentValue + (deltaY * sensitivity);
    newValue = juce::jlimit(0.0f, 1.0f, newValue);
    
    setValue(newValue, true);
    lastMousePos = event.getPosition();
}

void ImageKnobComponent::setValue(float newValue, bool sendNotification)
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