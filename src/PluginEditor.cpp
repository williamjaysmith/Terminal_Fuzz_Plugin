#include "PluginEditor.h"
#include "Common.h"

namespace TerminalFuzz {

PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor_(p)
{
    // Load enclosure background image
    juce::File enclosureFile("/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/enclosure1.png");
    if (enclosureFile.exists())
    {
        enclosureImage_ = juce::ImageFileFormat::loadFrom(enclosureFile);
        if (!enclosureImage_.isValid())
        {
            DBG("Failed to load enclosure image");
        }
    }
    else
    {
        DBG("Enclosure image file not found: " << enclosureFile.getFullPathName());
    }
    
    // Load LED images
    juce::File yellowLedFile("/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/yellowled.png");
    juce::File greenLedFile("/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/greenled.png");
    juce::File redLedFile("/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/redled.png");
    juce::File yellowLedOffFile("/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/yellowledoff.png");
    
    if (yellowLedFile.exists()) {
        yellowLedImage_ = juce::ImageFileFormat::loadFrom(yellowLedFile);
        if (!yellowLedImage_.isValid()) {
            DBG("Failed to load yellow LED image");
        }
    } else {
        DBG("Yellow LED image file not found");
    }
    
    if (greenLedFile.exists()) {
        greenLedImage_ = juce::ImageFileFormat::loadFrom(greenLedFile);
        if (!greenLedImage_.isValid()) {
            DBG("Failed to load green LED image");
        }
    } else {
        DBG("Green LED image file not found");
    }
    
    if (redLedFile.exists()) {
        redLedImage_ = juce::ImageFileFormat::loadFrom(redLedFile);
        if (!redLedImage_.isValid()) {
            DBG("Failed to load red LED image");
        }
    } else {
        DBG("Red LED image file not found");
    }
    
    if (yellowLedOffFile.exists()) {
        yellowLedOffImage_ = juce::ImageFileFormat::loadFrom(yellowLedOffFile);
        if (!yellowLedOffImage_.isValid()) {
            DBG("Failed to load yellow LED off image");
        }
    } else {
        DBG("Yellow LED off image file not found");
    }
    
    // Create panel components (only physics panel needed now since main knobs are in editor)
    physicsPanel_ = std::make_unique<GUI::PhysicsPanelComponent>(audioProcessor_.getParameterState());
    physicsPanel_->setVisible(false);
    addAndMakeVisible(*physicsPanel_);
    
    // Front panel no longer needed - LED is handled directly in PluginEditor
    
    // Create physics panel toggle button
    physicsPanelButton_ = std::make_unique<juce::TextButton>("Physics Panel");
    physicsPanelButton_->setButtonText("🔬 PHYSICS PANEL");
    physicsPanelButton_->setTooltip("Tune comprehensive transistor physics");
    physicsPanelButton_->onClick = [this]() {
        showPhysicsPanel_ = !showPhysicsPanel_;
        updatePanelVisibility();
    };
    addAndMakeVisible(*physicsPanelButton_);
    
    // Create dynamic info display (shows knob values on hover/interaction)
    dynamicInfoLabel_ = std::make_unique<juce::Label>("", "");
    dynamicInfoLabel_->setJustificationType(juce::Justification::centred);
    dynamicInfoLabel_->setFont(juce::Font(14.0f, juce::Font::bold));
    dynamicInfoLabel_->setColour(juce::Label::textColourId, juce::Colours::white);
    dynamicInfoLabel_->setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.7f));
    addAndMakeVisible(*dynamicInfoLabel_);
    
    // Load bypass switch images
    juce::File switchOnFile("/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/switchon.png");
    juce::File switchOffFile("/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/switchoff.png");
    
    if (switchOnFile.exists()) {
        switchOnImage_ = juce::ImageCache::getFromFile(switchOnFile);
    }
    if (switchOffFile.exists()) {
        switchOffImage_ = juce::ImageCache::getFromFile(switchOffFile);
    }
    
    // Create main bypass switch
    mainBypassSwitch_ = std::make_unique<juce::ImageButton>("Main Bypass");
    mainBypassSwitch_->setImages(false, true, true,
                                switchOnImage_, 1.0f, juce::Colours::transparentBlack,   // Normal: circuit ON
                                switchOnImage_, 1.0f, juce::Colours::transparentBlack,   // Hover: circuit ON  
                                switchOffImage_, 1.0f, juce::Colours::transparentBlack); // Toggled: circuit OFF
    mainBypassSwitch_->setClickingTogglesState(true);
    mainBypassSwitch_->setTooltip("Bypass fuzz circuit (clean signal with input gain and lowpass filter)");
    addAndMakeVisible(*mainBypassSwitch_);
    
    // Connect bypass switch to parameter
    mainBypassAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor_.getParameterState(), PluginParameters::MAIN_BYPASS_ID, *mainBypassSwitch_);
    
    // Create Input Gain control (positioned in top area)
    inputGainKnob_ = std::make_unique<ImageKnobComponent>();
    
    // Load the knob1 image
    juce::File knobImageFile("/Users/williamsmith/Desktop/Terminal_Fuzz_Plugin/knob1.png");
    if (knobImageFile.exists())
    {
        if (!inputGainKnob_->loadImage(knobImageFile))
        {
            DBG("Failed to load knob1 image for input gain");
        }
    }
    else
    {
        DBG("Knob1 image file not found: " << knobImageFile.getFullPathName());
    }
    
    addAndMakeVisible(*inputGainKnob_);
    
    // Get the input gain parameter
    inputGainParameter_ = dynamic_cast<juce::AudioParameterFloat*>(
        audioProcessor_.getParameterState().getParameter("input_gain"));
    
    // Configure the knob range
    inputGainKnob_->setRange(0.0f, 1.0f);
    
    // Set up knob callback with dynamic info display
    inputGainKnob_->onValueChange = [this](float value) {
        if (inputGainParameter_) {
            // Map knob position to dB value with 0dB at center
            float dbValue;
            if (value < 0.5f) {
                // Left side: -20dB to 0dB
                dbValue = -20.0f + (value * 2.0f * 20.0f);
            } else {
                // Right side: 0dB to +100dB
                dbValue = (value - 0.5f) * 2.0f * 100.0f;
            }
            
            // Update dynamic info display
            juce::String infoText = "IN " + juce::String(dbValue, 1) + " dB";
            updateDynamicInfo(infoText);
            
            // Convert to normalized parameter value
            float normalizedValue = inputGainParameter_->range.convertTo0to1(dbValue);
            inputGainParameter_->setValueNotifyingHost(normalizedValue);
        }
    };
    
    // Add mouse interaction for IN knob
    inputGainKnob_->onMouseEnter = [this]() {
        if (inputGainParameter_) {
            float dbValue = inputGainParameter_->get();
            updateDynamicInfo("IN " + juce::String(dbValue, 1) + " dB");
        }
    };
    inputGainKnob_->onMouseExit = [this]() {
        clearDynamicInfo();
    };
    
    // Set initial knob value from parameter
    if (inputGainParameter_) {
        float dbValue = inputGainParameter_->get();
        float knobValue;
        if (dbValue < 0.0f) {
            // Negative dB: map -20 to 0 -> knob 0 to 0.5
            knobValue = (dbValue + 20.0f) / 40.0f;
        } else {
            // Positive dB: map 0 to 100 -> knob 0.5 to 1
            knobValue = 0.5f + (dbValue / 200.0f);
        }
        inputGainKnob_->setValue(knobValue);
        
        // Initial display doesn't need static labels
    }
    
    // Create main control knobs (all positioned directly in editor)
    auto createKnobWithImage = [&knobImageFile](std::unique_ptr<ImageKnobComponent>& knob) {
        knob = std::make_unique<ImageKnobComponent>();
        if (knobImageFile.exists()) {
            knob->loadImage(knobImageFile);
        }
        knob->setRange(0.0f, 1.0f);
    };
    
    // Create Fuzz control
    createKnobWithImage(fuzzKnob_);
    fuzzLabel_ = std::make_unique<juce::Label>("", "FUZZ");
    fuzzValueLabel_ = std::make_unique<juce::Label>("", "50%");
    configureLabel(*fuzzLabel_);
    configureLabel(*fuzzValueLabel_, true);
    addAndMakeVisible(*fuzzKnob_);
    // Static labels removed - using dynamic info instead
    
    // Add mouse interaction for dynamic info
    fuzzKnob_->onMouseEnter = [this]() {
        if (fuzzParameter_) {
            int percentage = static_cast<int>(fuzzParameter_->get() * 100.0f);
            updateDynamicInfo("FUZZ " + juce::String(percentage) + "%");
        }
    };
    fuzzKnob_->onMouseExit = [this]() {
        clearDynamicInfo();
    };
    
    // Create Voice control (hidden but functional)
    createKnobWithImage(voiceKnob_);
    voiceLabel_ = std::make_unique<juce::Label>("", "VOICE");
    voiceValueLabel_ = std::make_unique<juce::Label>("", "50%");
    configureLabel(*voiceLabel_);
    configureLabel(*voiceValueLabel_, true);
    addAndMakeVisible(*voiceKnob_);
    // Hidden controls don't need mouse handlers
    
    // Create Treble control (hidden but functional)
    createKnobWithImage(trebleKnob_);
    trebleLabel_ = std::make_unique<juce::Label>("", "TREBLE");
    trebleValueLabel_ = std::make_unique<juce::Label>("", "50%");
    configureLabel(*trebleLabel_);
    configureLabel(*trebleValueLabel_, true);
    addAndMakeVisible(*trebleKnob_);
    // Hidden controls don't need mouse handlers
    
    // Create Level control
    createKnobWithImage(levelKnob_);
    levelLabel_ = std::make_unique<juce::Label>("", "LEVEL");
    levelValueLabel_ = std::make_unique<juce::Label>("", "50%");
    configureLabel(*levelLabel_);
    configureLabel(*levelValueLabel_, true);
    addAndMakeVisible(*levelKnob_);
    // Static labels removed - using dynamic info instead
    
    // Add mouse interaction for dynamic info
    levelKnob_->onMouseEnter = [this]() {
        if (levelParameter_) {
            int percentage = static_cast<int>(levelParameter_->get() * 100.0f);
            updateDynamicInfo("LEVEL " + juce::String(percentage) + "%");
        }
    };
    levelKnob_->onMouseExit = [this]() {
        clearDynamicInfo();
    };
    
    // Get parameters for main controls
    fuzzParameter_ = dynamic_cast<juce::AudioParameterFloat*>(
        audioProcessor_.getParameterState().getParameter("fuzz"));
    voiceParameter_ = dynamic_cast<juce::AudioParameterFloat*>(
        audioProcessor_.getParameterState().getParameter("voice"));
    trebleParameter_ = dynamic_cast<juce::AudioParameterFloat*>(
        audioProcessor_.getParameterState().getParameter("treble"));
    levelParameter_ = dynamic_cast<juce::AudioParameterFloat*>(
        audioProcessor_.getParameterState().getParameter("level"));
    
    // Set up main knob callbacks
    setupMainKnobCallbacks();
    
    // Start with front panel only
    showPhysicsPanel_ = false;
    updatePanelVisibility();
    
    // Set plugin window size to match enclosure aspect ratio (square)
    setSize(600, 600);
    
    // Make the editor maintain aspect ratio
    setResizable(true, true);
    setResizeLimits(400, 400, 1200, 1200);
    getConstrainer()->setFixedAspectRatio(1.0); // Square aspect ratio
    
    // Start timer for LED meter updates (30 FPS)
    startTimer(33);
}

PluginEditor::~PluginEditor() {
    stopTimer();
}

void PluginEditor::paint(juce::Graphics& g) {
    // Draw enclosure background image
    if (enclosureImage_.isValid())
    {
        // Maintain aspect ratio and center the image
        g.drawImageWithin(enclosureImage_, 0, 0, getWidth(), getHeight(), 
                         juce::RectanglePlacement::centred | 
                         juce::RectanglePlacement::onlyReduceInSize);
    }
    else
    {
        // Fallback background if image fails to load
        g.fillAll(juce::Colour(0xff2a2a2a));
    }
    
    // Draw drop shadows for all knobs first
    auto drawKnobShadow = [&g](const juce::Rectangle<int>& knobBounds) {
        auto shadowBounds = knobBounds.toFloat().expanded(6.0f); // Larger shadow for knobs
        auto shadowOffset = 5.0f; // Shadow offset below knob
        shadowBounds.translate(0, shadowOffset);
        
        // Create radial gradient shadow - darker center than LED shadow
        juce::ColourGradient shadowGradient(juce::Colours::black,  // Full black, no alpha reduction
                                           shadowBounds.getCentreX(), shadowBounds.getCentreY(),
                                           juce::Colours::transparentBlack,
                                           shadowBounds.getRight(), shadowBounds.getCentreY(),
                                           true); // Radial gradient
        
        // Add intermediate color stop for more concentrated darkness
        shadowGradient.addColour(0.3, juce::Colours::black.withAlpha(0.8f));
        
        g.setGradientFill(shadowGradient);
        g.fillEllipse(shadowBounds);
    };
    
    // Draw shadows for all visible knobs
    drawKnobShadow(inputGainKnob_->getBounds());
    drawKnobShadow(fuzzKnob_->getBounds());
    drawKnobShadow(levelKnob_->getBounds());
    
    // Draw LED input level indicator (image-based)
    if (!ledBounds_.isEmpty()) {
        // Draw drop shadow behind LED - gradient shadow
        auto shadowBounds = ledBounds_.expanded(4.0f); // Nice size shadow
        auto shadowOffset = 2.0f; // Higher shadow (reduced offset from 4.0f to 2.0f)
        shadowBounds.translate(0, shadowOffset);
        
        // Create radial gradient shadow - much darker center like knob shadows
        juce::ColourGradient shadowGradient(juce::Colours::black,  // Pure black center
                                           shadowBounds.getCentreX(), shadowBounds.getCentreY(),
                                           juce::Colours::transparentBlack,
                                           shadowBounds.getRight(), shadowBounds.getCentreY(),
                                           true); // Radial gradient
        
        // Add intermediate color stop for more concentrated darkness
        shadowGradient.addColour(0.3, juce::Colours::black.withAlpha(0.8f));
        
        g.setGradientFill(shadowGradient);
        g.fillEllipse(shadowBounds);
        
        // Reset fill to solid for LED drawing
        g.setFillType(juce::FillType(juce::Colours::white));
        
        // Check if bypass is active
        bool isBypassed = PluginParameters::getMainBypass(audioProcessor_.getParameterState());
        
        if (isBypassed) {
            // Show off LED when bypassed
            if (yellowLedOffImage_.isValid()) {
                g.setOpacity(1.0f);
                g.drawImageWithin(yellowLedOffImage_, 
                                static_cast<int>(ledBounds_.getX()), static_cast<int>(ledBounds_.getY()),
                                static_cast<int>(ledBounds_.getWidth()), static_cast<int>(ledBounds_.getHeight()),
                                juce::RectanglePlacement::centred);
            }
        } else {
            // Normal LED behavior when circuit is engaged
            // Draw yellow LED (base layer, always dimly visible)
            if (yellowLedImage_.isValid()) {
                g.setOpacity(0.3f + yellowLEDBrightness_ * 0.7f); // 30% base + up to 70% brightness
                g.drawImageWithin(yellowLedImage_, 
                                static_cast<int>(ledBounds_.getX()), static_cast<int>(ledBounds_.getY()),
                                static_cast<int>(ledBounds_.getWidth()), static_cast<int>(ledBounds_.getHeight()),
                                juce::RectanglePlacement::centred);
            }
            
            // Draw green LED (overlay)
            if (greenLedImage_.isValid() && greenLEDBrightness_ > 0.0f) {
                g.setOpacity(greenLEDBrightness_);
                g.drawImageWithin(greenLedImage_, 
                                static_cast<int>(ledBounds_.getX()), static_cast<int>(ledBounds_.getY()),
                                static_cast<int>(ledBounds_.getWidth()), static_cast<int>(ledBounds_.getHeight()),
                                juce::RectanglePlacement::centred);
            }
            
            // Draw red LED (overlay)
            if (redLedImage_.isValid() && redLEDBrightness_ > 0.0f) {
                g.setOpacity(redLEDBrightness_);
                g.drawImageWithin(redLedImage_, 
                                static_cast<int>(ledBounds_.getX()), static_cast<int>(ledBounds_.getY()),
                                static_cast<int>(ledBounds_.getWidth()), static_cast<int>(ledBounds_.getHeight()),
                                juce::RectanglePlacement::centred);
            }
        }
        
        // Reset opacity for other drawing operations
        g.setOpacity(1.0f);
    }
}

void PluginEditor::resized() {
    auto bounds = getLocalBounds();
    
    // Background is now painted directly
    
    // IN knob positioned at the actual top of the plugin window, moved more to the left
    auto knobSize = static_cast<int>(getWidth() * 0.12f); // 12% of window width
    auto knobX = static_cast<int>(getWidth() * 0.03f); // 3% from left edge (moved 5% more left)
    auto knobY = static_cast<int>(getHeight() * 0.05f); // 5% from actual top
    inputGainKnob_->setBounds(knobX, knobY, knobSize, knobSize);
    
    // No static labels for IN knob anymore
    
    // Position main control knobs directly relative to plugin window (no container constraints)
    auto mainKnobSize = static_cast<int>(getWidth() * 0.11f);
    
    // Fuzz knob in center, moved up 10% and made 30% bigger
    auto fuzzKnobSize = static_cast<int>(getWidth() * 0.215f); // 30% bigger than 0.165f (0.165f * 1.3 = 0.215f)
    auto fuzzCenterX = static_cast<int>(getWidth() * 0.5f - fuzzKnobSize * 0.5f);
    auto fuzzY = static_cast<int>(getHeight() * 0.15f); // Moved up 10% (from 25% to 15%)
    
    // Only position FUZZ knob (no static labels)
    fuzzKnob_->setBounds(fuzzCenterX, fuzzY, fuzzKnobSize, fuzzKnobSize);
    
    // Level control positioned at top right corner (symmetrical with IN knob on top left)
    auto levelKnobX = static_cast<int>(getWidth() * 0.97f - knobSize); // 3% from right edge (mirroring IN knob)
    auto levelKnobY = static_cast<int>(getHeight() * 0.05f); // 5% from top (same as IN knob)
    
    // Only position LEVEL knob (no static labels)
    levelKnob_->setBounds(levelKnobX, levelKnobY, knobSize, knobSize);
    
    // Hide voice and treble controls (set to invisible bounds)
    voiceKnob_->setBounds(0, 0, 0, 0);
    voiceLabel_->setBounds(0, 0, 0, 0);
    voiceValueLabel_->setBounds(0, 0, 0, 0);
    trebleKnob_->setBounds(0, 0, 0, 0);
    trebleLabel_->setBounds(0, 0, 0, 0);
    trebleValueLabel_->setBounds(0, 0, 0, 0);
    
    // Hide all static labels since we're using dynamic info
    fuzzLabel_->setBounds(0, 0, 0, 0);
    fuzzValueLabel_->setBounds(0, 0, 0, 0);
    levelLabel_->setBounds(0, 0, 0, 0);
    levelValueLabel_->setBounds(0, 0, 0, 0);
    
    // Position main bypass switch (5% higher up)
    auto switchSize = static_cast<int>(getWidth() * 0.144f); // 14.4% of window width (10% smaller)
    auto switchX = (getWidth() - switchSize) / 2;
    auto switchY = static_cast<int>(getHeight() * 0.85f - switchSize); // 15% up from bottom (5% higher)
    mainBypassSwitch_->setBounds(switchX, switchY, switchSize, switchSize);
    
    // Move physics panel button to bottom
    auto buttonWidth = 150;
    auto buttonHeight = 25;
    auto buttonX = (getWidth() - buttonWidth) / 2;
    auto buttonY = getHeight() - buttonHeight - 15;
    physicsPanelButton_->setBounds(buttonX, buttonY, buttonWidth, buttonHeight);
    
    // Position dynamic info display where physics button used to be (top center)
    auto infoWidth = 200;
    auto infoHeight = 30;
    auto infoX = (getWidth() - infoWidth) / 2;
    dynamicInfoLabel_->setBounds(infoX, 15, infoWidth, infoHeight);
    dynamicInfoLabel_->setVisible(false); // Hidden by default
    
    // Position LED input level indicator at center, 45% down from top
    auto ledSize = static_cast<int>(getWidth() * 0.06f); // 6% of window width
    auto ledX = (getWidth() - ledSize) / 2; // Centered horizontally
    auto ledY = static_cast<int>(getHeight() * 0.45f); // 45% down from top
    ledBounds_ = juce::Rectangle<float>(static_cast<float>(ledX), static_cast<float>(ledY), 
                                       static_cast<float>(ledSize), static_cast<float>(ledSize));
    
    // Physics panel uses the full window when visible
    physicsPanel_->setBounds(getLocalBounds());
}

void PluginEditor::updatePanelVisibility() {
    // Update button state
    physicsPanelButton_->setToggleState(showPhysicsPanel_, juce::dontSendNotification);
    
    // Show/hide physics panel only (main knobs are always visible in editor)
    physicsPanel_->setVisible(showPhysicsPanel_);
    // Front panel removed - LED handled directly in PluginEditor
    
    // Update button text to show current state
    physicsPanelButton_->setButtonText(showPhysicsPanel_ ? "🎛️ MAIN PANEL" : "🔬 PHYSICS PANEL");
}

void PluginEditor::timerCallback() {
    // Update LED meter with current input level from the circuit
    float inputLevelDb = audioProcessor_.getTerminalCircuit().getInputLevelDb();
    updateInputLevelMeter(inputLevelDb);
}

void PluginEditor::configureLabel(juce::Label& label, bool isValueLabel) {
    label.setJustificationType(juce::Justification::centred);
    if (isValueLabel) {
        label.setFont(juce::Font(11.0f));
        label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    } else {
        label.setFont(juce::Font(12.0f, juce::Font::bold));
        label.setColour(juce::Label::textColourId, juce::Colours::white);
    }
}

void PluginEditor::setupMainKnobCallbacks() {
    // Fuzz knob callback with dynamic info
    fuzzKnob_->onValueChange = [this](float value) {
        if (fuzzParameter_) {
            fuzzParameter_->setValueNotifyingHost(value);
            int percentage = static_cast<int>(value * 100.0f);
            updateDynamicInfo("FUZZ " + juce::String(percentage) + "%");
        }
    };
    
    // Voice knob callback (hidden but still functional)
    voiceKnob_->onValueChange = [this](float value) {
        if (voiceParameter_) {
            voiceParameter_->setValueNotifyingHost(value);
        }
    };
    
    // Treble knob callback (hidden but still functional)
    trebleKnob_->onValueChange = [this](float value) {
        if (trebleParameter_) {
            trebleParameter_->setValueNotifyingHost(value);
        }
    };
    
    // Level knob callback with dynamic info
    levelKnob_->onValueChange = [this](float value) {
        if (levelParameter_) {
            levelParameter_->setValueNotifyingHost(value);
            int percentage = static_cast<int>(value * 100.0f);
            updateDynamicInfo("LEVEL " + juce::String(percentage) + "%");
        }
    };
    
    // Set initial values from parameters
    if (fuzzParameter_) {
        float value = fuzzParameter_->get();
        fuzzKnob_->setValue(value);
    }
    
    if (voiceParameter_) {
        float value = voiceParameter_->get();
        voiceKnob_->setValue(value);
    }
    
    if (trebleParameter_) {
        float value = trebleParameter_->get();
        trebleKnob_->setValue(value);
    }
    
    if (levelParameter_) {
        float value = levelParameter_->get();
        levelKnob_->setValue(value);
    }
}

void PluginEditor::updateDynamicInfo(const juce::String& text) {
    dynamicInfoLabel_->setText(text, juce::dontSendNotification);
    dynamicInfoLabel_->setVisible(true);
}

void PluginEditor::clearDynamicInfo() {
    dynamicInfoLabel_->setVisible(false);
}

void PluginEditor::updateInputLevelMeter(float inputLevelDb) {
    // LED ranges: Yellow = below -16dB, Green = -16 to -10dB, Red = above -10dB
    // Same logic as FrontPanelComponent but using image-based LEDs
    
    if (inputLevelDb > -10.0f) {
        // Too hot - red LED
        redLEDTarget_ = 1.0f;
        greenLEDTarget_ = juce::jmax(0.0f, 1.0f - (inputLevelDb + 10.0f) / 5.0f); // Fade out green
        yellowLEDTarget_ = 0.0f;
    } else if (inputLevelDb >= -16.0f) {
        // Ideal range - green LED
        redLEDTarget_ = 0.0f;
        greenLEDTarget_ = 1.0f;
        yellowLEDTarget_ = 0.0f;
    } else {
        // Too quiet - yellow LED
        redLEDTarget_ = 0.0f;
        greenLEDTarget_ = juce::jmax(0.0f, 1.0f - (-16.0f - inputLevelDb) / 5.0f); // Fade out green  
        yellowLEDTarget_ = 1.0f;
    }
    
    // Quick fade between LED states
    float fadeSpeed = 0.15f; // Faster transition (original was 0.05f)
    redLEDBrightness_ += (redLEDTarget_ - redLEDBrightness_) * fadeSpeed;
    greenLEDBrightness_ += (greenLEDTarget_ - greenLEDBrightness_) * fadeSpeed;
    yellowLEDBrightness_ += (yellowLEDTarget_ - yellowLEDBrightness_) * fadeSpeed;
    
    repaint(); // Trigger redraw to update LED display
}

}  // namespace TerminalFuzz