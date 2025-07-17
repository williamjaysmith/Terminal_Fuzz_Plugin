#include "Source/DSP/TerminalCircuitTests.cpp"

int main() {
    std::cout << "Terminal Fuzz Plugin - DSP Diagnostic Tests\n";
    std::cout << "==========================================\n";
    
    try {
        TerminalFuzz::DSP::Tests::runTerminalCircuitTests();
        std::cout << "\nAll tests completed successfully!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception\n";
        return 1;
    }
}