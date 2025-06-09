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

// === Convert 16-bit PCM to float (-1.0 to 1.0) ===
float pcm16ToFloat(int16_t sample) {
    return static_cast<float>(sample) / 32768.0f;
}

// === Convert float (-1.0 to 1.0) to 16-bit PCM ===
int16_t floatToPcm16(float sample) {
    sample = std::clamp(sample, -1.0f, 1.0f);
    return static_cast<int16_t>(sample * 32767.0f);
}

// process an entire buffer
void processBuffer(std::vector<float>& buffer, float (*clipFunc)(float))
{
    for (float& sample : buffer)
        sample = clipFunc(sample);
}

// split a string into a vector
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        // Optionally trim whitespace here
        tokens.push_back(item);
    }
    return tokens;
}

// export input and all results to a csv
void exportToCSV(const std::string& filename,
                 const std::vector<float>& input,
                 const std::map<std::string, std::vector<float>>& outputs)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filename << "\n";
        return;
    }

    file << std::fixed << std::setprecision(6);

    // Write header
    file << "Input";
    for (const auto& [name, _] : outputs)
        file << "," << name;
    file << "\n";

    // Write rows
    size_t N = input.size();
    for (size_t i = 0; i < N; ++i) {
        file << input[i];
        for (const auto& [_, vec] : outputs)
            file << "," << vec[i];
        file << "\n";
    }

    file.close();
    std::cout << "Exported all clipper outputs to " << filename << "\n";
}


int main(int argc, char** argv)
{
    std::string clipperTypesStr = "Hard,Tanh,Atan,Cubic,Smooth";
    std::string csvFilename = "output.csv";
    std::string useSampleDataStr = "false";
    std::string inputWavFilename = "input.wav";

    // Parse argv
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if ((arg == "-c" || arg == "--csv") && i + 1 < argc) {
            csvFilename = argv[++i];
        }
        if ((arg == "-t" || arg == "--type") && i + 1 < argc) {
            clipperTypesStr = argv[++i];
        }
        if ((arg == "-s" || arg == "--sample") && i + 1 < argc) {
            useSampleDataStr = "true";
        }
        if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            inputWavFilename = argv[++i];
        }
    }

    std::vector<float> inputBuffer;

    // Set inputBuffer
    if (useSampleDataStr == "true")
    {
        // Simulate a simple waveform with values from -2 to 2
        for (float x = -2.0f; x <= 2.0f; x += 0.1f)
            inputBuffer.push_back(x);
    } else {
        // Load WAV File

        drwav wav;
        const char* inputWavFileNameCStr = inputWavFilename.c_str();

        if (!drwav_init_file(&wav, inputWavFileNameCStr, nullptr)) {
            std::cerr << "Failed to open WAV file.\n";
            return 1;
        }

        if (wav.bitsPerSample != 16) {
            std::cerr << "Only 16-bit WAV files are supported in this example.\n";
            drwav_uninit(&wav);
            return 1;
        }

        std::vector<int16_t> inputSamples(wav.totalPCMFrameCount * wav.channels);
        drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, inputSamples.data());
        std::vector<int16_t> outputSamples(inputSamples.size());
    }







    
    // Define clippers 
    std::map<std::string, float(*)(float)> clippers = {
        {"Hard", hardClip},
        {"Tanh", tanhClip},
        {"Atan", atanClip},
        {"Cubic", cubicClip},
        {"Smooth", smoothClip}
    };

    std::vector<std::string> clipperTypes = split(clipperTypesStr, ',');

    // Process only selected clippers
    std::map<std::string, std::vector<float>> results;
    for (const auto& name : clipperTypes) {
        std::cout << "Processing clipper type '" << name << "'\n";
        auto it = clippers.find(name);
        if (it != clippers.end()) {
            std::vector<float> processed = inputBuffer;  // Copy
            processBuffer(processed, it->second);        // Apply clipper
            results[name] = std::move(processed);        // Store result
        } else {
            std::cerr << "Warning: Unknown clipper type '" << name << "'\n";
        }
    }

    // Export to CSV
    if (useSampleDataStr == "true")
        exportToCSV(csvFilename, inputBuffer, results);
    else {
        
    }

    return 0;
}