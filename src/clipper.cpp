#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h" // downloaded this

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>

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

// clipper wrapper
float clipperWrapper(float x, float inputGain, float outputGain, float (*clipFunc)(float))
{
    float input = x * inputGain;
    float clipped = clipFunc(input);
    float output = clipped / outputGain;
    return output;
}

// === Convert 16-bit PCM to float (-1.0 to 1.0) ===
float pcm16ToFloat(int16_t sample) {
    return static_cast<float>(sample) / 32768.0f;
}

// === Convert float (-1.0 to 1.0) to 16-bit PCM ===
int16_t floatToPcm16(float sample) {
    sample = std::clamp(sample, -1.0f, 1.0f);
    return static_cast<int16_t>(sample * 32767.0f);
}

std::vector<int16_t> read16BitWav(drwav wav, const char* filename)
{
    // Open and initialize the input WAV file
    if (!drwav_init_file(&wav, filename, nullptr)) {
        std::cerr << "Failed to open input WAV file.\n";
        return {};
    }

    // Read frames into a vector
    std::vector<int16_t> pcmSamples(wav.totalPCMFrameCount * wav.channels);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pcmSamples.data());
    return pcmSamples;
}

drwav_uint64 write16BitWav(const char* filename, const std::vector<int16_t>& pcmSamples, drwav wav)
{
    // Prepare output WAV format (32-bit float)
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = wav.channels;
    format.sampleRate = wav.sampleRate;
    format.bitsPerSample = 16;

    // Open output WAV file for writing
    drwav outWav;
    if (!drwav_init_file_write(&outWav, filename, &format, nullptr)) {
        std::cerr << "Failed to open output WAV file.\n";
        drwav_uninit(&wav);
        return 0;
    }

    // Get framesRead from input wav
    drwav_uint64 framesRead = pcmSamples.size() / wav.channels;

    // Write frames to output wav
    drwav_uint64 framesWritten = drwav_write_pcm_frames(&outWav, framesRead, pcmSamples.data());
    if (framesWritten != framesRead) {
        std::cerr << "Warning: Not all frames were written.\n";
        return 0;
    }

    // Cleanup
    drwav_uninit(&wav);
    drwav_uninit(&outWav);

    return framesWritten;
}

void writeToCSV(const std::string& filename,
                const std::vector<float>& original,
                const std::vector<float>& clipped)
{
    if (original.size() != clipped.size()) {
        std::cerr << "Error: Vectors must be the same size.\n";
        return;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filename << "\n";
        return;
    }

    file << std::fixed << std::setprecision(6);
    file << "Original,Clipped\n";

    size_t max = original.size();
    for (size_t i = 0; i < max; ++i) {
        file << original[i] << "," << clipped[i] << "\n";
    }

    file.close();
    std::cout << "Num Samples written: " << max << "\n";
    std::cout << "CSV written to: " << filename << "\n";
}

int main(int argc, char** argv)
{
    // DEFAULT ARGUMENTS
    const char* inputWavFileName = "wav/NTM_Underoath_Kick1_44khz_16bit.wav";
    std::string outputWavFileName = "wav/NTM_Underoath_Kick1_44khz_16bit_clip_.wav";
    std::string outputCSVFileName = "csv/NTM_Underoath_Kick1_44khz_16bit_";
    std::string clipperType = "hard";
    float inputGainDB = 0.0f;
    float outputGainDB = 0.0f;

    // SET ARGUMENTS
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if ((arg == "-ig" || arg == "--input-gain") && i + 1 < argc) {
            inputGainDB = std::stof(argv[++i]);
        }
        if ((arg == "-og" || arg == "--output-gain") && i + 1 < argc) {
            outputGainDB = std::stof(argv[++i]);
        }
        if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            inputWavFileName = argv[++i];
        }
        if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            outputWavFileName = argv[++i];
        }
        if ((arg == "-t" || arg == "--type") && i + 1 < argc) {
            clipperType = argv[++i];
        }
    }

    float inputGainLinear = std::pow(10.0f, inputGainDB / 20.0f);
    float outputGainLinear = std::pow(10.0f, outputGainDB / 20.0f);
    outputCSVFileName += \
        clipperType + "_" + \
        std::to_string(inputGainDB) + "_" + \
        std::to_string(outputGainDB) + ".csv";
    outputWavFileName += \
        clipperType + "_" + \
        std::to_string(inputGainDB) + "_" + \
        std::to_string(outputGainDB) + ".wav";
    
    float (*clipFunc)(float);
    if (clipperType == "hard")
        clipFunc = hardClip;
    else if (clipperType == "tanh")
        clipFunc = tanhClip;
    else if (clipperType == "atan")
        clipFunc = atanClip;
    else if (clipperType == "cubic")
        clipFunc = cubicClip;
    else if (clipperType == "smooth")
        clipFunc = smoothClip;
    else {
        // invalid clipper type, raise a value error
        std::cerr << "Invalid clipper type: " << clipperType << "\n";
        return 0;
    }
    
    std::cout << "Input DB Gain: " << inputGainDB << " dB\n";
    std::cout << "Input Linear Gain: " << inputGainLinear << "\n";
    std::cout << "Output DB Gain: " << outputGainDB << " dB\n";
    std::cout << "Output Linear Gain: " << outputGainLinear << "\n";
    std::cout << "Input WAV: " << inputWavFileName << "\n";
    std::cout << "Output WAV: " << outputWavFileName << "\n";
    std::cout << "Output WAV cstr: " << outputWavFileName.c_str() << "\n";
    std::cout << "Clipper Type: " << clipperType << "\n";

    // Open and initialize the input WAV file
    drwav wavIn;
    if (!drwav_init_file(&wavIn, inputWavFileName, nullptr)) {
        std::cerr << "Failed to open input WAV file.\n";
        return 1;
    }

    // Read frames into a vector
    std::vector<int16_t> pcmSamples(wavIn.totalPCMFrameCount * wavIn.channels);
    drwav_uint64 framesRead = drwav_read_pcm_frames_s16(&wavIn, wavIn.totalPCMFrameCount, pcmSamples.data());
    std::cout << "Frames Read: " << framesRead << "\n";

    // Vectors for sample processing
    std::vector<float> floatSamples(pcmSamples.size());
    std::vector<float> clippedSamples(pcmSamples.size());
    std::vector<int16_t> outputBuffer(pcmSamples.size());

    // Convert to float, Clip, and convert back to pcm16
    for (size_t i = 0; i < pcmSamples.size(); ++i)
    {
        floatSamples[i] = pcm16ToFloat(pcmSamples[i]);
        clippedSamples[i] = clipperWrapper(floatSamples[i], inputGainLinear, outputGainLinear, clipFunc);
        outputBuffer[i] = floatToPcm16(clippedSamples[i]);
    }

    // export to csv
    writeToCSV(outputCSVFileName, floatSamples, clippedSamples);

    // Prepare output WAV format (32-bit float)
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = wavIn.channels;
    format.sampleRate = wavIn.sampleRate;
    format.bitsPerSample = wavIn.bitsPerSample;

    // // Open output WAV file for writing
    drwav outWav;
    std::cout << "Writing to: " << outputWavFileName << "\n";
    if (!drwav_init_file_write(&outWav, outputWavFileName.c_str(), &format, nullptr)) {
        std::cerr << "Failed to open output WAV file.\n";
        return 1;
    }
    std::cout << "Successfully opened output WAV file.\n";

    // Write frames to output wav
    drwav_uint64 framesWritten = drwav_write_pcm_frames(&outWav, framesRead, outputBuffer.data());
    std::cout << "Frames Written: " << framesWritten << "\n";

    // // Cleanup
    drwav_uninit(&wavIn);
    drwav_uninit(&outWav);

    return 0;
}
