# Sonova

**Sonova** is a compact dual-oscillator software synthesizer built with JUCE.

The goal of v0.1 is deliberately simple: fast sound creation, a clean interface, and a small codebase that can grow into a more distinctive instrument.

## Current instrument

- 8-voice polyphony
- Two oscillators
- Sine / saw / square / triangle waveforms
- Per-oscillator octave and fine detune
- Oscillator blend
- ADSR amplitude envelope
- Resonant low-pass filter
- Smoothed master output
- MIDI input
- VST3 and Standalone builds
- DAW automation and state recall through JUCE APVTS

## Build locally

Requirements:

- CMake 3.22+
- C++17 toolchain
- JUCE 9.0.1 checkout

```bash
cmake -S . -B build -DJUCE_DIR=/path/to/JUCE
cmake --build build --config Release
```

## Windows VST3

GitHub Actions automatically builds a Windows VST3 and Standalone executable on pushes to `main` and on pull requests. The workflow artifact is named `Sonova-Windows`.

## Direction

Sonova should stay immediate and musical. Future versions can add unison, modulation, filter envelopes, effects, a preset browser, visual waveform feedback, and more character without turning the interface into a cockpit.
