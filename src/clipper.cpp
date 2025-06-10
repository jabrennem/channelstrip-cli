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
#include <chrono>
#include <ctime>

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

void writeToCSV(const std::string& filename, const std::vector<float>& original, const std::vector<float>& clipped)
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
    
    // Create a static log file for the writeToCSV function
    static std::ofstream csvLogFile("logs/clipper.log", std::ios::app);
    if (csvLogFile.is_open()) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time);
        char timestamp[25];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", now_tm);
        
        csvLogFile << "[" << timestamp << "] Num Samples written: " << max << std::endl;
        csvLogFile << "[" << timestamp << "] CSV written to: " << filename << std::endl;
    }
}

// Logger function to write to a log file instead of cout
void log(const std::string& message, std::ofstream& logFile) {
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    
    // Format timestamp
    std::tm* now_tm = std::localtime(&now_time);
    char timestamp[25];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", now_tm);
    
    // Write to log file with timestamp
    logFile << "[" << timestamp << "] " << message << std::endl;
}

std::string getFilenameWithoutExtension(const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    size_t lastDot = path.find_last_of(".");
    
    size_t startPos = (lastSlash != std::string::npos) ? lastSlash + 1 : 0;
    size_t length = (lastDot != std::string::npos && lastDot > startPos) ? lastDot - startPos : std::string::npos;
    
    return path.substr(startPos, length);
}

int main(int argc, char** argv)
{
    // Open log file
    std::ofstream logFile("logs/clipper.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file. Continuing without logging." << std::endl;
    }
    
    // DEFAULT ARGUMENTS
    std::string inputWavFileName;
    std::string outputWavFileName;
    std::string clipperType = "hard";
    float inputGainDB = 0.0f;
    float outputGainDB = 0.0f;
    bool csv = false;

    // SET ARGUMENTS
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--input-file" && i + 1 < argc) inputWavFileName = argv[++i];
        if (arg == "--output-file" && i + 1 < argc) outputWavFileName = argv[++i];
        if (arg == "--input-gain" && i + 1 < argc) inputGainDB = std::stof(argv[++i]);
        if (arg == "--output-gain" && i + 1 < argc) outputGainDB = std::stof(argv[++i]);
        if (arg == "--type" && i + 1 < argc) clipperType = argv[++i];
        if (arg == "--csv") csv = true;
    }

    // SET ARGUMENTS ACCORDING TO COMMAND LINE ARGUMENTS
    float inputGainLinear = std::pow(10.0f, inputGainDB / 20.0f);
    float outputGainLinear = std::pow(10.0f, outputGainDB / 20.0f);
    std::string outputCSVFileName = "csv/" + getFilenameWithoutExtension(outputWavFileName) + ".csv";
    
    // Set Clipper Function
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

    log("Input DB Gain: " + std::to_string(inputGainDB) + " dB", logFile);
    log("Input Linear Gain: " + std::to_string(inputGainLinear), logFile);
    log("Output DB Gain: " + std::to_string(outputGainDB) + " dB", logFile);
    log("Output Linear Gain: " + std::to_string(outputGainLinear), logFile);
    log("Input WAV: " + inputWavFileName, logFile);
    log("Input WAV cstr: " + std::string(inputWavFileName.c_str()), logFile);
    log("Output WAV: " + outputWavFileName, logFile);
    log("Output WAV cstr: " + std::string(outputWavFileName.c_str()), logFile);
    log("Clipper Type: " + clipperType, logFile);

    // // Simulate a simple waveform with values from -2 to 2
    // for (float x = -2.0f; x <= 2.0f; x += 0.1f)
    //     inputBuffer.push_back(x);

    // Open and initialize the input WAV file
    drwav wavIn;
    if (!drwav_init_file(&wavIn, inputWavFileName.c_str(), nullptr)) {
        log("Failed to open input WAV file.", logFile);
        std::cerr << "Failed to open input WAV file.\n";
        return 1;
    }

    // Read frames into a vector
    std::vector<int16_t> pcmSamples(wavIn.totalPCMFrameCount * wavIn.channels);
    drwav_uint64 framesRead = drwav_read_pcm_frames_s16(&wavIn, wavIn.totalPCMFrameCount, pcmSamples.data());
    log("Frames Read: " + std::to_string(framesRead), logFile);

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

    // Prepare output WAV format (32-bit float)
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = wavIn.channels;
    format.sampleRate = wavIn.sampleRate;
    format.bitsPerSample = wavIn.bitsPerSample;

    // Open output WAV file for writing
    drwav outWav;
    log("Writing to: " + outputWavFileName, logFile);
    if (!drwav_init_file_write(&outWav, outputWavFileName.c_str(), &format, nullptr)) {
        log("Failed to open output WAV file.", logFile);
        std::cerr << "Failed to open output WAV file.\n";
        return 1;
    }
    log("Successfully opened output WAV file.", logFile);

    // Write frames to output wav
    drwav_uint64 framesWritten = drwav_write_pcm_frames(&outWav, framesRead, outputBuffer.data());
    log("Frames Written: " + std::to_string(framesWritten), logFile);

    // Cleanup
    drwav_uninit(&wavIn);
    drwav_uninit(&outWav);

    // export to csv
    if (csv) writeToCSV(outputCSVFileName, floatSamples, clippedSamples);
    
    // Close log file
    if (logFile.is_open()) {
        log("Processing completed successfully.", logFile);
        logFile.close();
    }

    return 0;
}
