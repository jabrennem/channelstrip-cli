/**
 * @file processor.h
 * @brief Base processor class for all DSP modules
 */

#pragma once

#include <algorithm>
#include <vector>

/**
 * @brief Base class for all audio processors
 * 
 * Provides common DSP parameters (input/output gain, mix) and 
 * common processing utilities for derived classes.
 */
class Processor {
public:
    /**
     * @brief Construct a new Processor object
     * @param inputGain Input gain multiplier
     * @param outputGain Output gain multiplier  
     * @param mix Wet/dry mix ratio (0.0-1.0)
     */
    Processor(float inputGain = 1.0f, float outputGain = 1.0f, float mix = 1.0f)
        : inputGain(inputGain), 
          outputGain(outputGain),
          wetDryMix(std::clamp(mix, 0.0f, 1.0f)) {}

    /**
     * @brief Process samples method to be implemented by derived classes
     * @param samples Input/output samples vector
     */
    virtual void processSamples(std::vector<float>& samples) = 0;

    virtual ~Processor() = default;

protected:
    /**
     * @brief Common processing flow: input gain, processing, output gain, mix
     * @param samples Input/output samples vector
     * @param processFunc Function to apply the specific DSP algorithm
     */
    void processInternal(std::vector<float>& samples, void (*processFunc)(std::vector<float>&)) {
        std::vector<float> original = samples;
        
        // Apply input gain
        for (float& sample : samples) {
            sample *= inputGain;
        }
        
        // Apply processing
        processFunc(samples);
        
        // Apply output gain and mix
        for (size_t i = 0; i < samples.size(); ++i) {
            samples[i] = wetDryMix * (samples[i] * outputGain) + (1.0f - wetDryMix) * original[i];
        }
    }

    float inputGain;
    float outputGain;
    float wetDryMix;
};