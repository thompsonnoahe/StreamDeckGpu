#include "AmdGpuMetrics.hpp"

AmdGpuMetrics::AmdGpuMetrics(int32_t index) {
    std::stringstream errorStatus;
    // Check so we don't have to reinitialize ADLX
    if (!initialized_) {
        // Use the provided global to prevent the library from terminating
        ADLX_RESULT result = g_ADLX.Initialize();
        if (!ADLX_SUCCEEDED(result)) {
            errorStatus << "Failed to initialize ADLX. Error: " << result;
            initialized_ = false;
            return;
        }
        initialized_ = true;
    }

    ADLX_RESULT result = g_ADLX.GetSystemServices()->GetPerformanceMonitoringServices(&perfMonitoringServices_);
    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get performance monitoring services. Error: " << result;
        initialized_ = false;
        return;
    }

    adlx::IADLXGPUListPtr gpus;
    result = g_ADLX.GetSystemServices()->GetGPUs(&gpus);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPUs. Error: " << result;
        initialized_ = false;
        return;
    }

    result = gpus->At(index, &gpu_);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPU. Error: " << result;
        initialized_ = false;
    }
}

uint32_t AmdGpuMetrics::GetGpuUsage() {
    std::stringstream errorStatus;
    if (!initialized_) {
        errorStatus << "Not initialized";
        return 0;
    }

    adlx::IADLXGPUMetricsSupportPtr gpuMetricsSupport;
    ADLX_RESULT result = perfMonitoringServices_->GetSupportedGPUMetrics(gpu_, &gpuMetricsSupport);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPU metrics support. Error: " << result;
        return 0;
    }

    adlx::IADLXAllMetricsPtr allMetrics;

    result = perfMonitoringServices_->GetCurrentAllMetrics(&allMetrics);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get all metrics. Error: " << result;
        return 0;
    }


    adlx::IADLXGPUMetricsPtr gpuMetrics;
    result = allMetrics->GetGPUMetrics(gpu_, &gpuMetrics);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPU metrics. Error: " << result;
        return 0;
    }

    adlx_bool supported = false;
    result = gpuMetricsSupport->IsSupportedGPUUsage(&supported);

    if (!ADLX_SUCCEEDED(result) || !supported) {
        errorStatus << "Failed to get GPU usage support, or GPU usage is not supported. Error: " << result;
        return 0;
    }

    adlx_double usage = 0;
    result = gpuMetrics->GPUUsage(&usage);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPU usage. Error: " << result;
        return 0;
    }

    return static_cast<uint32_t>(std::trunc(usage));
}

uint32_t AmdGpuMetrics::GetGpuTemperature()
{
   std::stringstream errorStatus;
    if (!initialized_) {
        errorStatus << "Not initialized";
        return 0;
    }

    adlx::IADLXGPUMetricsSupportPtr gpuMetricsSupport;
    ADLX_RESULT result = perfMonitoringServices_->GetSupportedGPUMetrics(gpu_, &gpuMetricsSupport);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPU metrics support. Error: " << result;
        return 0;
    }

    adlx::IADLXAllMetricsPtr allMetrics;

    result = perfMonitoringServices_->GetCurrentAllMetrics(&allMetrics);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get all metrics. Error: " << result;
        return 0;
    }


    adlx::IADLXGPUMetricsPtr gpuMetrics;
    result = allMetrics->GetGPUMetrics(gpu_, &gpuMetrics);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPU metrics. Error: " << result;
        return 0;
    }

    adlx_bool supported = false;
    result = gpuMetricsSupport->IsSupportedGPUTemperature(&supported);

    if (!ADLX_SUCCEEDED(result) || !supported) {
        errorStatus << "Failed to get GPU temperature support, or GPU temperature is not supported. Error: " << result;
        return 0;
    }

    adlx_double temp = 0;
    result = gpuMetrics->GPUTemperature(&temp);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPU temperature. Error: " << result;
        return 0;
    }

    return static_cast<uint32_t>(std::trunc(temp));
}

uint64_t AmdGpuMetrics::GetUsedMemory()
{
    std::stringstream errorStatus;
    if (!initialized_) {
        errorStatus << "Not initialized";
        return 0;
    }

    adlx::IADLXGPUMetricsSupportPtr gpuMetricsSupport;
    ADLX_RESULT result = perfMonitoringServices_->GetSupportedGPUMetrics(gpu_, &gpuMetricsSupport);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPU metrics support. Error: " << result;
        return 0;
    }

    adlx::IADLXAllMetricsPtr allMetrics;

    result = perfMonitoringServices_->GetCurrentAllMetrics(&allMetrics);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get all metrics. Error: " << result;
        return 0;
    }

    adlx::IADLXGPUMetricsPtr gpuMetrics;
    result = allMetrics->GetGPUMetrics(gpu_, &gpuMetrics);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPU metrics. Error: " << result;
        return 0;
    }

    adlx_bool supported = false;
    result = gpuMetricsSupport->IsSupportedGPUTemperature(&supported);

    if (!ADLX_SUCCEEDED(result) || !supported) {
        errorStatus << "Failed to get GPU temperature support, or GPU temperature is not supported. Error: " << result;
        return 0;
    }

    adlx_int vram = 0;
    result = gpuMetrics->GPUVRAM(&vram);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get GPU temperature. Error: " << result;
        return 0;
    }

    return vram;
}

uint64_t AmdGpuMetrics::GetTotalMemory()
{
    adlx_uint totalVram = 0;
    std::stringstream errorStatus;

    ADLX_RESULT result = gpu_->TotalVRAM(&totalVram);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get total VRAM. Error: " << result;
        return 0;
    }

    return totalVram;
}

void AmdGpuMetrics::LaunchAssociatedApp() {
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

std::vector<Gpu> AmdGpuMetrics::GetGpus() {
    std::vector<Gpu> gpus;
    std::stringstream errorStatus;

    // Again, check so we don't have to reinitialize ADLX
    if (!initialized_) {
        ADLX_RESULT result = g_ADLX.Initialize();
        if (!ADLX_SUCCEEDED(result)) {
            errorStatus << "Failed to initialize ADLX. Error: " << result;
            initialized_ = false;
            return gpus;
        }
        initialized_ = true;
    }

    adlx::IADLXGPUListPtr devices;
    ADLX_RESULT result = g_ADLX.GetSystemServices()->GetGPUs(&devices);

    if (!ADLX_SUCCEEDED(result)) {
        errorStatus << "Failed to get devices. Error: " << result;
        return gpus;
    }

    for (adlx_uint i = 0; i < devices->Size(); ++i) {
        adlx::IADLXGPUPtr device;
        result = devices->At(i, &device);

        if (!ADLX_SUCCEEDED(result)) {
            errorStatus << "Failed to get device at index " << i << ". Error: " << result;
            continue;
        }

        ADLX_GPU_TYPE type;
        result = device->Type(&type);

        if (!ADLX_SUCCEEDED(result)) {
            errorStatus << "Failed to get device type. Error: " << result;
            continue;
        }

        if (type == ADLX_GPU_TYPE::GPUTYPE_INTEGRATED) {
            continue;
        }

        const char *name = "";

        result = device->Name(&name);

        if (!ADLX_SUCCEEDED(result)) {
            errorStatus << "Failed to get device name. Error: " << result;
            continue;
        }

        const char* deviceId = "";

        result = device->DeviceId(&deviceId);

        if (!ADLX_SUCCEEDED(result)) {
            errorStatus << "Failed to get device id. Error: " << result;
            continue;
        }

        Gpu gpu;
        gpu.vendor = GpuVendor::Amd;
        gpu.name = name;
        gpu.index = i;
        gpu.deviceId = deviceId;
        gpu.metrics = new AmdGpuMetrics(i);
        gpus.push_back(gpu);
    }
    return gpus;
}

void AmdGpuMetrics::Shutdown() {
    if (initialized_) {
        g_ADLX.Terminate();
        initialized_ = false;
    }
}
