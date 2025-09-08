/**
 * @file common_args.h
 * @brief Shared command line argument structures for all modules
 */

#pragma once

#include <string>
#include <iostream>
#include "audio_utils.h"

/**
 * @brief Base arguments structure shared across all audio processing modules
 */
struct BaseArgs {
    bool streamMode = false;      ///< Stream processing mode flag (-)
    float inputGainDB = 0.0f;     ///< Input gain in decibels
    float outputGainDB = 0.0f;    ///< Output gain in decibels
    float mix = 1.0f;             ///< Wet/dry mix (1.0 = fully wet)
    std::string outputCsv = "";   ///< CSV output file path
    bool showHelp = false;        ///< Show help flag
    
    // Helper methods
    float getInputGainLinear() const { return dbToGain(inputGainDB); }
    float getOutputGainLinear() const { return dbToGain(outputGainDB); }
    bool hasStreamMode() const { return streamMode; }
    bool shouldExportCsv() const { return !outputCsv.empty(); }
    
    // Parse common arguments, returns the index where parsing stopped
    int parseCommonArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-" || arg == "--stream") {
                streamMode = true;
            } else if (arg == "--help" || arg == "-h") {
                showHelp = true;
            } else if ((arg == "--input-gain" || arg == "-i") && i + 1 < argc) {
                inputGainDB = std::stof(argv[++i]);
            } else if ((arg == "--output-gain" || arg == "-o") && i + 1 < argc) {
                outputGainDB = std::stof(argv[++i]);
            } else if ((arg == "--mix" || arg == "-m") && i + 1 < argc) {
                mix = std::stof(argv[++i]);
            } else if ((arg == "--output-csv" || arg == "-c") && i + 1 < argc) {
                outputCsv = argv[++i];
            } else {
                // Return index of first unrecognized argument for module-specific parsing
                return i;
            }
        }
        
        return argc; // All arguments were parsed
    }
    
    // Validate common arguments
    bool validateCommonArgs() const {
        if (!showHelp && !streamMode && outputCsv.empty()) {
            std::cerr << "Error: Either stream mode (-) or CSV output (--output-csv) must be specified\n";
            return false;
        }
        return true;
    }
    
    virtual void printCommonHelp() const {
        std::cout << "Common options:\n";
        std::cout << "  -                  Stream mode (required if no --output-csv)\n";
        std::cout << "  --input-gain DB    Input gain in dB [default: 0.0]\n";
        std::cout << "  --output-gain DB   Output gain in dB [default: 0.0]\n";
        std::cout << "  --mix VALUE        Wet/dry mix (0.0-1.0) [default: 1.0]\n";
        std::cout << "  --output-csv FILE  Export to CSV file (required if no -)\n";
        std::cout << "  --help             Show this help message\n";
    }
    
    virtual ~BaseArgs() = default;
};