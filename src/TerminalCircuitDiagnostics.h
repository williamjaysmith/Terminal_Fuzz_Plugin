#pragma once

#include <JuceHeader.h>
#include <fstream>
#include <iomanip>

namespace TerminalFuzz {
namespace DSP {

/**
 * @brief Diagnostic utilities for Terminal Circuit debugging
 * 
 * This class provides runtime diagnostics and logging for the DSP chain
 * to help identify and fix audio processing issues.
 */
class TerminalCircuitDiagnostics {
public:
    static TerminalCircuitDiagnostics& getInstance() {
        static TerminalCircuitDiagnostics instance;
        return instance;
    }
    
    void enableLogging(bool enable) {
        loggingEnabled_ = enable;
        if (enable && !logFile_.is_open()) {
            logFile_.open("/tmp/terminal_fuzz_debug.log");
        }
    }
    
    void logSignalFlow(const std::string& stage, float input, float output, int channel = 0) {
        if (!loggingEnabled_ || !logFile_.is_open()) return;
        
        logFile_ << std::fixed << std::setprecision(6)
                 << "CH" << channel << " " << stage 
                 << ": " << input << " -> " << output << std::endl;
    }
    
    void logParameterState(float fuzz, float voice, float treble, float level) {
        if (!loggingEnabled_ || !logFile_.is_open()) return;
        
        logFile_ << "PARAMS: Fuzz=" << fuzz << " Voice=" << voice 
                 << " Treble=" << treble << " Level=" << level << std::endl;
    }
    
    void checkForAudioIssues(float sample, const std::string& location) {
        if (std::isnan(sample) || std::isinf(sample)) {
            DBG("AUDIO ISSUE: NaN/Inf detected at " << location << ": " << sample);
        }
        if (std::abs(sample) > 10.0f) {
            DBG("AUDIO ISSUE: Excessive level at " << location << ": " << sample);
        }
    }
    
    void flush() {
        if (logFile_.is_open()) {
            logFile_.flush();
        }
    }
    
private:
    TerminalCircuitDiagnostics() = default;
    ~TerminalCircuitDiagnostics() {
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
    
    bool loggingEnabled_ = false;
    std::ofstream logFile_;
};

// Convenience macros for debugging
#define TERMINAL_LOG_SIGNAL(stage, input, output, channel) \
    TerminalCircuitDiagnostics::getInstance().logSignalFlow(stage, input, output, channel)

#define TERMINAL_CHECK_AUDIO(sample, location) \
    TerminalCircuitDiagnostics::getInstance().checkForAudioIssues(sample, location)

#define TERMINAL_LOG_PARAMS(f, v, t, l) \
    TerminalCircuitDiagnostics::getInstance().logParameterState(f, v, t, l)

}  // namespace DSP
}  // namespace TerminalFuzz