# JClipper

Practice code for implementing a clipper in C++

## Build

Build all clippers

```bash
make all
```

# User Guide

## Input and Output

### PIPE

Process streams with clipper using PIPE to another file. ✅

```bash
cat wav/input.wav | compiled/clipper - > output.wav
```

Process streams with clipper using PIPE to a audio player such as play. ✅

```bash
cat wav/input.wav | compiled/clipper - | play -t wav -
```

## Clipper Type

Clip a wav file using different clipping types.

```bash
cat wav/input.wav | compiled/clipper --type hard - > wav/output.wav
cat wav/input.wav | compiled/clipper --type smooth - > wav/output.wav
```

<img src="./images/clipper_types_comparison.png" alt="Description of image" width="500">

## Gain compensation

Specify Input and Output Gain to drive the distortion and compensate output level afterwards.

```bash
cat wav/input.wav | compiled/clipper --input-gain 9.0 --output-gain 3.0 - > wav/output.wav
```

## Mix Control

Control the blend between the dry (original) and wet (processed) signal.

```bash
cat wav/input.wav | compiled/clipper --mix 0.7 - > wav/output.wav
```

## Alpha Parameter

Adjust the alpha parameter to control the shape of the smooth clipper curve.

```bash
cat wav/input.wav | compiled/clipper --type smooth --alpha 0.95 - > wav/output.wav
```
