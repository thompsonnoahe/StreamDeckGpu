//
// Created by Noah Thompson on 5/26/2024.
//

#include "AmdGpuUsage.h"

namespace nthompson {
    AmdGpuUsage::AmdGpuUsage(int32_t index) {
        ADLX_RESULT result = helper_.Initialize();
        if (!ADLX_SUCCEEDED(result)) {
            ESDLog("Failed to initialize ADLX");
            initialized_ = false;
            return;
        }

        result = helper_.GetSystemServices()->GetPerformanceMonitoringServices(&perfMonitoringServices_);
        if (!ADLX_SUCCEEDED(result)) {
            ESDLog("Failed to get ADLX services");
            initialized_ = false;
            return;
        }
        adlx::IADLXGPUListPtr gpus;
        result = helper_.GetSystemServices()->GetGPUs(&gpus);

        if (!ADLX_SUCCEEDED(result)) {
            ESDLog("Failed to get GPUs");
            initialized_ = false;
            return;
        }

        result = gpus->At(index, &gpu_);

        if (!ADLX_SUCCEEDED(result)) {
            ESDLog("Failed to get GPU");
            initialized_ = false;
        }

    }

    uint32_t AmdGpuUsage::GetGpuUsage() {
        if (!initialized_) return 0;

        adlx::IADLXGPUMetricsSupportPtr gpuMetricsSupport;
        ADLX_RESULT result = perfMonitoringServices_->GetSupportedGPUMetrics(gpu_, &gpuMetricsSupport);

        if (!ADLX_SUCCEEDED(result)) return 0;

        adlx::IADLXAllMetricsPtr allMetrics;
        perfMonitoringServices_->GetCurrentAllMetrics(&allMetrics);
        adlx::IADLXGPUMetricsPtr gpuMetrics;
        allMetrics->GetGPUMetrics(gpu_, &gpuMetrics);

        adlx_bool supported = false;
        result = gpuMetricsSupport->IsSupportedGPUUsage(&supported);

        if (!ADLX_SUCCEEDED(result) || !supported) return 0;

        adlx_double usage = 0;
        result = gpuMetrics->GPUUsage(&usage);

        if (!ADLX_SUCCEEDED(result)) return 0;

        return static_cast<uint32_t>(std::trunc(usage));
    }

    AmdGpuUsage::~AmdGpuUsage() {
        helper_.Terminate();
    }

    void AmdGpuUsage::LaunchAssociatedApp() {
        STARTUPINFO startupInfo;
        PROCESS_INFORMATION processInformation;

        ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(startupInfo);
        ZeroMemory(&processInformation, sizeof(processInformation));

        LPCTSTR path = R"(C:\Program Files\AMD\CNext\CNext\RadeonSoftware.exe)";

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

}

