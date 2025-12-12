cmake_minimum_required(VERSION 3.24)
project(GpuScienceLab LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

# OpenGL
find_package(OpenGL REQUIRED)

add_executable(GpuScienceLab
  src/main.cpp
  src/MainWindow.h src/MainWindow.cpp
  src/GLView.h src/GLView.cpp
  src/SimulationCuda.h src/SimulationCuda.cu
)

# Qt MOC/UIC
set_target_properties(GpuScienceLab PROPERTIES
  AUTOMOC ON
  AUTOUIC ON
  AUTORCC ON
)

target_link_libraries(GpuScienceLab PRIVATE
  Qt6::Widgets
  Qt6::OpenGL
  Qt6::OpenGLWidgets
  OpenGL::GL
)

# CUDA flags
set_target_properties(GpuScienceLab PROPERTIES
  CUDA_SEPARABLE_COMPILATION ON
)

# Pick a sensible arch for RTX 4070 (Ada Lovelace is SM 8.9)
# If you prefer, you can remove this and let nvcc decide.
set_property(TARGET GpuScienceLab PROPERTY CUDA_ARCHITECTURES 89)

