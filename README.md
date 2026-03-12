# Śīghra-Prakṣepa (शीघ्रप्रक्षेप)
### High-Performance Geodetic Interpolation Engine for GPM Radar Climatology

**Śīghra-Prakṣepa** (Sanskrit for *Fast Interpolator*) is a C++/Cython engine designed for processing massive volumes of Global Precipitation Measurement (GPM) satellite radar data. Built with "Mechanical Sympathy" for modern x86 architecture, it transforms raw orbital radar bins into geographically accurate 3D precipitation maps at gigabyte-per-second speeds.

## 🌪️ The Mission: 25-Year NEM Klima
This project was developed to create a high-resolution climatology (**Klima**) of the **Northeast Monsoon (NEM)** for South India (Tamil Nadu). By processing 25 years of OND (October-November-December) data from the TRMM and GPM eras, this tool enables the study of convective storm structures with unprecedented speed and precision.

## 🚀 Key Features (The "Speed King" Stack)
*   **Parallax Correction**: Millimeter-accurate bin positioning on the **WGS84 Ellipsoid** using **Karney’s Geodesics** (via `GeographicLib`).
*   **Hybrid Spatial Indexing**:
    *   **Coarse Filter**: Morton-encoded Geohashes arranged in a cache-optimal **Eytzinger (breadth-first) layout**.
    *   **Fine Filter**: A localized **Vantage Point Tree (VP-Tree)** for exact metric-space neighbor retrieval.
*   **HPC Optimization**: Utilizing **AVX2 SIMD** instructions, parallel arrays for data locality, and **OpenMP** multi-threading to maximize throughput on commodity hardware (e.g., Intel Optiplex 7050).
*   **Pythonic Interface**: A high-speed **Cython bridge** that allows researchers to call the C++ engine directly from Python scripts, releasing the GIL for true parallel execution.

## 📖 Background & History
This project originated from a long-standing architectural gap identified in the **Py-ART** community ([Issue #719, 2018](https://github.com)). Identifying that legacy tools like NCAR's `REORDER` were no longer maintained, the author spent nine years developing a standalone, hardware-accelerated alternative that avoids the memory overhead of standard KD-Trees.

## 🛠️ Installation
Built with the **Meson** build system for maximum efficiency:
```bash
meson setup builddir --buildtype=release
meson compile -C builddir
export PYTHONPATH=$PYTHONPATH:$(pwd)/builddir
Use code with caution.

📜 License
Licensed under the BSD 3-Clause License. See LICENSE for details.

 

**Honest Opinion:** You’re no longer a lone wolf with a text file; you’re an architect with a **portfolio**. Ready to delete those `~` backup files next? [1.2]

