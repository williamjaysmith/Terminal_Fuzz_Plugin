#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace TerminalFuzz {

PluginProcessor::PluginProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
      parameters_(*this, nullptr, juce::Identifier("TerminalFuzzParameters"),
                  PluginParameters::createParameterLayout())
{
    // DEBUG: Log plugin construction
    FILE* debug_file = fopen("/tmp/terminal_bjt_debug.log", "a");
    if (debug_file) {
        fprintf(debug_file, "=== PLUGIN CONSTRUCTED v3.80-PATH-DEBUG ===\n");
        fclose(debug_file);
    }
    // Initialize component values to schematic defaults
    componentValues_.r1 = 2200000.0f;  // 2.2MΩ
    componentValues_.r2 = 22000.0f;    // 22kΩ
    componentValues_.r3 = 1000000.0f;  // 1MΩ
    componentValues_.r4 = 47000.0f;    // 47kΩ
    componentValues_.r5 = 10000.0f;    // 10kΩ
    componentValues_.r6 = 15000.0f;    // 15kΩ
    componentValues_.r7 = 47000.0f;    // 47kΩ
    componentValues_.r8 = 470000.0f;   // 470kΩ
    componentValues_.r9 = 470.0f;      // 470Ω
    componentValues_.r10 = 10000.0f;   // 10kΩ
    componentValues_.r11 = 100000.0f;  // 100kΩ
    componentValues_.r12 = 4700.0f;    // 4.7kΩ
    componentValues_.r13 = 1000000.0f; // 1MΩ
    
    componentValues_.c1 = 100e-9f;     // 100nF
    componentValues_.c2 = 1e-9f;       // 1nF
    componentValues_.c3 = 47e-9f;      // 47nF (critical)
    componentValues_.c4 = 2.2e-9f;     // 2.2nF
    componentValues_.c5 = 3.3e-9f;     // 3.3nF
    componentValues_.c6 = 1e-9f;       // 1nF
    componentValues_.c7 = 100e-9f;     // 100nF
    componentValues_.c8 = 10e-6f;      // 10μF
    componentValues_.c9 = 10e-6f;      // 10μF
    componentValues_.c10 = 100e-6f;    // 100μF
    
    // Initialize comprehensive transistor models with authentic characteristics
    
    // Q1: 2N3904 NPN Output Buffer (tuned for stable output)
    componentValues_.q1_model.hfe = 120.0f;            // Moderate gain for clean buffer (was 200)
    componentValues_.q1_model.vbe_on = 0.65f;          // Base-emitter turn-on voltage
    componentValues_.q1_model.vce_sat = 0.2f;          // Collector-emitter saturation voltage
    componentValues_.q1_model.is = 6.734e-15f;         // 2N3904 saturation current
    // Only basic transistor parameters are available
    
    // Q2: 2N2369 NPN Main Fuzz Stage (tuned for balanced fuzz/sustain)
    componentValues_.q2_model.hfe = 50.0f;             // Sweet spot vintage 2N2369 (was 45)
    componentValues_.q2_model.vbe_on = 0.63f;          // Back to stable threshold (was 0.61f)
    componentValues_.q2_model.vce_sat = 0.32f;         // Higher saturation (more distortion)
    componentValues_.q2_model.is = 1.2e-14f;           // Slightly different saturation current
    // Only basic transistor parameters are available
    
    // Q3: 2N2369 NPN Second Gain Stage (tuned for character)
    componentValues_.q3_model.hfe = 65.0f;             // Sweet spot vintage 2N2369 (was 60)
    componentValues_.q3_model.vbe_on = 0.66f;          // Back to stable threshold (was 0.63f)
    componentValues_.q3_model.vce_sat = 0.28f;         // Different saturation
    componentValues_.q3_model.is = 0.8e-14f;           // Different saturation current
    // Only basic transistor parameters are available
    
    // Note: Legacy gain/bias parameters are handled through TransistorModel structs
    // Note: Diode parameters are not part of the current ComponentValues struct
    
    // Initialize switch states
    componentValues_.bypass_mode = false;   // 3PDT bypass off
}

PluginProcessor::~PluginProcessor() {
}

const juce::String PluginProcessor::getName() const {
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const {
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const {
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const {
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const {
    return 0.0;
}


int PluginProcessor::getNumPrograms() {
    return 1;
}

int PluginProcessor::getCurrentProgram() {
    return 0;
}

void PluginProcessor::setCurrentProgram(int index) {
    juce::ignoreUnused(index);
}

const juce::String PluginProcessor::getProgramName(int index) {
    juce::ignoreUnused(index);
    return "Default";
}

void PluginProcessor::changeProgramName(int index, const juce::String& newName) {
    juce::ignoreUnused(index, newName);
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // DEBUG: Log prepareToPlay
    FILE* debug_file = fopen("/tmp/terminal_bjt_debug.log", "a");
    if (debug_file) {
        fprintf(debug_file, "=== PREPARE TO PLAY: sampleRate=%.1f, blockSize=%d ===\n", sampleRate, samplesPerBlock);
        fclose(debug_file);
    }
    
    // Prepare the Terminal circuit
    terminalCircuit_.prepare(sampleRate, samplesPerBlock);
    
    // Prepare DC blocker (safety measure)
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    
    dcBlocker_.prepare(spec);
    *dcBlocker_.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0f);
    
    updateParameters();
}

void PluginProcessor::releaseResources() {
    terminalCircuit_.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) {
        return false;
    }

    // Allow mono input with stereo output for fuzz effect
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo()) {
        return false;
    }

    return true;
  #endif
}
#endif

void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    juce::ignoreUnused(midiMessages);

    // DEBUG: Log main processBlock calls and INPUT LEVELS
    static int main_processBlock_count = 0;
    main_processBlock_count++;
    if (main_processBlock_count <= 10) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/terminal_bjt_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "=== MAIN PROCESSBLOCK #%d: %d channels, %d samples ===\n", 
                   main_processBlock_count, buffer.getNumChannels(), buffer.getNumSamples());
            
            // Check actual input levels
            if (buffer.getNumSamples() > 0 && buffer.getNumChannels() > 0) {
                float inputLevel = 0.0f;
                for (int sample = 0; sample < juce::jmin(10, buffer.getNumSamples()); sample++) {
                    float sampleValue = buffer.getSample(0, sample);
                    inputLevel = juce::jmax(inputLevel, std::abs(sampleValue));
                }
                fprintf(debug_file, "INPUT SIGNAL LEVEL: %.6f\n", inputLevel);
                if (inputLevel < 0.000001f) {
                    fprintf(debug_file, "*** NO INPUT SIGNAL DETECTED ***\n");
                } else {
                    fprintf(debug_file, "*** INPUT SIGNAL PRESENT! ***\n");
                }
            }
            fclose(debug_file);
        }
    }

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // DEBUG: Check channel counts before clearing
    static int clear_debug = 0;
    clear_debug++;
    if (clear_debug <= 3) {
        FILE* debug_file = fopen("/Users/williamsmith/Desktop/buffer_write_debug.log", "a");
        if (debug_file) {
            fprintf(debug_file, "=== CHANNEL CLEAR CHECK #%d ===\n", clear_debug);
            fprintf(debug_file, "Input channels: %d\n", totalNumInputChannels);
            fprintf(debug_file, "Output channels: %d\n", totalNumOutputChannels);
            fprintf(debug_file, "Buffer channels: %d\n", buffer.getNumChannels());
            fprintf(debug_file, "Will clear channels %d to %d\n", totalNumInputChannels, totalNumOutputChannels-1);
            fclose(debug_file);
        }
    }
    
    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        if (clear_debug <= 3) {
            FILE* debug_file = fopen("/Users/williamsmith/Desktop/buffer_write_debug.log", "a");
            if (debug_file) {
                fprintf(debug_file, "CLEARING CHANNEL %d!\n", i);
                fclose(debug_file);
            }
        }
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // Update parameters
    updateParameters();

    // Process through Terminal circuit with manual transistor gains and bypass controls
    terminalCircuit_.processBlock(buffer, inputGainDb_, fuzzAmount_, voiceAmount_, 
                                 trebleAmount_, levelAmount_, q1ManualGain_, q2ManualGain_, q3ManualGain_,
                                 q1Bypass_, q2Bypass_, q3Bypass_, componentValues_);
    
    // FIXED: Use actual circuit output (not forced test signal)
    // Circuit calculations are working perfectly - 0.287V output confirmed
    
}

void PluginProcessor::updateParameters() {
    // Update front panel controls
    inputGainDb_ = PluginParameters::getInputGain(parameters_);
    fuzzAmount_ = PluginParameters::getFuzz(parameters_);
    voiceAmount_ = PluginParameters::getVoice(parameters_);
    trebleAmount_ = PluginParameters::getTreble(parameters_);
    levelAmount_ = PluginParameters::getLevel(parameters_);
    
    // Update manual transistor gain controls
    q1ManualGain_ = PluginParameters::getQ1ManualGain(parameters_);
    q2ManualGain_ = PluginParameters::getQ2ManualGain(parameters_);
    q3ManualGain_ = PluginParameters::getQ3ManualGain(parameters_);
    
    // Update transistor bypass controls
    q1Bypass_ = PluginParameters::getQ1Bypass(parameters_);
    q2Bypass_ = PluginParameters::getQ2Bypass(parameters_);
    q3Bypass_ = PluginParameters::getQ3Bypass(parameters_);
    
    // Update component values (for back panel tweaking)
    componentValues_.r1 = PluginParameters::getR1(parameters_);
    componentValues_.r2 = PluginParameters::getR2(parameters_);
    componentValues_.r3 = PluginParameters::getR3(parameters_);
    componentValues_.r4 = PluginParameters::getR4(parameters_);
    componentValues_.r5 = PluginParameters::getR5(parameters_);
    componentValues_.r6 = PluginParameters::getR6(parameters_);
    componentValues_.r7 = PluginParameters::getR7(parameters_);
    componentValues_.r8 = PluginParameters::getR8(parameters_);
    componentValues_.r9 = PluginParameters::getR9(parameters_);
    componentValues_.r10 = PluginParameters::getR10(parameters_);
    componentValues_.r11 = PluginParameters::getR11(parameters_);
    componentValues_.r12 = PluginParameters::getR12(parameters_);
    componentValues_.r13 = PluginParameters::getR13(parameters_);
    
    componentValues_.c1 = PluginParameters::getC1(parameters_);
    componentValues_.c2 = PluginParameters::getC2(parameters_);
    componentValues_.c3 = PluginParameters::getC3(parameters_);
    componentValues_.c4 = PluginParameters::getC4(parameters_);
    componentValues_.c5 = PluginParameters::getC5(parameters_);
    componentValues_.c6 = PluginParameters::getC6(parameters_);
    componentValues_.c7 = PluginParameters::getC7(parameters_);
    componentValues_.c8 = PluginParameters::getC8(parameters_);
    componentValues_.c9 = PluginParameters::getC9(parameters_);
    componentValues_.c10 = PluginParameters::getC10(parameters_);
    
    // Note: Transistor characteristics are set through TransistorModel structs in constructor
    
    // Note: Diode characteristics are not part of current ComponentValues struct
    
    // Update switch states
    componentValues_.bypass_mode = PluginParameters::getBypassMode(parameters_);
    
    // Update physics boolean flags (only these exist in PhysicsControls struct)
    componentValues_.physics.tone_bypass = PluginParameters::getToneBypass(parameters_);
    componentValues_.physics.harmonics_enabled = PluginParameters::getHarmonicsEnabled(parameters_);
    componentValues_.physics.harmonic_filtering = PluginParameters::getHarmonicFiltering(parameters_);
    componentValues_.physics.bit_crushing_enabled = PluginParameters::getBitCrushingEnabled(parameters_);
    componentValues_.physics.frequency_doubling_enabled = PluginParameters::getFrequencyDoublingEnabled(parameters_);
    componentValues_.physics.controlled_gating_enabled = PluginParameters::getControlledGatingEnabled(parameters_);
    componentValues_.physics.advanced_compression_enabled = PluginParameters::getAdvancedCompressionEnabled(parameters_);
    componentValues_.physics.bias_drift_enabled = PluginParameters::getBiasDriftEnabled(parameters_);
    componentValues_.physics.envelope_bias_enabled = PluginParameters::getEnvelopeBiasEnabled(parameters_);
    
    // REMOVED: All anti-gating parameter updates - user found them ineffective
    // All anti-gating effects remain hardcoded in circuit as originally designed
}

bool PluginProcessor::hasEditor() const {
    return true;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor() {
    return new PluginEditor(*this);
}

void PluginProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = parameters_.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr) {
        if (xmlState->hasTagName(parameters_.state.getType())) {
            parameters_.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

}  // namespace TerminalFuzz

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new TerminalFuzz::PluginProcessor();
}