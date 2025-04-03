#include <sstream>
#include "NvidiaGpuMetrics.hpp"

NvidiaGpuMetrics::NvidiaGpuMetrics(std::string deviceId) {
    nvmlReturn_t status;

    // Check so we don't have to reinitialize NVML
    if (!initialized_) {
        nvmlReturn_t init = nvmlInit();
        if (init != NVML_SUCCESS) {
            std::stringstream error_status;
            error_status << "Failed to initialize NVML. Error: " << nvmlErrorString(init);
            return;
        }
        initialized_ = true;
    }

    if (status = nvmlDeviceGetHandleByUUID(deviceId.c_str(), &device_); status != NVML_SUCCESS) {
        std::stringstream error_status;
        error_status << "Failed to query device. Error: " << nvmlErrorString(status);
        return;
    }
}

uint32_t NvidiaGpuMetrics::GetGpuUsage() {
    nvmlUtilization_t utilization;

    if (nvmlReturn_t status = nvmlDeviceGetUtilizationRates(device_, &utilization); status != NVML_SUCCESS) {
        std::stringstream error_status;
        error_status << "Failed to query utilization. Error: " << nvmlErrorString(status);
        return 0;
    }

    return utilization.gpu;
}

uint32_t NvidiaGpuMetrics::GetGpuTemperature()
{
    uint32_t temperature;
    if (nvmlReturn_t status = nvmlDeviceGetTemperature(device_, NVML_TEMPERATURE_GPU, &temperature); status != NVML_SUCCESS) {
        std::stringstream error_status;
        error_status << "Failed to query temperature. Error: " << nvmlErrorString(status);
        return 0;
    }

    return temperature;
}

uint64_t NvidiaGpuMetrics::GetUsedMemory()
{
    nvmlMemory_t memory;

    if (nvmlReturn_t status = nvmlDeviceGetMemoryInfo(device_, &memory); status != NVML_SUCCESS) {
        std::stringstream error_status;
        error_status << "Failed to query memory. Error: " << nvmlErrorString(status);
        return 0;
    }

    return memory.used;
}

uint64_t NvidiaGpuMetrics::GetTotalMemory()
{
    nvmlMemory_t memory;

    if (nvmlReturn_t status = nvmlDeviceGetMemoryInfo(device_, &memory); status != NVML_SUCCESS) {
        std::stringstream error_status;
        error_status << "Failed to query memory. Error: " << nvmlErrorString(status);
        return 0;
    }

    return memory.total;
}

uint32_t NvidiaGpuMetrics::GetGpuPowerUsage()
{
    uint32_t powerUsage;

    if (nvmlReturn_t status = nvmlDeviceGetPowerUsage(device_, &powerUsage); status != NVML_SUCCESS) {
        std::stringstream errorStatus;
        errorStatus << "Failed to query power usage. Error: " << nvmlErrorString(status);
        return 0;
    }

    return powerUsage;
}

std::vector<Gpu> NvidiaGpuMetrics::GetGpus() {
    std::vector<Gpu> gpus;

    // Again, check so we don't have to reinitialize NVML
    if (!initialized_) {
        nvmlReturn_t init = nvmlInit();
        if (init != NVML_SUCCESS) {
            std::stringstream error_status;
            error_status << "Failed to initialize NVML. Error: " << nvmlErrorString(init);
            return gpus;
        }
        initialized_ = true;
    }

    uint32_t count;
    if (nvmlReturn_t status = nvmlDeviceGetCount(&count); status != NVML_SUCCESS) {
        std::stringstream error_status;
        error_status << "Failed to query device count. Error: " << nvmlErrorString(status);
        return gpus;
    }

    for (uint32_t i = 0; i < count; ++i) {
        nvmlDevice_t device;
        if (nvmlReturn_t status = nvmlDeviceGetHandleByIndex(i, &device); status != NVML_SUCCESS) {
            std::stringstream error_status;
            error_status << "Failed to query device. Error: " << nvmlErrorString(status);
            continue;
        }

        char *name = new char[NVML_DEVICE_NAME_V2_BUFFER_SIZE];

        if (nvmlReturn_t status = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_V2_BUFFER_SIZE); status !=
                                                                                                    NVML_SUCCESS) {
            std::stringstream error_status;
            error_status << "Failed to query device name. Error: " << nvmlErrorString(status);
            continue;
        }

        char *deviceId = new char[NVML_DEVICE_UUID_V2_BUFFER_SIZE];

        // Get the device UUID so we can select the GPU in the property inspector once settings are received
        if (nvmlReturn_t status = nvmlDeviceGetUUID(device, deviceId, NVML_DEVICE_UUID_V2_BUFFER_SIZE); status !=
                                                                                                        NVML_SUCCESS) {
            std::stringstream error_status;
            error_status << "Failed to query device id. Error: " << nvmlErrorString(status);
            continue;
        }

        Gpu gpu;
        gpu.vendor = GpuVendor::Nvidia;
        gpu.name = name;
        gpu.index = i;
        gpu.deviceId = deviceId;
        gpu.metrics = new NvidiaGpuMetrics(gpu.deviceId);
        gpus.push_back(gpu);
    }
    return gpus;
}

void NvidiaGpuMetrics::LaunchAssociatedApp() {
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInformation;

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInformation, sizeof(processInformation));

    LPCSTR path = R"(C:\Program Files\NVIDIA Corporation\NVIDIA app\CEF\NVIDIA app.exe)";

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

void NvidiaGpuMetrics::Shutdown() {
    nvmlReturn_t status = nvmlShutdown();
    if (status != NVML_SUCCESS) {
        std::stringstream error_status;
        error_status << "Failed to shutdown NVML. Error: " << nvmlErrorString(status);
    }
    initialized_ = false;
}