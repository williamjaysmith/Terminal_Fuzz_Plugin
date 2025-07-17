#include "TerminalCircuit.h"
#include <iostream>
#include <cassert>
#include <cmath>

namespace TerminalFuzz {
namespace DSP {
namespace Tests {

class TerminalCircuitTests {
public:
    static void runAllTests() {
        std::cout << "Running Terminal Circuit DSP Tests...\n";
        
        testBasicSignalFlow();
        testParameterRanges();
        testDCBlocking();
        testOutputLevels();
        testFrequencyResponse();
        testComponentValues();
        
        std::cout << "All tests completed!\n";
    }
    
private:
    static void testBasicSignalFlow() {
        std::cout << "Testing basic signal flow... ";
        
        TerminalCircuit circuit;
        circuit.prepare(44100.0, 512);
        
        // Test with default component values
        TerminalCircuit::ComponentValues defaultComponents;
        initializeDefaultComponents(defaultComponents);
        
        // Test silence in = silence out
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        
        circuit.processBlock(buffer, 0.5f, 0.5f, 0.5f, 0.5f, defaultComponents);
        
        // Check if output is still zero (should be approximately zero)
        float maxOutput = buffer.getMagnitude(0, 0, 512);
        assert(maxOutput < 0.001f && "Silent input should produce near-silent output");
        
        // Test with sine wave input
        buffer.clear();
        for (int i = 0; i < 512; ++i) {
            float sineValue = std::sin(2.0f * M_PI * 440.0f * i / 44100.0f) * 0.1f;
            buffer.setSample(0, i, sineValue);
            buffer.setSample(1, i, sineValue);
        }
        
        circuit.processBlock(buffer, 0.5f, 0.5f, 0.5f, 0.8f, defaultComponents);
        
        // Check if we get some output
        maxOutput = buffer.getMagnitude(0, 0, 512);
        assert(maxOutput > 0.001f && "Sine wave input should produce audible output");
        assert(maxOutput < 10.0f && "Output should not be excessively loud");
        
        std::cout << "PASSED (max output: " << maxOutput << ")\n";
    }
    
    static void testParameterRanges() {
        std::cout << "Testing parameter ranges... ";
        
        TerminalCircuit circuit;
        circuit.prepare(44100.0, 512);
        
        TerminalCircuit::ComponentValues components;
        initializeDefaultComponents(components);
        
        juce::AudioBuffer<float> buffer(2, 64);
        for (int i = 0; i < 64; ++i) {
            buffer.setSample(0, i, 0.1f * std::sin(2.0f * M_PI * i / 64.0f));
            buffer.setSample(1, i, 0.1f * std::sin(2.0f * M_PI * i / 64.0f));
        }
        
        // Test extreme parameter values
        circuit.processBlock(buffer, 0.0f, 0.0f, 0.0f, 1.0f, components);
        circuit.processBlock(buffer, 1.0f, 1.0f, 1.0f, 1.0f, components);
        
        // Check output is within reasonable bounds
        float maxOutput = buffer.getMagnitude(0, 0, 64);
        assert(maxOutput < 5.0f && "Extreme parameters should not cause excessive output");
        
        std::cout << "PASSED\n";
    }
    
    static void testDCBlocking() {
        std::cout << "Testing DC blocking... ";
        
        TerminalCircuit circuit;
        circuit.prepare(44100.0, 512);
        
        TerminalCircuit::ComponentValues components;
        initializeDefaultComponents(components);
        
        // Test with DC offset
        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();
        for (int i = 0; i < 512; ++i) {
            buffer.setSample(0, i, 0.5f); // DC offset
            buffer.setSample(1, i, 0.5f);
        }
        
        circuit.processBlock(buffer, 0.5f, 0.5f, 0.5f, 0.5f, components);
        
        // Calculate DC component
        float dcSum = 0.0f;
        for (int i = 0; i < 512; ++i) {
            dcSum += buffer.getSample(0, i);
        }
        float dcAverage = dcSum / 512.0f;
        
        // DC should be significantly reduced
        assert(std::abs(dcAverage) < 0.1f && "DC blocking should reduce DC component");
        
        std::cout << "PASSED (DC average: " << dcAverage << ")\n";
    }
    
    static void testOutputLevels() {
        std::cout << "Testing output levels... ";
        
        TerminalCircuit circuit;
        circuit.prepare(44100.0, 512);
        
        TerminalCircuit::ComponentValues components;
        initializeDefaultComponents(components);
        
        // Test various input levels
        for (float inputLevel : {0.01f, 0.1f, 0.5f, 1.0f}) {
            juce::AudioBuffer<float> buffer(2, 512);
            buffer.clear();
            
            for (int i = 0; i < 512; ++i) {
                float sineValue = inputLevel * std::sin(2.0f * M_PI * 440.0f * i / 44100.0f);
                buffer.setSample(0, i, sineValue);
                buffer.setSample(1, i, sineValue);
            }
            
            circuit.processBlock(buffer, 0.5f, 0.5f, 0.5f, 0.5f, components);
            
            float maxOutput = buffer.getMagnitude(0, 0, 512);
            assert(maxOutput < 2.0f && "Output should not exceed reasonable levels");
            
            std::cout << "Input: " << inputLevel << " -> Output: " << maxOutput << " ";
        }
        
        std::cout << "PASSED\n";
    }
    
    static void testFrequencyResponse() {
        std::cout << "Testing frequency response... ";
        
        TerminalCircuit circuit;
        circuit.prepare(44100.0, 512);
        
        TerminalCircuit::ComponentValues components;
        initializeDefaultComponents(components);
        
        // Test different frequencies
        for (float freq : {100.0f, 440.0f, 1000.0f, 4000.0f}) {
            juce::AudioBuffer<float> buffer(2, 512);
            buffer.clear();
            
            for (int i = 0; i < 512; ++i) {
                float sineValue = 0.1f * std::sin(2.0f * M_PI * freq * i / 44100.0f);
                buffer.setSample(0, i, sineValue);
                buffer.setSample(1, i, sineValue);
            }
            
            circuit.processBlock(buffer, 0.5f, 0.5f, 0.5f, 0.5f, components);
            
            float maxOutput = buffer.getMagnitude(0, 0, 512);
            assert(maxOutput > 0.001f && "All frequencies should produce some output");
            
            std::cout << freq << "Hz: " << maxOutput << " ";
        }
        
        std::cout << "PASSED\n";
    }
    
    static void testComponentValues() {
        std::cout << "Testing component value validation... ";
        
        TerminalCircuit circuit;
        circuit.prepare(44100.0, 512);
        
        // Test with invalid component values
        TerminalCircuit::ComponentValues invalidComponents;
        initializeDefaultComponents(invalidComponents);
        
        // Test with zero values (should not crash)
        invalidComponents.c1 = 0.0f;
        invalidComponents.r1 = 0.0f;
        
        juce::AudioBuffer<float> buffer(2, 64);
        for (int i = 0; i < 64; ++i) {
            buffer.setSample(0, i, 0.1f * std::sin(2.0f * M_PI * i / 64.0f));
            buffer.setSample(1, i, 0.1f * std::sin(2.0f * M_PI * i / 64.0f));
        }
        
        // Should not crash with invalid values
        circuit.processBlock(buffer, 0.5f, 0.5f, 0.5f, 0.5f, invalidComponents);
        
        std::cout << "PASSED\n";
    }
    
    static void initializeDefaultComponents(TerminalCircuit::ComponentValues& cv) {
        // Initialize with schematic values
        cv.r1 = 2200000.0f;  // 2.2MΩ
        cv.r2 = 22000.0f;    // 22kΩ
        cv.r3 = 1000000.0f;  // 1MΩ
        cv.r4 = 47000.0f;    // 47kΩ
        cv.r5 = 10000.0f;    // 10kΩ
        cv.r6 = 15000.0f;    // 15kΩ
        cv.r7 = 47000.0f;    // 47kΩ
        cv.r8 = 470000.0f;   // 470kΩ
        cv.r9 = 470.0f;      // 470Ω
        cv.r10 = 10000.0f;   // 10kΩ
        cv.r11 = 100000.0f;  // 100kΩ
        cv.r12 = 4700.0f;    // 4.7kΩ
        cv.r13 = 1000000.0f; // 1MΩ
        
        cv.c1 = 100e-9f;     // 100nF
        cv.c2 = 1e-9f;       // 1nF
        cv.c3 = 47e-9f;      // 47nF
        cv.c4 = 2.2e-9f;     // 2.2nF
        cv.c5 = 3.3e-9f;     // 3.3nF
        cv.c6 = 1e-9f;       // 1nF
        cv.c7 = 100e-9f;     // 100nF
        cv.c8 = 10e-6f;      // 10μF
        cv.c9 = 10e-6f;      // 10μF
        cv.c10 = 100e-6f;    // 100μF
    }
};

// Function to run tests from main or test harness
void runTerminalCircuitTests() {
    TerminalCircuitTests::runAllTests();
}

}  // namespace Tests
}  // namespace DSP
}  // namespace TerminalFuzz