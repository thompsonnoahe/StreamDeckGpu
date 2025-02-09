#pragma once
#include "../../third-party/amd/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h"
#include "../../third-party/amd/SDK/Include/IPerformanceMonitoring.h"
#include "../../third-party/amd/SDK/Include/ADLXDefines.h"
#include <sstream>
#include "../Interfaces.hpp"
#include "../../GpuAbstraction.hpp"
#include <vector>

// Gets the GPU utilization for AMD GPUs
class AmdGpuMetrics : public IGpuMetrics {
    public:
        explicit AmdGpuMetrics(int32_t index = 0);
        uint32_t GetGpuUsage() override;
        uint32_t GetGpuTemperature() override;
        uint64_t GetUsedMemory() override;
        uint64_t GetTotalMemory() override;
        // Launches AMD's Adrenalin software
        void LaunchAssociatedApp() override;
        // Helper function to get all AMD devices
        static std::vector<struct Gpu> GetGpus();
        // Release all resources and shutdown
        void Shutdown() override;
    private:
        adlx::IADLXGPUPtr gpu_;
        adlx::IADLXPerformanceMonitoringServicesPtr perfMonitoringServices_;
        static inline bool initialized_;
};
