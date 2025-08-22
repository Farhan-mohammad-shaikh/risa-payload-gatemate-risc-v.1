# Memory Evaluation for Smart Sensors under Radiation

Lightweight, software-only framework to **detect and classify radiation-induced bit flips** in external SPI NOR flash using a **low-power Lattice iCE40 UltraPlus FPGA**. The system performs periodic **read-compare** cycles against a fixed reference pattern, logs mismatches to **HyperRAM**, and reports to an on-board computer (OBC). Designed for resource-constrained, space-relevant platforms (e.g., CubeSats). :contentReference[oaicite:0]{index=0}

---

## Key Features

- **Software-only SEU monitoring**: Detects **SEU**, **sticky**, and **cluster** bit flips without hardware ECC/TMR. :contentReference[oaicite:1]{index=1}  
- **Deterministic timing**: Full **16 MB** scan completes in ~**23.9 minutes** at **8 MHz**, with **sub-0.04 ms** per-flip detection latency. :contentReference[oaicite:2]{index=2}  
- **Compact logs**: Mismatches (address, offset, value) recorded to **HyperRAM**; supports ~**15,300** events (model-based). :contentReference[oaicite:3]{index=3}  
- **Low-power target**: Built around **iCE40 UltraPlus** + **IS25WP128** SPI NOR flash; adaptable to other SPI flashes. :contentReference[oaicite:4]{index=4}

---


# RISA Payload Gatemate Source code

### Creating and programming
./colognechip_gatemate_evb.py --cpu-variant=minimal --build


make clean
make
make doc
sudo make pgm



### Toolchain Setup: TODO
