#pragma once

#include <nvml.h>
#include <vector>
#include <windows.h>
#include <cstdint>
#include "../Interfaces.hpp"
#include "../../GpuAbstraction.hpp"

// Gets the GPU utilization for NVIDIA GPUs
class NvidiaGpuMetrics : public IGpuMetrics {
    public:
        explicit NvidiaGpuMetrics(std::string deviceId);
        uint32_t GetGpuUsage() override;
        uint32_t GetGpuTemperature() override;
        uint64_t GetUsedMemory() override;
        uint64_t GetTotalMemory() override;
        // Helper function to get all NVIDIA devices
        static std::vector<struct Gpu> GetGpus();
        // Launches the NVIDIA app
        void LaunchAssociatedApp() override;
        // Release all resources and shutdown
        void Shutdown() override;
    private:
        nvmlDevice_t device_;
        static inline bool initialized_;
};