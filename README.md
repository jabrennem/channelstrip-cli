# JClipper

Practice code for implementing a clipper in C++


Build all clippers
```bash
make all
```


Clip a wav file using different clipping type.

```bash
compiled/clipper -t hard -ig 9.0 -og 6.0
compiled/clipper -t tanh -ig 9.0 -og 6.0
compiled/clipper -t smooth -ig 9.0 -og 6.0
```

python plotting/plot_ntm_files.py

<img src="./images/clipper_types_comparison.png" alt="Description of image" width="800">
