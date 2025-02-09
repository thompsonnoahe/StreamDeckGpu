#pragma once

#include <cstdint>

class IGpuUsage {
    public:
        IGpuUsage() = default;
        virtual ~IGpuUsage() = default;
        // Gets the GPU utilization metric
        virtual uint32_t GetGpuUsage() = 0;
};

class IGpuTemperature {
    public:
        IGpuTemperature() = default;
        virtual ~IGpuTemperature() = default;
        // Gets the GPU temperature metric
        virtual uint32_t GetGpuTemperature() = 0;

};

class IGpuMemoryUsage {
    public:
        IGpuMemoryUsage() = default;
        virtual ~IGpuMemoryUsage() = default;
        virtual uint64_t GetTotalMemory() = 0;
        virtual uint64_t GetUsedMemory() = 0;
};

class IGpuMetrics : public IGpuUsage, public IGpuTemperature, public IGpuMemoryUsage {
    public:
        IGpuMetrics() = default;
        // Launches the associated application for driver settings
        virtual void LaunchAssociatedApp() = 0;
        // Release all resources and shutdown
        virtual void Shutdown() = 0;
        virtual ~IGpuMetrics() = default;
};
