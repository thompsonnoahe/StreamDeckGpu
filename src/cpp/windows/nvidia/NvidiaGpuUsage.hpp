#pragma once

#include <nvml.h>
#include <vector>
#include <windows.h>
#include <cstdint>
#include "../IGpuUsage.hpp"
#include "../../GpuAbstraction.hpp"

namespace nthompson {
    // Gets the GPU utilization for NVIDIA GPUs
    class NvidiaGpuUsage : public IGpuUsage {
        public:
            explicit NvidiaGpuUsage(std::string deviceId);
            uint32_t GetGpuUsage() override;
            // Helper function to get all NVIDIA devices
            static std::vector<Gpu> GetGpus();
            // Launches the NVIDIA app
            void LaunchAssociatedApp() override;
            // Release all resources and shutdown
            void Shutdown() override;
        private:
            nvmlDevice_t device_;
            nvmlUtilization_t utilization_;
            static inline bool initialized_;
    };
}
