//
// Created by Noah Thompson on 5/25/2024.
//

#include "NvidiaGpuUsage.h"


namespace nthompson {
    NvidiaGpuUsage::NvidiaGpuUsage() {
        nvmlInit();
    }

    uint32_t NvidiaGpuUsage::GetGpuUsage() {
        if (nvmlDeviceGetHandleByIndex(0, &device_) != NVML_SUCCESS) {
            ESDLog("Failed to query device.");
            return 0;
        }

        if (nvmlDeviceGetUtilizationRates(device_, &utilization_) != NVML_SUCCESS) {
            ESDLog("Failed to query utilization.");
            return 0;
        }

        return utilization_.gpu;
    }

    NvidiaGpuUsage::~NvidiaGpuUsage() {
        nvmlShutdown();
    }


} // nthompson