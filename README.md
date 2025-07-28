# ChannelStrip Pipe CLI

Collection of audio processing tools designed for command-line pipe workflows.

## Table of Contents
- [Tools](#tools)
- [Build](#build)
- [User Guide](#user-guide)
  - [Global Parameters](#global-parameters)
    - [Input and Output](#input-and-output)
    - [Gain Compensation](#gain-compensation)
    - [Mix Control](#mix-control)
  - [Clipper](#clipper)
    - [Parameters](#parameters)


## Tools

- **clipper**: Audio clipping/saturation with various algorithms

## Build

Build all tools:

```bash
make all
```

Build individual tools:

```bash
make clipper
```

# User Guide

## Global Parameters

### Input and Output

#### PIPE

Process streams with clipper using PIPE to another file. ✅

```bash
cat wav/input.wav | bin/clipper - > output.wav
```

Process streams with clipper using PIPE to a audio player such as play. ✅

```bash
cat wav/input.wav | bin/clipper - | play -t wav -
```

### Gain Compensation

Specify Input and Output Gain to drive the distortion and compensate output level afterwards.

```bash
cat wav/input.wav | bin/clipper --input-gain 9.0 --output-gain 3.0 - > wav/output.wav
```

### Mix Control

Control the blend between the dry (original) and wet (processed) signal.

```bash
cat wav/input.wav | bin/clipper --mix 0.7 - > wav/output.wav
```

## Clipper

### Parameters

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| `--type` | Clipping algorithm | `hard` | `hard`, `smooth`, `tanh`, `atan`, `cubic` |
| `--input-gain` | Input gain in dB | `0.0` | Any float |
| `--output-gain` | Output gain in dB | `0.0` | Any float |
| `--alpha` | Smoothing factor | `0.0` | `0.0` - `1.0` |
| `--mix` | Wet/dry mix | `1.0` | `0.0` - `1.0` |

#### Type

Clip a wav file using different clipping types.

```bash
cat wav/input.wav | bin/clipper --type hard - > wav/output.wav
cat wav/input.wav | bin/clipper --type smooth - > wav/output.wav
```

<img src="./images/clipper_types_comparison.png" alt="Description of image" width="500">

#### Alpha

Adjust the alpha parameter to control the shape of the smooth clipper curve.

```bash
cat wav/input.wav | bin/clipper --type smooth --alpha 0.95 - > wav/output.wav
```
