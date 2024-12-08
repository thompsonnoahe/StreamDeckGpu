//
// Created by Noah Thompson on 5/26/2024.
//

#pragma once
#include <SDK/ADLXHelper/Windows/Cpp/ADLXHelper.h>
#include <SDK/Include/IPerformanceMonitoring.h>
#include <SDK/Include/ADLXDefines.h>
#include <StreamDeckSDK/ESDLogger.h>
#include "../IGpuUsage.h"

namespace nthompson {

    class AmdGpuUsage : public IGpuUsage {
        public:
            explicit AmdGpuUsage(int32_t index = 0);
            ~AmdGpuUsage() override;
            uint32_t GetGpuUsage() override;
            void LaunchAssociatedApp() override;

    private:
            static inline ADLXHelper helper_;
            adlx::IADLXGPUPtr gpu_;
            adlx::IADLXPerformanceMonitoringServicesPtr perfMonitoringServices_;
            bool initialized_ = true;
    };
}

