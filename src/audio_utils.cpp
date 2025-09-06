/**
 * @file audio_utils.cpp
 * @brief Implementation of shared audio processing utilities
 */

#define DR_WAV_IMPLEMENTATION
#include "audio_utils.h"
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <fstream>

/**
 * @brief Write callback function for stdout output
 */
static size_t write_stdout(void* pUserData, const void* pData, size_t bytesToWrite) {
    return fwrite(pData, 1, bytesToWrite, (FILE*)pUserData);
}

/**
 * @brief Read entire stdin into memory buffer
 */
static std::vector<uint8_t> read_stdin_fully() {
    std::vector<uint8_t> buffer;
    char chunk[4096];
    while (size_t n = fread(chunk, 1, sizeof(chunk), stdin)) {
        buffer.insert(buffer.end(), chunk, chunk + n);
    }
    return buffer;
}

AudioData readWavFromStdin() {
    AudioData audioData;
    
    auto input = read_stdin_fully();
    drwav wav;
    if (!drwav_init_memory(&wav, input.data(), input.size(), nullptr)) {
        fprintf(stderr, "Failed to read WAV from stdin\n");
        return audioData;
    }

    audioData.channels = wav.channels;
    audioData.sampleRate = wav.sampleRate;
    audioData.totalFrames = wav.totalPCMFrameCount;
    
    audioData.samples.resize(wav.totalPCMFrameCount * wav.channels);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, audioData.samples.data());
    drwav_uninit(&wav);
    
    return audioData;
}

bool writeWavToStdout(const AudioData& audioData) {
    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = audioData.channels;
    format.sampleRate = audioData.sampleRate;
    format.bitsPerSample = 16;

    drwav outWav;
    if (!drwav_init_write_sequential(&outWav, &format, audioData.totalFrames * audioData.channels, write_stdout, stdout, nullptr)) {
        fprintf(stderr, "Failed to init WAV writer to stdout\n");
        return false;
    }

    drwav_write_pcm_frames(&outWav, audioData.totalFrames, audioData.samples.data());
    drwav_uninit(&outWav);
    return true;
}

bool exportToCsv(const std::string& filename, 
                 const std::vector<float>& inputSamples,
                 const std::vector<float>& outputSamples) {
    std::ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        fprintf(stderr, "Failed to open CSV file: %s\n", filename.c_str());
        return false;
    }
    
    csvFile << "sample,input,output\n";
    for (size_t i = 0; i < inputSamples.size() && i < outputSamples.size(); ++i) {
        csvFile << i << "," << inputSamples[i] << "," << outputSamples[i] << "\n";
    }
    csvFile.close();
    return true;
}

float pcm16ToFloat(int16_t sample) {
    return static_cast<float>(sample) / 32768.0f;
}

int16_t floatToPcm16(float sample) {
    sample = std::clamp(sample, -1.0f, 1.0f);
    return static_cast<int16_t>(sample * 32767.0f);
}

float dbToGain(float db) {
    return powf(10.0f, db / 20.0f);
}