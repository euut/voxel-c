# Voxel Engine, but in C

A simple voxel engine made in C and OpenGL without using any third-party game engines. This voxel engine is basically like a Minecraft clone where players can move around freely and interact with a infinite, procedurally generated world. It is a small personal project for me to learn about computer graphics and how game engines work under the hood, and is still a work in progress. This is also my first ever C project so a lot of the things are still not very well polished and fully optimized, but I truly enjoyed making this project and learnt a lot during the process.

## Screenshots

![Screenshot](https://github.com/user-attachments/assets/0690e31e-4796-4ac9-9735-f5fce2c8decf)

## Features

- Procedural/infinite terrain generation
- Face culling (faces that are not visible to the player are not rendered)
- Chunk management system using a hashmap
- Chunk loading and unloading
- Block placing/breaking
- Different block types
- Semi-transparent rendering (sort chunks to render water properly)
- Distant fog effect
- More features coming in the future...hopefully

## Building the Project

### Prerequisites

- Windows 10/11
- [CMake](https://cmake.org/) 3.28 or higher
- [MSYS2](https://www.msys2.org/) with MinGW-w64

### Building

Clone the repository:

```bash
git clone --recurse-submodules https://github.com/euut/voxel-c.git
cd voxel-c
```

Create a `build/` directory:

```bash
mkdir build
cd build
```

Configure and build:

```bash
cmake -G "MinGW Makefiles" ..
cmake --build .
```

Run:

```bash
.\voxel-c.exe
```

## Controls

- `WASD` - Movement
- `Space` - Fly up
- `Left Shift` - Fly down
- `Left Ctrl` - Accelerate (hold while moving)
- `Left Click` - Break block
- `Right Click` - Place block
- `1-6` - Switch block types
- `F` - Toggle wireframe rendering
- `F11` - Toggle fullscreen
- `Esc` - Exit and close
