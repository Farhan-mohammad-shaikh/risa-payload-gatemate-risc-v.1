# RISA Payload – Gatemate RISC-V Platform

## Overview

This project implements a RISC-V based payload targeting the Cologne Chip Gatemate FPGA platform.  
It is designed as part of a radiation and reliability-focused embedded experiment framework, where a minimal RISC-V system performs controlled memory evaluation and monitoring tasks.

The goal of this repository is to provide:

- A minimal RISC-V soft-core configuration
- Build and programming flow for the Gatemate evaluation board
- Structured payload execution framework
- Clean reproducible FPGA build setup

This project reflects FPGA system design, embedded firmware integration, and structured hardware-software co-design.

---

## Platform

- FPGA: Cologne Chip Gatemate
- CPU: Minimal RISC-V soft-core configuration
- Build Flow: Python + Make
- Host OS: Linux recommended

---

## Project Structure

```
risa-payload-gatemate-risc-v.1/
├── colognechip_gatemate_evb.py
├── Makefile
├── README.md
└── .vscode/
```

The Python script configures and builds the minimal RISC-V system.  
The Makefile automates build, documentation generation, and programming steps.

---

## Build Instructions

### Clean Build

```bash
make clean
```

### Compile Design

```bash
make
```

### Generate Documentation (if configured)

```bash
make doc
```

---

## Program FPGA

```bash
sudo make pgm
```

Ensure the Gatemate board is connected and recognized before programming.

---

## Alternative Build Method

You may also build directly using:

```bash
./colognechip_gatemate_evb.py --cpu-variant=minimal --build
```

This builds the minimal RISC-V configuration for the evaluation board.

---

## System Design Concept

The RISC-V payload is designed to:

- Execute deterministic memory routines
- Perform structured read/compare cycles
- Support reliability experimentation workflows
- Operate within constrained FPGA resources

The minimal CPU configuration reduces complexity while enabling controlled experiment execution.

---

## Applications

- Radiation reliability experimentation
- Memory validation payload development
- FPGA-based embedded system prototyping
- Academic research in fault-tolerant systems
- CubeSat payload experimentation frameworks

---

## Key Technical Highlights

- Minimal RISC-V soft-core configuration
- FPGA toolchain integration
- Structured Make-based build flow
- Clean reproducible build process
- Designed for reliability-focused embedded experimentation

---

## Requirements

- Linux host system
- Python 3
- Make
- Cologne Chip Gatemate FPGA toolchain installed and configured
- Gatemate evaluation board

---
