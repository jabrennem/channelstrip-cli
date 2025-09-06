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

    float processSample(float x) override {
        return x;
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
    std::vector<float> floatSamples(audioData.samples.size());
    std::vector<float> processedSamples(audioData.samples.size());
    Eq eq(
        args.highPassFilterFreq,
        args.lowPassFilterFreq,
        args.getInputGainLinear(),
        args.getOutputGainLinear(),
        args.mix
    );

    // process samples
    for (size_t i = 0; i < audioData.samples.size(); ++i) {
        floatSamples[i] = pcm16ToFloat(audioData.samples[i]);
        processedSamples[i] = eq.processSample(floatSamples[i]);
        audioData.samples[i] = floatToPcm16(processedSamples[i]);
    }

    //  Write audio to stdout or Export to CSV if requested
    if (args.shouldExportCsv()) {
        if (!exportToCsv(args.outputCsv, floatSamples, processedSamples)) {
            return 1;
        }
    } else {
        if (!writeWavToStdout(audioData)) {
            return 1;
        }
    }

    return 1;
}