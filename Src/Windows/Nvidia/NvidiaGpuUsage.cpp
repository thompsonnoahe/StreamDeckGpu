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

    void NvidiaGpuUsage::LaunchAssociatedApp() {
        STARTUPINFO startupInfo;
        PROCESS_INFORMATION processInformation;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        ZeroMemory(&processInformation, sizeof(processInformation));

        LPCTSTR path = R"(C:\Program Files\NVIDIA Corporation\NVIDIA app\CEF\NVIDIA app.exe)";

        CreateProcess(
            path,
            nullptr,
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &startupInfo,
            &processInformation
        );

        CloseHandle(processInformation.hProcess);
        CloseHandle(processInformation.hThread);
    }


} // nthompson