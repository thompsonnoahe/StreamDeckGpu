cmake_minimum_required(VERSION 3.26)
set(VCPKG_TARGET_TRIPLET x64-windows-static)

project(com.nthompson.gpu VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)

find_package(CUDAToolkit)
find_package(GLEW CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL)

include("Src/ThirdParty/StreamDeckSDK.cmake")

set(THIRD_PARTY_AMD_INCLUDE Src/ThirdParty/Amd/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h)
set(THIRD_PARTY_AMD_SRC Src/ThirdParty/Amd/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.cpp)
set(THIRD_PARTY_AMD_SRC_WIN Src/ThirdParty/Amd/SDK/Platform/Windows/WinAPIs.cpp)

add_executable(gpu
        Src/main.cpp
        Src/Windows/Nvidia/NvidiaGpuUsage.cpp
        Src/Windows/Nvidia/NvidiaGpuUsage.h
        Src/GpuPlugin.cpp
        Src/GpuPlugin.h
        Src/Windows/Amd/AmdGpuUsage.cpp
        Src/Windows/Amd/AmdGpuUsage.h
        Src/Windows/IGpuUsage.h
)

target_include_directories(gpu PUBLIC Src/ThirdParty/Amd)

add_library(adlx STATIC ${THIRD_PARTY_AMD_INCLUDE} ${THIRD_PARTY_AMD_SRC} ${THIRD_PARTY_AMD_SRC_WIN})

target_link_libraries(gpu PRIVATE StreamDeckSDK CUDA::nvml adlx OpenGL::GL glfw GLEW::GLEW)

install(FILES Src/com.nthompson.gpu.sdPlugin/manifest.json DESTINATION Release)

