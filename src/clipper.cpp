#include <iostream>
#include <cmath>
#include <vector>
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

// process an entire buffer
void processBuffer(std::vector<float>& buffer, float (*clipFunc)(float))
{
    for (float& sample : buffer)
        sample = clipFunc(sample);
}

int main(int argc, char** argv)
{

    std::string clipperType;

    // Parse argv
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if ((arg == "-t" || arg == "--type") && i + 1 < argc) {
            clipperType = argv[++i];
        }
    }

    // Validate arguments
    if (clipperType.empty())
    {
        std::cerr << "Error: No clipper type specified." << std::endl;
        return 1;
    }

    // Simulate a simple waveform with values from -2 to 2
    std::vector<float> inputBuffer;
    for (float x = -2.0f; x <= 2.0f; x += 0.1f)
        inputBuffer.push_back(x);
    
    // define which clipper function to use based on type
    float (*clipperFunc)(float);
    if (clipperType == "hard") clipperFunc = hardClip;
    else if (clipperType == "tanh") clipperFunc = tanhClip;
    else if (clipperType == "atan") clipperFunc = atanClip;
    else if (clipperType == "cubic") clipperFunc = cubicClip;
    else if (clipperType == "smooth") clipperFunc = smoothClip;
    else {
        std::cerr << "Error: Invalid clipper type.\n";
        return 1;
    }
    
    // Process with one of the clippers
    std::vector<float> outputBuffer = inputBuffer;
    processBuffer(outputBuffer, clipperFunc);

    // Output results
    std::cout << "Input Buffer:" << std::endl;
    for (float i = 0; i < inputBuffer.size(); i++)
    {
        std::cout << inputBuffer[i] << "\t" << outputBuffer[i] << std::endl;
    }

    return 0;
}