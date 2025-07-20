#pragma once

#include <JuceHeader.h>

class VintageKnobComponent : public juce::Component
{
public:
    VintageKnobComponent();
    ~VintageKnobComponent() override;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
    
    // Value control
    void setValue(float newValue, bool sendNotification = true);
    float getValue() const { return currentValue; }
    
    // Range control
    void setRange(float min, float max) { minValue = min; maxValue = max; }
    
    // Callbacks
    std::function<void(float)> onValueChange;

private:
    float currentValue = 0.5f;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    
    bool isDragging = false;
    bool isHovering = false;
    juce::Point<int> lastMousePos;
    
    // Guitar pick shape path
    juce::Path createGuitarPickShape(float centerX, float centerY, float width, float height);
    
    // Brushed metal texture
    void drawBrushedMetal(juce::Graphics& g, const juce::Rectangle<float>& bounds);
    
    // 3D plastic border
    void drawPlasticBorder(juce::Graphics& g, const juce::Path& shape);
    
    // Helper to update value from mouse position
    void updateValueFromMousePosition(const juce::MouseEvent& event);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VintageKnobComponent)
};