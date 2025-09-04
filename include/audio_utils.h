/**
 * @file audio_utils.h
 * @brief Shared audio processing utilities for WAV I/O and CSV export
 */

#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "dr_wav.h"

/**
 * @brief Audio data structure containing samples and metadata
 */
struct AudioData {
    std::vector<int16_t> samples;
    uint32_t channels;
    uint32_t sampleRate;
    uint64_t totalFrames;
};

/**
 * @brief Read WAV audio data from stdin
 * @return AudioData structure containing samples and metadata
 */
AudioData readWavFromStdin();

/**
 * @brief Write WAV audio data to stdout
 * @param audioData Audio data to write
 * @return true on success, false on failure
 */
bool writeWavToStdout(const AudioData& audioData);

/**
 * @brief Export audio samples to CSV file
 * @param filename Output CSV file path
 * @param inputSamples Original input samples (normalized float)
 * @param outputSamples Processed output samples (normalized float)
 * @return true on success, false on failure
 */
bool exportToCsv(const std::string& filename, 
                 const std::vector<float>& inputSamples,
                 const std::vector<float>& outputSamples);

/**
 * @brief Convert 16-bit PCM sample to normalized float
 * @param sample 16-bit PCM sample
 * @return Normalized float sample (-1.0 to 1.0)
 */
float pcm16ToFloat(int16_t sample);

/**
 * @brief Convert normalized float sample to 16-bit PCM
 * @param sample Normalized float sample (-1.0 to 1.0)
 * @return 16-bit PCM sample
 */
int16_t floatToPcm16(float sample);

/**
 * @brief Convert decibel value to linear gain
 * @param db Gain in decibels
 * @return Linear gain multiplier
 */
float dbToGain(float db);