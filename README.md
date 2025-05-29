![Gpu Banner](com.nthompson.gpu.sdPlugin/imgs/plugin/GPU%20Metrics%20Banner.png)

[![CI](https://github.com/thompsonnoahe/StreamDeckGpu/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/thompsonnoahe/StreamDeckGpu/actions/workflows/ci.yml)
[![CodeQL](https://github.com/thompsonnoahe/StreamDeckGpu/actions/workflows/github-code-scanning/codeql/badge.svg)](https://github.com/thompsonnoahe/StreamDeckGpu/actions/workflows/github-code-scanning/codeql)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# Stream Deck GPU Metrics 📊

A plugin for monitoring metrics of your GPU on the Elgato Stream Deck. Windows only.

# Installation ⚙️

Download the latest sdPlugin file from the [Releases](https://github.com/thompsonnoahe/StreamDeckGpu/releases) link on GitHub, and double-click it to install.

# Building 🛠️

## Prerequisites

- [NodeJS v20](https://nodejs.org/) or above
- [Elgato Stream Deck SDK](https://github.com/elgatosf/streamdeck)
- [CMake 3.15](https://cmake.org) or above
- [NVIDIA CUDA Toolkit v12](https://developer.nvidia.com/cuda-toolkit)
- Any Stream Deck device running the v6.4 release or newer of the Stream Deck software

Setup the repo as follows:

```commandline
git clone --recurse-submodules https://github.com/thompsonnoahe/StreamDeckGpu
cd StreamDeckGpu
npm install
npm run compile
npm run watch
```
