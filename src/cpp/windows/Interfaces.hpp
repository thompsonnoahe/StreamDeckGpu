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
        // Gets the total amount of VRAM on the device
        virtual uint64_t GetTotalMemory() = 0;
        // Gets the GPU memory usage metric
        virtual uint64_t GetUsedMemory() = 0;
};

class IGpuPowerUsage {
    public:
        IGpuPowerUsage() = default;
        virtual ~IGpuPowerUsage() = default;
        // Gets the GPU power usage metric
        virtual uint32_t GetGpuPowerUsage() = 0;
};

class IGpuMetrics : public IGpuUsage, public IGpuTemperature, public IGpuMemoryUsage, public IGpuPowerUsage {
    public:
        IGpuMetrics() = default;
        // Launches the associated application for driver settings
        virtual void LaunchAssociatedApp() = 0;
        // Release all resources and shutdown
        virtual void Shutdown() = 0;
        virtual ~IGpuMetrics() = default;
};
