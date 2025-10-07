# GlEngine
GlEngine is a simple graphics engine I built for fun.
It serves as a personal learning project exploring OpenGL rendering, procedural terrain, and terrain destruction.

# Features
- Procedural terrain generation using the Marching Cubes algorithm.
- Intended support for Transvoxel to handle LOD transitions and stitch chunk borders (not implemented).
- Terrain destruction is functional, but density field isn't shared across neighbouring chunks; terrain construction is experimental.
- All terrain management currently runs on the CPU, meaning it's not optimized (a lot of calculation could have been made GPU side with compute shaders).

Rendering is done using pure OpenGL.

# Status & Limitations
- The project does not build directly from this repository.
- Some parts of the architecture are experimental, and I have doubts about their design.
- Compute shaders for GPU-side terrain management were attempted but not completed.
- This repository contains source code only, intended as reference for future projects.

# Notes
This project is primarily a learning exercise and a template for my future work.
I’m sharing it because I’m proud of having implemented something completely from scratch, even though graphics programming isn’t my main field (I’m a physics student at university).
