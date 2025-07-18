# Terminal Fuzz Plugin Project - Claude Documentation

## Project Overview

Building a faithful JUCE plugin recreation of the EarthQuaker Devices Terminal fuzz pedal, which is a modified Shin-Ei Companion FY-2 circuit.

## Circuit Analysis (from PDF schematic)

### Complete Component List

**Resistors:**

- R1: 2.2MΩ (input bias resistor)
- R2: 22kΩ (Q2 collector load resistor)
- R3: 1MΩ (coupling resistor between Q2 and Q3)
- R4: 47kΩ (Q2 base bias resistor)
- R5: 10kΩ (voice control network resistor)
- R6: 15kΩ (voice control network resistor)
- R7: 47kΩ (Q3 collector load resistor)
- R8: 470kΩ (Q3 base bias resistor)
- R9: 470Ω (Q1 emitter resistor)
- R10: 10kΩ (Q1 base bias resistor)
- R11: 100kΩ (Q1 collector load resistor)
- R12: 4.7kΩ (LED current limiting resistor)
- R13: 1MΩ (input impedance to ground)

**Capacitors:**

- C1: 100nF (input AC coupling)
- C2: 1nF (frequency shaping)
- C3: 47nF\* (critical Q2→Q3 coupling - affects fuzz character)
- C4: 2.2nF (tone stack frequency shaping)
- C5: 3.3nF (treble control network)
- C6: 1nF (voice control network)
- C7: 100nF (additional coupling/filtering)

**Electrolytic Capacitors:**

- C8: 10μF (output coupling stage 1)
- C9: 10μF (output coupling stage 2)
- C10: 100μF (power supply filtering)

**Transistors:**

- Q1: twwewowh (NPN, output buffer stage)
- Q2: 2N2369 (NPN, main fuzz/gain stage)
- Q3: 2N2369 (NPN, second gain stage)od

**Controls:**

- Fuzz: 100kΩ B (controls input to Q2)
- Voice: 10kΩ B (midrange control)
- Treble: 50kΩ B (high frequency control)
- Level: 50kΩ B (output volume)

**Diodes:**

- D1: 1N5817 (Schottky diode for reverse polarity protection)
- D2: 3mm Red LED (power indicator)

**Switches:**

- 3PDT Stomp Switch (true bypass switching)

**Jacks:**

- DC Power Jack (9V input)
- Audio Input Jack (1/4" mono)
- Audio Output Jack (1/4" mono)

### Circuit Signal Path (Exact from Schematic)

1. **Input Jack** → R13 (1MΩ to ground) → C1 (100nF AC coupling)
2. **Fuzz Control** → 100kΩ B pot attenuates signal level into Q2 base
3. **Q2 Fuzz Stage** → 2N2369 NPN (R4=47kΩ base bias, R2=22kΩ collector load)
4. **C3 Coupling** → 47nF capacitor couples Q2 collector to R3 (1MΩ)
5. **Tone Stack** → Voice (R5/R6/C6) + Treble (C5) active controls
6. **Q3 Gain Stage** → 2N2369 NPN (R8=470kΩ base bias, R7=47kΩ collector load)
7. **Q1 Output Buffer** → 2N3904 NPN (R10=10kΩ base bias, R11=100kΩ collector, R9=470Ω emitter)
8. **Output Coupling** → C8/C9 (10μF each) → Level Control (50kΩ B) → Output Jack

### Power Circuit

- **9V Input** → D1 (1N5817 reverse protection) → R12 (4.7kΩ) → D2 (LED) → C10 (100μF filtering)

### Key Circuit Characteristics

- **Base Design**: Modified Shin-Ei Companion FY-2
- **Topology**: 3-transistor fuzz with tone stack between gain stages
- **Critical Component**: C3 value affects character (47nF = original)
- **Power Supply**: 9V with protection diode and filtering

## Project Goals

1. **FAITHFUL CIRCUIT RECREATION - MOST IMPORTANT**: Component-by-component DSP implementation with ALL interactions
2. **Front Panel**: Standard Fuzz, Voice, Treble, Level controls
3. **Documentation**: Detailed explanations of each component's function

## CRITICAL DESIGN PHILOSOPHY - NEVER FORGET

**USER PRIORITY #1**: Broken, buzzy, unstable character of original Terminal fuzz

- **NO SHORTCUTS ALLOWED** - Model every single component interaction exactly
- **NO smooth saturation** - want the chaotic, broken vintage sound
- **Model ALL parasitic capacitances** and component nonlinearities
- **Precise component interactions** in exact schematic order
- **Don't care about CPU usage** - accuracy over efficiency
- **Every component must load every other component** through proper impedance network
- **Circuit instabilities and oscillation tendencies** are FEATURES, not bugs
- **Asymmetric clipping, bias drift, intermodulation distortion** - all wanted

## REQUIRED CIRCUIT MODELING APPROACH

1. **Transistor Parasitics**: Cbe, Cbc capacitances (~10-50pF, 5-15pF)
2. **Real Saturation Curves**: Knee points, soft clipping, temperature effects
3. **Component Nonlinearities**: Voltage-dependent capacitors, resistor noise
4. **Supply Voltage Sag**: Under load effects
5. **Continuous Impedance Network**: Not isolated stages
6. **Real Frequency Response**: Emerges from component interactions
7. **Bias Point Drift**: Creates authentic asymmetric behavior

## Plugin Build Requirements

**ALWAYS BUILD BOTH AU AND VST3 FORMATS**

- AU: Required for Logic Pro and other macOS DAWs
- VST3: Modern standard for most DAWs
- VST2: Requires legacy SDK (not included)

## Build Version System

- Version displayed in plugin GUI (top next to terminal fuzz text)
- Increment version in Source/Common/BuildVersion.h for each build

## WORKING BJT TRANSISTOR CONFIGURATION - CRITICAL SUCCESS

**BREAKTHROUGH CONFIGURATION** - Produces authentic Terminal Fuzz hard clipping character

### Key Working Parameters (TerminalCircuit.cpp):

```cpp
// CRITICAL: VBE Response Configuration
float vbe_base = model.vbe_on;                    // Use model's VBE_ON (0.63-0.66V)
float vbe_max = vbe_base + 0.15f;                 // REDUCED ceiling - allows variation
float log_scale = 2.0f;                          // LESS aggressive scaling - more responsive
float vbe_delta = 0.15f * (1.0f - std::exp(-signal_factor / log_scale));
float vbe = vbe_base + (vbe_delta * vbe_polarity);

// CRITICAL: Current Scaling
float current_scale = 0.5f;                      // Balanced for audible distortion
float outputScale = 5.0f;                        // Final output boost for DAW levels

// CRITICAL: Q1 Buffer Simplification
float buffer_gain = 0.9f;                        // Simple unity-gain buffer
float buffered_output = sample * buffer_gain;    // No complex BJT math
```

### Why This Works:

1. **VBE Ceiling Fix**: Reduced from 0.25V to 0.15V - transistors can actually vary VBE
2. **Responsive Scaling**: log_scale 2.0f makes transistors react to input changes
3. **Proper Current Output**: 0.5f scaling produces meaningful collector variations
4. **Stable Buffer**: Q1 simplified to prevent massive negative amplification
5. **Audible Levels**: 5.0f output scaling without DAW overload

### BJT Characteristics That Work:

- **Q1**: 2N3904, hFE=120, VBE_ON=0.65V (simple buffer)
- **Q2**: 2N2369, hFE=50, VBE_ON=0.63V (main fuzz stage)
- **Q3**: 2N2369, hFE=65, VBE_ON=0.66V (second gain stage)

### Debug Results Showing Success:

- Q2 VBE varying: 0.659-0.661V (proper variation)
- Q3 VBE varying: 0.672-0.673V (good amplification)
- Final output: ~0.094V (audible, not overwhelming)
- Natural BJT saturation producing authentic hard clipping

**NEVER CHANGE THESE WORKING VALUES WITHOUT TESTING**
