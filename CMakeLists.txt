cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (gpu-metrics)
find_package(CUDAToolkit)

add_definitions(-DNAPI_VERSION=7)

include_directories(${CMAKE_JS_INC})

set(SOURCE_FILES src/cpp/GpuAddon.cpp
  src/cpp/windows/amd/AmdGpuMetrics.cpp
  src/cpp/windows/nvidia/NvidiaGpuMetrics.cpp
  src/cpp/windows/Interfaces.hpp
  src/cpp/GpuAbstraction.cpp
  src/cpp/Logger.cpp)

set(THIRD_PARTY_AMD_INCLUDE src/cpp/third-party/amd/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h)
set(THIRD_PARTY_AMD_SRC src/cpp/third-party/amd/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.cpp)
set(THIRD_PARTY_AMD_SRC_WIN src/cpp/third-party/amd/SDK/Platform/Windows/WinAPIs.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
add_library(adlx STATIC ${THIRD_PARTY_AMD_INCLUDE} ${THIRD_PARTY_AMD_SRC} ${THIRD_PARTY_AMD_SRC_WIN})

set_property(TARGET ${PROJECT_NAME} PROPERTY LINK_FLAGS "/DELAYLOAD:nvml.dll")

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} CUDA::nvml adlx d3d12.lib dxgi.lib dxcore.lib)

if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  # Generate node.lib
  execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()