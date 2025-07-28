#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <algorithm>

// hard clipper
float hardClip(float x)
{
    return std::clamp(x, -1.0f, 1.0f);
}

// soft clip - tanh clipper
float tanhClip(float x)
{
    return std::tanh(x);
}

// soft clip - atan clipper
float atanClip(float x)
{
    return std::atan(x);
}

// soft clip - cubic clipper
float cubicClip(float x)
{
    if (x < -1.0f) return -2.0f / 3.0f;
    if (x > 1.0f) return 2.0f / 3.0f;
    return x - (1.0f / 3.0f) * x * x * x;
}

// soft clip - x / (1 + |x|) 
float smoothClip(float x)
{
    return x / (1.0f + std::abs(x));
}

class TapeClipper {
    public:
        TapeClipper(
            std::string clipType,
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

        // Create a funciton that returns a function based on a string argument
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

// Convert 16-bit PCM to float (-1.0 to 1.0)
float pcm16ToFloat(int16_t sample) {
    return static_cast<float>(sample) / 32768.0f;
}

// Convert float (-1.0 to 1.0) to 16-bit PCM
int16_t floatToPcm16(float sample) {
    sample = std::clamp(sample, -1.0f, 1.0f);
    return static_cast<int16_t>(sample * 32767.0f);
}

struct Args {
    bool streamMode = false;
    std::string clipType = "hard";
    float inputGainDB = 0.0f;
    float outputGainDB = 0.0f;
    float alpha = 0.0f; // no memory only saturation
    float mix = 1.0f; // wet signal
};

Args parseArgs(int argc, char** argv) {
    Args args;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-") args.streamMode = true;
        if (arg == "--type" && i + 1 < argc) args.clipType = argv[++i];
        if (arg == "--input-gain" && i + 1 < argc) args.inputGainDB = std::stof(argv[++i]);
        if (arg == "--output-gain" && i + 1 < argc) args.outputGainDB = std::stof(argv[++i]);
        if (arg == "--alpha" && i + 1 < argc) args.alpha = std::stof(argv[++i]);
        if (arg == "--mix" && i + 1 < argc) args.mix = std::stof(argv[++i]);
    }
    
    return args;
}

// create function that converts db into linear gain
float dbToGain(float db) {
    return powf(10.0f, db / 20.0f);
}

// Write callback for stdout
size_t write_stdout(void* pUserData, const void* pData, size_t bytesToWrite) {
    return fwrite(pData, 1, bytesToWrite, (FILE*)pUserData);
}

// Optional: read entire stdin
std::vector<uint8_t> read_stdin_fully() {
    std::vector<uint8_t> buffer;
    char chunk[4096];
    while (size_t n = fread(chunk, 1, sizeof(chunk), stdin)) {
        buffer.insert(buffer.end(), chunk, chunk + n);
    }
    return buffer;
}

int clipper_main(int argc, char** argv) {

    // parse args and set parameters
    Args args = parseArgs(argc, argv);
    float inputGainLinear = dbToGain(args.inputGainDB);
    float outputGainLinear = dbToGain(args.outputGainDB);

    // read stdin
    auto input = read_stdin_fully();
    drwav wav;
    if (!drwav_init_memory(&wav, input.data(), input.size(), nullptr)) {
        fprintf(stderr, "Failed to read WAV from stdin\n");
        return 1;
    }

    // read pcm frames
    std::vector<int16_t> pcmData(wav.totalPCMFrameCount * wav.channels);
    size_t framesRead = drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pcmData.data());
    drwav_uninit(&wav);

    // Vectors for sample processing
    std::vector<float> floatSamples(pcmData.size());
    std::vector<float> clippedSamples(pcmData.size());
    std::vector<int16_t> outputBuffer(pcmData.size());

    // Convert to float, Clip, and convert back to pcm16
    TapeClipper clipper(args.clipType, args.alpha, inputGainLinear, outputGainLinear, args.mix);
    for (size_t i = 0; i < pcmData.size(); ++i)
    {
        floatSamples[i] = pcm16ToFloat(pcmData[i]);
        clippedSamples[i] = clipper.processSample(floatSamples[i]);
        outputBuffer[i] = floatToPcm16(clippedSamples[i]);
    }

    // Format settings
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = wav.channels;
    format.sampleRate = wav.sampleRate;
    format.bitsPerSample = 16;

    // write pcm frames to stdout
    drwav outWav;
    if (!drwav_init_write_sequential(&outWav, &format, framesRead * wav.channels, write_stdout, stdout, nullptr)) {
        fprintf(stderr, "Failed to init WAV writer to stdout\n");
        return 1;
    }

    drwav_write_pcm_frames(&outWav, framesRead, outputBuffer.data());
    drwav_uninit(&outWav);

    return 0;
}