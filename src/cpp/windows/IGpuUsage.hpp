#pragma once

#include <cstdint>

namespace nthompson {

    class IGpuUsage {
        public:
            IGpuUsage() = default;
            virtual ~IGpuUsage() = default;
            // Gets the GPU utilization metric
            virtual uint32_t GetGpuUsage() = 0;
            // Launches the associated application for driver settings
            virtual void LaunchAssociatedApp() = 0;
            // Release all resources and shutdown
            virtual void Shutdown() = 0;
    };
}
