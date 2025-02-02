#pragma once
#include "../../third-party/amd/SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h"
#include "../../third-party/amd/SDK/Include/IPerformanceMonitoring.h"
#include "../../third-party/amd/SDK/Include/ADLXDefines.h"
#include <sstream>
#include "../IGpuUsage.hpp"
#include "../../GpuAbstraction.hpp"
#include <vector>

namespace nthompson {
    // Gets the GPU utilization for AMD GPUs
    class AmdGpuUsage : public IGpuUsage {
        public:
            explicit AmdGpuUsage(int32_t index = 0);
            uint32_t GetGpuUsage() override;
            // Launches AMD's Adrenalin software
            void LaunchAssociatedApp() override;
            // Helper function to get all AMD devices
            static std::vector<Gpu> GetGpus();
            // Release all resources and shutdown
            void Shutdown() override;
        private:
            adlx::IADLXGPUPtr gpu_;
            adlx::IADLXPerformanceMonitoringServicesPtr perfMonitoringServices_;
            static inline bool initialized_;
    };
}

