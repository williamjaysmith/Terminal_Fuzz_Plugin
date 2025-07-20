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
    
    // Create panel components (only physics panel needed now since main knobs are in editor)
    physicsPanel_ = std::make_unique<GUI::PhysicsPanelComponent>(audioProcessor_.getParameterState());
    physicsPanel_->setVisible(false);
    addAndMakeVisible(*physicsPanel_);
    
    // Create front panel only for LED meter
    frontPanel_ = std::make_unique<GUI::FrontPanelComponent>(audioProcessor_.getParameterState());
    frontPanel_->setVisible(true);  // Only visible for LED meter
    addAndMakeVisible(*frontPanel_);
    
    // Create physics panel toggle button
    physicsPanelButton_ = std::make_unique<juce::TextButton>("Physics Panel");
    physicsPanelButton_->setButtonText("🔬 PHYSICS PANEL");
    physicsPanelButton_->setTooltip("Tune comprehensive transistor physics");
    physicsPanelButton_->onClick = [this]() {
        showPhysicsPanel_ = !showPhysicsPanel_;
        updatePanelVisibility();
    };
    addAndMakeVisible(*physicsPanelButton_);
    
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
    
    inputGainLabel_ = std::make_unique<juce::Label>("", "IN");
    inputGainLabel_->setJustificationType(juce::Justification::centred);
    inputGainLabel_->setFont(juce::Font(12.0f, juce::Font::bold));
    inputGainLabel_->setColour(juce::Label::textColourId, juce::Colours::white);
    
    inputGainValueLabel_ = std::make_unique<juce::Label>("", "0.0 dB");
    inputGainValueLabel_->setJustificationType(juce::Justification::centred);
    inputGainValueLabel_->setFont(juce::Font(11.0f));
    inputGainValueLabel_->setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    
    addAndMakeVisible(*inputGainKnob_);
    addAndMakeVisible(*inputGainLabel_);
    addAndMakeVisible(*inputGainValueLabel_);
    
    // Get the input gain parameter
    inputGainParameter_ = dynamic_cast<juce::AudioParameterFloat*>(
        audioProcessor_.getParameterState().getParameter("input_gain"));
    
    // Configure the knob range
    inputGainKnob_->setRange(0.0f, 1.0f);
    
    // Set up knob callback
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
            
            // Update value display
            juce::String valueText = juce::String(dbValue, 1) + " dB";
            inputGainValueLabel_->setText(valueText, juce::dontSendNotification);
            
            // Convert to normalized parameter value
            float normalizedValue = inputGainParameter_->range.convertTo0to1(dbValue);
            inputGainParameter_->setValueNotifyingHost(normalizedValue);
        }
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
        
        // Update value display
        juce::String valueText = juce::String(dbValue, 1) + " dB";
        inputGainValueLabel_->setText(valueText, juce::dontSendNotification);
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
    addAndMakeVisible(*fuzzLabel_);
    addAndMakeVisible(*fuzzValueLabel_);
    
    // Create Voice control
    createKnobWithImage(voiceKnob_);
    voiceLabel_ = std::make_unique<juce::Label>("", "VOICE");
    voiceValueLabel_ = std::make_unique<juce::Label>("", "50%");
    configureLabel(*voiceLabel_);
    configureLabel(*voiceValueLabel_, true);
    addAndMakeVisible(*voiceKnob_);
    addAndMakeVisible(*voiceLabel_);
    addAndMakeVisible(*voiceValueLabel_);
    
    // Create Treble control
    createKnobWithImage(trebleKnob_);
    trebleLabel_ = std::make_unique<juce::Label>("", "TREBLE");
    trebleValueLabel_ = std::make_unique<juce::Label>("", "50%");
    configureLabel(*trebleLabel_);
    configureLabel(*trebleValueLabel_, true);
    addAndMakeVisible(*trebleKnob_);
    addAndMakeVisible(*trebleLabel_);
    addAndMakeVisible(*trebleValueLabel_);
    
    // Create Level control
    createKnobWithImage(levelKnob_);
    levelLabel_ = std::make_unique<juce::Label>("", "LEVEL");
    levelValueLabel_ = std::make_unique<juce::Label>("", "50%");
    configureLabel(*levelLabel_);
    configureLabel(*levelValueLabel_, true);
    addAndMakeVisible(*levelKnob_);
    addAndMakeVisible(*levelLabel_);
    addAndMakeVisible(*levelValueLabel_);
    
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
}

void PluginEditor::resized() {
    auto bounds = getLocalBounds();
    
    // Background is now painted directly
    
    // IN knob positioned at the actual top of the plugin window, moved more to the left
    auto knobSize = static_cast<int>(getWidth() * 0.12f); // 12% of window width
    auto knobX = static_cast<int>(getWidth() * 0.03f); // 3% from left edge (moved 5% more left)
    auto knobY = static_cast<int>(getHeight() * 0.05f); // 5% from actual top
    inputGainKnob_->setBounds(knobX, knobY, knobSize, knobSize);
    
    // Position labels below the IN knob
    auto labelHeight = static_cast<int>(getHeight() * 0.04f);
    inputGainLabel_->setBounds(knobX, knobY + knobSize + 3, knobSize, labelHeight);
    inputGainValueLabel_->setBounds(knobX, knobY + knobSize + 3 + labelHeight, knobSize, labelHeight);
    
    // Position main control knobs directly relative to plugin window (no container constraints)
    auto mainKnobSize = static_cast<int>(getWidth() * 0.11f);
    
    // Fuzz knob in center, moved up 10% and made 1.5x bigger (half of 3x)
    auto fuzzKnobSize = static_cast<int>(getWidth() * 0.165f); // 1.5x bigger (0.33f / 2 = 0.165f)
    auto fuzzCenterX = static_cast<int>(getWidth() * 0.5f - fuzzKnobSize * 0.5f);
    auto fuzzY = static_cast<int>(getHeight() * 0.15f); // Moved up 10% (from 25% to 15%)
    
    // Position FUZZ label above the knob and percentage below - as close as possible
    fuzzLabel_->setBounds(fuzzCenterX, fuzzY - labelHeight - 1, fuzzKnobSize, labelHeight);
    fuzzKnob_->setBounds(fuzzCenterX, fuzzY, fuzzKnobSize, fuzzKnobSize);
    fuzzValueLabel_->setBounds(fuzzCenterX, fuzzY + fuzzKnobSize + 1, fuzzKnobSize, labelHeight);
    
    // Other controls in grid around the bottom area
    auto leftX = static_cast<int>(getWidth() * 0.25f);
    auto rightX = static_cast<int>(getWidth() * 0.75f - mainKnobSize);
    auto bottomY = static_cast<int>(getHeight() * 0.65f);
    
    // Voice control (top right)
    voiceKnob_->setBounds(rightX, fuzzY, mainKnobSize, mainKnobSize);
    voiceLabel_->setBounds(rightX, fuzzY + mainKnobSize + 3, mainKnobSize, labelHeight);
    voiceValueLabel_->setBounds(rightX, fuzzY + mainKnobSize + 3 + labelHeight, mainKnobSize, labelHeight);
    
    // Treble control (bottom left)
    trebleKnob_->setBounds(leftX, bottomY, mainKnobSize, mainKnobSize);
    trebleLabel_->setBounds(leftX, bottomY + mainKnobSize + 3, mainKnobSize, labelHeight);
    trebleValueLabel_->setBounds(leftX, bottomY + mainKnobSize + 3 + labelHeight, mainKnobSize, labelHeight);
    
    // Level control (bottom right)
    levelKnob_->setBounds(rightX, bottomY, mainKnobSize, mainKnobSize);
    levelLabel_->setBounds(rightX, bottomY + mainKnobSize + 3, mainKnobSize, labelHeight);
    levelValueLabel_->setBounds(rightX, bottomY + mainKnobSize + 3 + labelHeight, mainKnobSize, labelHeight);
    
    // The enclosure has a dark top section and light bottom section
    // Physics panel toggle button in dark top section
    auto buttonWidth = 150;
    auto buttonHeight = 25;
    auto buttonX = (getWidth() - buttonWidth) / 2;
    physicsPanelButton_->setBounds(buttonX, 15, buttonWidth, buttonHeight);
    
    // Front panel uses the light bottom section of the enclosure
    // The enclosure image shows the light section starts about halfway down
    auto panelBounds = bounds;
    panelBounds.removeFromTop(getHeight() * 0.48f);  // Dark top section
    panelBounds.removeFromBottom(getHeight() * 0.05f); // Bottom margin
    panelBounds.reduce(getWidth() * 0.05f, 0);  // Side margins proportional to width
    
    frontPanel_->setBounds(panelBounds);
    physicsPanel_->setBounds(panelBounds);
}

void PluginEditor::updatePanelVisibility() {
    // Update button state
    physicsPanelButton_->setToggleState(showPhysicsPanel_, juce::dontSendNotification);
    
    // Show/hide physics panel only (main knobs are always visible in editor)
    physicsPanel_->setVisible(showPhysicsPanel_);
    // Keep front panel visible for LED meter only
    frontPanel_->setVisible(!showPhysicsPanel_);
    
    // Update button text to show current state
    physicsPanelButton_->setButtonText(showPhysicsPanel_ ? "🎛️ MAIN PANEL" : "🔬 PHYSICS PANEL");
}

void PluginEditor::timerCallback() {
    // Update LED meter with current input level from the circuit
    if (frontPanel_ && frontPanel_->isVisible()) {
        float inputLevelDb = audioProcessor_.getTerminalCircuit().getInputLevelDb();
        frontPanel_->updateInputLevelMeter(inputLevelDb);
    }
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
    // Fuzz knob callback
    fuzzKnob_->onValueChange = [this](float value) {
        if (fuzzParameter_) {
            fuzzParameter_->setValueNotifyingHost(value);
            int percentage = static_cast<int>(value * 100.0f);
            fuzzValueLabel_->setText(juce::String(percentage) + "%", juce::dontSendNotification);
        }
    };
    
    // Voice knob callback
    voiceKnob_->onValueChange = [this](float value) {
        if (voiceParameter_) {
            voiceParameter_->setValueNotifyingHost(value);
            int percentage = static_cast<int>(value * 100.0f);
            voiceValueLabel_->setText(juce::String(percentage) + "%", juce::dontSendNotification);
        }
    };
    
    // Treble knob callback
    trebleKnob_->onValueChange = [this](float value) {
        if (trebleParameter_) {
            trebleParameter_->setValueNotifyingHost(value);
            int percentage = static_cast<int>(value * 100.0f);
            trebleValueLabel_->setText(juce::String(percentage) + "%", juce::dontSendNotification);
        }
    };
    
    // Level knob callback
    levelKnob_->onValueChange = [this](float value) {
        if (levelParameter_) {
            levelParameter_->setValueNotifyingHost(value);
            int percentage = static_cast<int>(value * 100.0f);
            levelValueLabel_->setText(juce::String(percentage) + "%", juce::dontSendNotification);
        }
    };
    
    // Set initial values from parameters
    if (fuzzParameter_) {
        float value = fuzzParameter_->get();
        fuzzKnob_->setValue(value);
        int percentage = static_cast<int>(value * 100.0f);
        fuzzValueLabel_->setText(juce::String(percentage) + "%", juce::dontSendNotification);
    }
    
    if (voiceParameter_) {
        float value = voiceParameter_->get();
        voiceKnob_->setValue(value);
        int percentage = static_cast<int>(value * 100.0f);
        voiceValueLabel_->setText(juce::String(percentage) + "%", juce::dontSendNotification);
    }
    
    if (trebleParameter_) {
        float value = trebleParameter_->get();
        trebleKnob_->setValue(value);
        int percentage = static_cast<int>(value * 100.0f);
        trebleValueLabel_->setText(juce::String(percentage) + "%", juce::dontSendNotification);
    }
    
    if (levelParameter_) {
        float value = levelParameter_->get();
        levelKnob_->setValue(value);
        int percentage = static_cast<int>(value * 100.0f);
        levelValueLabel_->setText(juce::String(percentage) + "%", juce::dontSendNotification);
    }
}

}  // namespace TerminalFuzz