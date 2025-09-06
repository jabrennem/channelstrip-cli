/**
 * @file processor.h
 * @brief Base processor class for all DSP modules
 */

#pragma once

#include <algorithm>

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
     * @brief Process sample method to be implemented by derived classes
     * @param x Input sample
     * @return Processed sample
     */
    virtual float processSample(float x) = 0;

    virtual ~Processor() = default;

protected:
    /**
     * @brief Common processing flow: input gain, processing, output gain, mix
     * @param x Input sample
     * @param processFunc Function to apply the specific DSP algorithm
     * @return Processed sample with gain and mix applied
     */
    float processInternal(float x, float (*processFunc)(float)) {
        float driven = x * inputGain;
        float processed = processFunc(driven);
        float output = processed * outputGain;
        return wetDryMix * output + (1.0f - wetDryMix) * x;
    }

    float inputGain;
    float outputGain;
    float wetDryMix;
};