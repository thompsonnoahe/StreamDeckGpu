#include <sstream>
#include "NvidiaGpuUsage.hpp"

namespace nthompson {
    NvidiaGpuUsage::NvidiaGpuUsage(std::string deviceId) {
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

    uint32_t NvidiaGpuUsage::GetGpuUsage() {
        if (nvmlReturn_t status = nvmlDeviceGetUtilizationRates(device_, &utilization_); status != NVML_SUCCESS) {
            std::stringstream error_status;
            error_status << "Failed to query utilization. Error: " << nvmlErrorString(status);
            return 0;
        }

        return utilization_.gpu;
    }

    std::vector<Gpu> NvidiaGpuUsage::GetGpus() {
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
            gpu.name = std::string{name};
            gpu.index = i;
            gpu.deviceId = std::string{deviceId};
            gpus.push_back(gpu);

            delete[] name;
            delete[] deviceId;
        }
        return gpus;
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

    void NvidiaGpuUsage::Shutdown() {
        nvmlReturn_t status = nvmlShutdown();
        if (status != NVML_SUCCESS) {
            std::stringstream error_status;
            error_status << "Failed to shutdown NVML. Error: " << nvmlErrorString(status);
        }
        initialized_ = false;
    }
} // nthompson