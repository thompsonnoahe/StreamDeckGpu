//
// Created by Noah Thompson on 5/25/2024.
//

#pragma once

#include <nvml.h>
#include <cstdint>
#include "StreamDeckSDK/ESDLogger.h"
#include "../IGpuUsage.h"

namespace nthompson {

    class NvidiaGpuUsage : public IGpuUsage {
        public:
            explicit NvidiaGpuUsage(int32_t index = 0);
            ~NvidiaGpuUsage() override;
            uint32_t GetGpuUsage() override;
        private:
            nvmlDevice_t device_;
            nvmlUtilization_t utilization_;
    };

}
