#include <iostream>
#include <cstdio>
#include "audio_utils.h"
#include "common_args.h"
#include "processor.h"

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

class Eq : public Processor {
public:
    Eq(
        float highPassFilterFreq = 0.0f,
        float lowPassFilterFreq = 0.0f,
        float inputGain = 1.0f,
        float outputGain = 1.0f,
        float mix = 1.0f
    )
    : Processor(inputGain, outputGain, mix)
    {

    }

    void processSamples(std::vector<float>& samples) override {
        // EQ processing - currently pass-through
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

    // initialize variables for processing
    Eq eq(
        args.highPassFilterFreq,
        args.lowPassFilterFreq,
        args.getInputGainLinear(),
        args.getOutputGainLinear(),
        args.mix
    );

    // process samples
    std::vector<float> originalSamples = audioData.fromPcm16ToFloat();
    std::vector<float> processedSamples = originalSamples;
    eq.processSamples(processedSamples);
    audioData.fromFloatToPcm16(processedSamples);

    //  Write audio to stdout or Export to CSV if requested
    if (args.shouldExportCsv()) {
        if (!exportToCsv(args.outputCsv, originalSamples, processedSamples)) {
            return 1;
        }
    } else {
        if (!writeWavToStdout(audioData)) {
            return 1;
        }
    }

    return 1;
}