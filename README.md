## Introduction

FPSGame is a next-generation first-person shooter that integrates modern graphics APIs—**Vulkan**, **OpenGL**, and **DirectX**—to showcase AAA-level visual features:

- **Ray Tracing** for global illumination, shadows, and reflections
- **Temporal Anti-Aliasing (TAA)** for smooth frame-to-frame detail
- **Volumetric Fog** and light shafts for immersive atmosphere
- **Physically Based Rendering (PBR)** for unified material and lighting
- **SSAO/HBAO** for enhanced ambient occlusion
- **Post-Processing**: HDR, tone mapping, depth-of-field, motion blur

The engine uses a modular rendering abstraction, allowing API switches at runtime for cross-platform development and performance benchmarking.


## Quick Start (Visual Studio)

```bash
# Clone the repo
git clone https://github.com/yourname/FPSGame.git
cd FPSGame/VoxelEngineVulkan

# Install dependencies (using vcpkg)
vcpkg integrate install
vcpkg install glfw3 volk glm glslang vulkan-headers vulkan-loader

# Open solution in Visual Studio
Open VoxelEngineVulkan.sln, enable vcpkg integration, set platform to x64

# Build
Select Debug or Release → Build Solution

# Run
Press F5 or run FPSGame.exe in the corresponding folder
```

## Project Structure

```
FPSGame/
├── VoxelEngineVulkan/       # Vulkan module (main branch)
│   ├── src/
│   └── VoxelEngineVulkan.sln
├── dep/
└── README.md
```
