/**
 * @file eq_module.cpp
 * @brief Audio equalizer and filtering module
 */

#include <iostream>
#include <cstdio>
#include <cmath>
#include "audio_utils.h"
#include "common_args.h"
#include "processor.h"

/**
 * @brief Command line arguments for EQ module
 */
struct EqArgs : BaseArgs {
    float highPassFilterFreq = 0.0f; ///< High-pass filter frequency in Hz
    float lowPassFilterFreq = 0.0f;  ///< Low-pass filter frequency in Hz

    /**
     * @brief Parse command line arguments
     * @param argc Argument count
     * @param argv Argument values
     * @return true if parsing succeeded, false otherwise
     */
    bool parseArgs(int argc, char** argv) {
        // Parse all arguments in one pass
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
            } else if (arg == "--hpf-freq" && i + 1 < argc) {
                highPassFilterFreq = std::stof(argv[++i]);
            } else if (arg == "--lpf-freq" && i + 1 < argc) {
                lowPassFilterFreq = std::stof(argv[++i]);
            }
        }
        
        return validateCommonArgs();
    }

    /**
     * @brief Print help information
     */
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

/**
 * @brief Second-order Butterworth filter implementation
 * 
 * Implements a biquad direct form II structure with proper coefficient normalization.
 * Supports both high-pass and low-pass filtering with Butterworth response (Q = 1/√2).
 */
class ButterworthFilter {
public:
    /**
     * @brief Default constructor
     */
    ButterworthFilter() : x1(0), x2(0), y1(0), y2(0) {}
    
    /**
     * @brief Configure as high-pass filter
     * @param freq Cutoff frequency in Hz
     * @param sampleRate Sample rate in Hz
     */
    void setHighPass(float freq, float sampleRate) {
        // frequency warping
        // Converts our cutoff frequency to normalized digital frequency (0 to π).
        float w = 2.0f * M_PI * freq / sampleRate;
    
        // bilinear transform coefficients
        // The α parameter controls the filter's Q factor. For Butterworth, Q = 1/√2 ≈ 0.707.
        float cosw = cosf(w);
        float sinw = sinf(w);
        float alpha = sinw / M_SQRT2;
        
        // calculate filter coefficients
        float b0 = (1 + cosw) / 2;
        float b1 = -(1 + cosw);
        float b2 = (1 + cosw) / 2;
        float a0 = 1 + alpha;
        float a1 = -2 * cosw;
        float a2 = 1 - alpha;
        
        this->b0 = b0 / a0;
        this->b1 = b1 / a0;
        this->b2 = b2 / a0;
        this->a1 = a1 / a0;
        this->a2 = a2 / a0;
    }
    
    /**
     * @brief Configure as low-pass filter
     * @param freq Cutoff frequency in Hz
     * @param sampleRate Sample rate in Hz
     */
    void setLowPass(float freq, float sampleRate) {
        float w = 2.0f * M_PI * freq / sampleRate;
        float cosw = cosf(w);
        float sinw = sinf(w);
        float alpha = sinw / M_SQRT2;
        
        float b0 = (1 - cosw) / 2;
        float b1 = 1 - cosw;
        float b2 = (1 - cosw) / 2;
        float a0 = 1 + alpha;
        float a1 = -2 * cosw;
        float a2 = 1 - alpha;
        
        this->b0 = b0 / a0;
        this->b1 = b1 / a0;
        this->b2 = b2 / a0;
        this->a1 = a1 / a0;
        this->a2 = a2 / a0;
    }
    
    /**
     * @brief Process a single sample
     * @param input Input sample
     * @return Filtered output sample
     */
    float process(float input) {
        float output = b0 * input + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        x2 = x1; x1 = input;
        y2 = y1; y1 = output;
        return output;
    }
    
private:
    float b0, b1, b2, a1, a2; ///< Filter coefficients
    float x1, x2, y1, y2;     ///< Filter state variables
};

/**
 * @brief Audio equalizer processor with high-pass and low-pass filters
 */
class Eq : public Processor {
public:
    /**
     * @brief Constructor
     * @param highPassFilterFreq High-pass filter frequency in Hz (0 = disabled)
     * @param lowPassFilterFreq Low-pass filter frequency in Hz (0 = disabled)
     * @param inputGain Input gain multiplier
     * @param outputGain Output gain multiplier
     * @param mix Wet/dry mix ratio (0.0 = dry, 1.0 = wet)
     * @param sampleRate Sample rate in Hz
     */
    Eq(
        float highPassFilterFreq = 0.0f,
        float lowPassFilterFreq = 0.0f,
        float inputGain = 1.0f,
        float outputGain = 1.0f,
        float mix = 1.0f,
        float sampleRate = 44100.0f
    )
    : Processor(inputGain,outputGain, mix), hpfFreq(highPassFilterFreq), lpfFreq(lowPassFilterFreq), _sampleRate(sampleRate)
    {
        if (hpfFreq > 0) {
            hpFilter.setHighPass(hpfFreq, _sampleRate);
        }
        if (lpfFreq > 0) {
            lpFilter.setLowPass(lpfFreq, _sampleRate);
        }
    }

    /**
     * @brief Process audio samples through the EQ filters
     * @param samples Audio samples to process (modified in-place)
     */
    void processSamples(std::vector<float>& samples) override {
        for (float& sample : samples) {
            if (hpfFreq > 0) {
                sample = hpFilter.process(sample);
            }
            if (lpfFreq > 0) {
                sample = lpFilter.process(sample);
            }
        }
    }
    
private:
    float hpfFreq, lpfFreq, _sampleRate; ///< Filter frequencies and sample rate
    ButterworthFilter hpFilter, lpFilter; ///< High-pass and low-pass filter instances
};

/**
 * @brief Main entry point for EQ module
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit code (0 = success, 1 = error)
 */
int eq_main(int argc, char** argv) {

    // parse args and set parameters
    EqArgs args;
    if (!args.parseArgs(argc, argv)) {
        return 1;
    }

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
        args.mix,
        static_cast<float>(audioData.sampleRate)
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

    return 0;
}