#include <iostream>
#include <cstdio>
#include "audio_utils.h"
#include "common_args.h"

struct EqArgs : BaseArgs {
    float highPassFilterFreq = 0.0f;
    float lowPassFilterFreq = 0.0f;

    void parseArgs(int argc, char** argv) {
        int i = parseCommonArgs(argc, argv);

        // Parse eq-specific arguments
        for (; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--hpf-freq" && i + 1 < argc) {
                highPassFilterFreq = std::stof(argv[++i]);
            } else if (arg == "--lpf-freq" && i + 1 < argc) {
                lowPassFilterFreq = std::stof(argv[++i]);
            }
        }
    }

    void printHelp() {
        std::cout << "Usage: chst eq [options] -\n";
        std::cout << "\nAudio eq and filtering with various algorithms\n\n";

        std::cout << "EQ-specific options:\n";
        std::cout << "  --hpf-freq FREQ   High-pass filter frequency\n";
        std::cout << "  --lpf-freq FREQ   Low-pass filter frequency\n";

        std::cout << "\n";

        printCommonHelp();
    }
};


int eq_main(int argc, char** argv) {

    // parse args and set parameters
    EqArgs args;
    args.parseArgs(argc, argv);

    if (args.showHelp) {
        args.printHelp();
        return 0;
    }

    // read audio from stdin
    AudioData audioData = readWavFromStdin();
    if (audioData.samples.empty()) {
        return 1;
    }

    return 1;
}