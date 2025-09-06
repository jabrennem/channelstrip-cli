/**
 * @file clipper_module.cpp
 * @brief Audio clipping/saturation module with various algorithms
 * @author ChannelStrip CLI
 */

#include "audio_utils.h"
#include "common_args.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

/**
 * @brief Hard clipping function that limits signal to [-1.0, 1.0]
 * @param x Input sample
 * @return Clipped sample
 */
float hardClip(float x)
{
    return std::clamp(x, -1.0f, 1.0f);
}

/**
 * @brief Soft clipping using hyperbolic tangent function
 * @param x Input sample
 * @return Clipped sample using tanh curve
 */
float tanhClip(float x)
{
    return std::tanh(x);
}

/**
 * @brief Soft clipping using arctangent function
 * @param x Input sample
 * @return Clipped sample using atan curve
 */
float atanClip(float x)
{
    return std::atan(x);
}

/**
 * @brief Cubic soft clipping function
 * @param x Input sample
 * @return Clipped sample using cubic polynomial
 */
float cubicClip(float x)
{
    if (x < -1.0f) return -2.0f / 3.0f;
    if (x > 1.0f) return 2.0f / 3.0f;
    return x - (1.0f / 3.0f) * x * x * x;
}

/**
 * @brief Smooth clipping using x / (1 + |x|) formula
 * @param x Input sample
 * @return Smoothly clipped sample
 */
float smoothClip(float x)
{
    return x / (1.0f + std::abs(x));
}

/**
 * @brief Tape-style audio clipper with memory and various clipping algorithms
 * 
 * Provides audio saturation/clipping with configurable algorithms, input/output gain,
 * smoothing (memory effect), and wet/dry mixing.
 */
class TapeClipper {
    public:
        /**
         * @brief Construct a new TapeClipper object
         * @param clipType Clipping algorithm ("hard", "tanh", "atan", "cubic", "smooth")
         * @param smoothing Smoothing factor for memory effect (0.0-1.0)
         * @param inputDrive Input gain multiplier
         * @param outputVolume Output gain multiplier
         * @param mix Wet/dry mix ratio (0.0-1.0)
         */
        TapeClipper(
            const std::string& clipType,
            float smoothing = 0.0f,
            float inputDrive = 1.0f,
            float outputVolume = 1.0f,
            float mix = 1.0f
        )
        : inputGain(inputDrive),
            outputGain(outputVolume),
            alpha(smoothing), 
            y_prev(0.0f),
            wetDryMix(std::clamp(mix, 0.0f, 1.0f))
            {
                clipFunc = getClipFunc(clipType);
            }
        
        /**
         * @brief Process a single audio sample through the clipper
         * @param x Input sample
         * @return Processed sample with clipping, smoothing, and mixing applied
         */
        float processSample(float x) 
        {
            // apply input gain
            float driven = x * inputGain;
            // saturate
            float saturated = clipFunc(driven);
            // add memory affect - low pass filtering (simple feedback)
            float y = alpha * y_prev + (1.0 - alpha) * saturated;
            // set memory
            y_prev = y;
            // apply output gain
            float output = y / outputGain;
            // mix wet and dry signals
            float wetDry = wetDryMix * output + (1.0 - wetDryMix) * x;
            return wetDry;
        }

    private:
        float inputGain;
        float outputGain;
        float alpha; // Smoothing factor (0.0 to 1.0) – higher means more memory
        float y_prev; // Previous output (memory state)
        float (*clipFunc)(float); // the saturation type
        float wetDryMix;

        /**
         * @brief Get clipping function pointer based on type string
         * @param clipType String identifier for clipping algorithm
         * @return Function pointer to the appropriate clipping function
         */
        float (*getClipFunc(const std::string& clipType))(float) {
            if (clipType == "hard") return hardClip;
            else if (clipType == "tanh") return tanhClip;
            else if (clipType == "atan") return atanClip;
            else if (clipType == "cubic") return cubicClip;
            else if (clipType == "smooth") return smoothClip;
            else {
                fprintf(stderr, "Invalid clip type: %s\n", clipType.c_str());
                exit(1);
            }
        }

};

/**
 * @brief Command line arguments structure for clipper module
 */
struct ClipperArgs : BaseArgs {
    std::string clipType = "hard"; ///< Clipping algorithm type
    float alpha = 0.0f;           ///< Smoothing factor (0.0 = no memory)
    
    // Clipper-specific methods
    bool isValidClipType() const {
        return clipType == "hard" || clipType == "smooth" || 
               clipType == "tanh" || clipType == "atan" || clipType == "cubic";
    }
    
    void parseArgs(int argc, char** argv) {
        int i = parseCommonArgs(argc, argv);
        
        // Parse clipper-specific arguments
        for (; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--type" && i + 1 < argc) {
                clipType = argv[++i];
            } else if (arg == "--alpha" && i + 1 < argc) {
                alpha = std::stof(argv[++i]);
            }
        }
    }
    
    void printHelp() const {
        std::cout << "Usage: chst clipper [options] -\n";
        std::cout << "\nAudio clipping/saturation with various algorithms\n\n";
        
        std::cout << "Clipper-specific options:\n";
        std::cout << "  --type TYPE        Clipping algorithm (hard, smooth, tanh, atan, cubic) [default: hard]\n";
        std::cout << "  --alpha VALUE      Smoothing factor (0.0-1.0) [default: 0.0]\n\n";
        
        printCommonHelp();
        
        std::cout << "\nExamples:\n";
        std::cout << "  cat input.wav | chst clipper - > output.wav\n";
        std::cout << "  cat input.wav | chst clipper --type smooth --alpha 0.5 - | play -t wav -\n";
    }
};

/**
 * @brief Main function for clipper module
 * 
 * Processes WAV audio from stdin, applies clipping/saturation effects,
 * and outputs processed audio to stdout.
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status code (0 for success, 1 for error)
 */
int clipper_main(int argc, char** argv) {

    // parse args and set parameters
    ClipperArgs args;
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

    // Process samples
    std::vector<float> floatSamples(audioData.samples.size());
    std::vector<float> clippedSamples(audioData.samples.size());
    
    TapeClipper clipper(args.clipType, args.alpha, args.getInputGainLinear(), args.getOutputGainLinear(), args.mix);
    for (size_t i = 0; i < audioData.samples.size(); ++i) {
        floatSamples[i] = pcm16ToFloat(audioData.samples[i]);
        clippedSamples[i] = clipper.processSample(floatSamples[i]);
        audioData.samples[i] = floatToPcm16(clippedSamples[i]);
    }

    // Write audio to stdout if CSV output is not requested
    if (!args.shouldExportCsv()) {
        if (!writeWavToStdout(audioData)) {
            return 1;
        }
    }

    // Export to CSV if requested
    if (args.shouldExportCsv()) {
        if (!exportToCsv(args.outputCsv, floatSamples, clippedSamples)) {
            return 1;
        }
    }

    return 0;
}