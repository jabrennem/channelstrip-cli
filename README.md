# JClipper

Practice code for implementing a clipper in C++

## Build

Build all clippers
```bash
make all
```

# User Guide

## Input and Output

Specify input file.

```bash
compiled/clipper --input-file input.wav --output-file output.wav
```

Stream to stdout

```bash
compiled/clipper --input-file input.wav > output.wav
```

Output values to a csv file 

```bash
compiled/clipper --input-file input.wav --output-file output.wav --csv
```

## Clipper Type

Clip a wav file using different clipping types.

```bash
compiled/clipper --type hard --input-file input.wav --output-file output.wav
compiled/clipper --type smooth --input-file input.wav
```

<img src="./images/clipper_types_comparison.png" alt="Description of image" width="500">

## Gain compensation

Specify Input and Output Gain to drive the distortion and compensate afterwards.

```bash
compiled/clipper \
    --type hard \
    --input-gain 9.0 \
    --output-gain 6.0 \
    --input-file wav/NTM_Underoath_Kick1_44khz_16bit.wav \
    --output-file wav/NTM_Underoath_Kick1_44khz_16bit_clip_hard_in9.0_out6.0.wav
```



python plotting/plot_ntm_files.py

<img src="./images/clipper_types_comparison.png" alt="Description of image" width="800">

```bash
comp --threshold '-18' --ratio 4 --attack 0.3 --release 0.1 | 
eq --hpf-freq 30hz --lpf-f3req 12khz | 
clipper --type hard --input-gain 10db --output-gain '-10db' > out.wav
```