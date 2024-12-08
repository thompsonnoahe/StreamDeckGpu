//
// Created by Noah Thompson on 5/25/2024.
//

#include <sstream>
#include "NvidiaGpuUsage.h"


namespace nthompson {
    NvidiaGpuUsage::NvidiaGpuUsage(int32_t index) {
        nvmlReturn_t status = nvmlInit();

        if (status != NVML_SUCCESS) {
            std::stringstream error_status;
            error_status << "Failed to initialize NVML. Error: " << nvmlErrorString(status);
            ESDLog(error_status.str());
            return;
        }

        if (status = nvmlDeviceGetHandleByIndex(index, &device_); status != NVML_SUCCESS) {
            std::stringstream error_status;
            error_status << "Failed to query device. Error: " << nvmlErrorString(status);
            ESDLog(error_status.str());
            return;
        }
    }

    uint32_t NvidiaGpuUsage::GetGpuUsage() {
        if (nvmlReturn_t status = nvmlDeviceGetUtilizationRates(device_, &utilization_); status != NVML_SUCCESS) {
            std::stringstream error_status;
            error_status << "Failed to query utilization. Error: " << nvmlErrorString(status);
            ESDLog(error_status.str());
            return 0;
        }

        return utilization_.gpu;
    }

    NvidiaGpuUsage::~NvidiaGpuUsage() {
        nvmlShutdown();
    }


} // nthompson