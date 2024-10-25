## Computer Graphics Assignments

This repository contains a series of assignments completed as part of a Computer Graphics course. The assignments focus on various aspects of ray tracing and rendering using different sampling methods, implemented in C++.

### Features

- **Ray Tracing**: Implementation of a basic ray tracer that simulates the way rays of light interact with objects in a scene to produce realistic images.
- **Rendering Techniques**: Exploration of different rendering techniques, including:
  - **Direct Illumination**: Calculation of light that comes directly from light sources.
  - **Global Illumination**: Simulation of light that has been reflected multiple times in the scene.
- **Sampling Methods**: Use of various sampling methods to improve the quality and efficiency of rendering, such as:
  - **Uniform Sampling**
  - **Stratified Sampling**
  - **Importance Sampling**

### Getting Started

1. **Clone the repository**:
   ```sh
   git clone https://github.com/yourusername/computer-graphics-assignments.git
   cd computer-graphics-assignments
   ```

2. **Build the project**:
   Ensure you have a C++ compiler installed. Then, run the following commands:
   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Run the assignments**:
   Each assignment is contained in its own directory. Navigate to the desired assignment directory. The path to scene config (typically named `config.json`) and the path of the output image are passed using command line arguments as follows:
    ```bash
    ./build/render <scene_path> <out_path>
    ```

### Prerequisites

- C++ Compiler (e.g., g++, clang++)
- CMake
- OpenGL (for visualization)

### Directory Structure

- `assignment1/`: Basic ray tracing
- `assignment2/`: Direct illumination
- `assignment3/`: Global illumination
- `assignment4/`: Sampling methods

### Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

### License

This project is licensed under the MIT License.
