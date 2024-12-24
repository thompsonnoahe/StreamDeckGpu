//
// Created by Noah Thompson on 5/25/2024.
//

#include "GpuPlugin.h"

namespace nthompson {
    // Convert a wide Unicode string to an UTF8 string
    std::string ConvToString(const std::wstring &wStr) {
        if(wStr.empty()) return {};
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wStr[0], (int)wStr.size(), nullptr, 0, nullptr, nullptr);
        std::string strTo(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wStr[0], (int)wStr.size(), &strTo[0], sizeNeeded, nullptr, nullptr);
        return strTo;
    }

    void Timer::Start(const int32_t& interval, const std::function<void()>& func) {
        if (running_) return;
        running_ = true;
        thread_ = std::thread([this, interval, func]() {
            std::scoped_lock<std::mutex> lock(mutex_);
            while (running_) {
                func();
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            }
        });
    }

    void Timer::Stop() {
        running_ = false;
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    GpuPlugin::GpuPlugin() {
        FindAvailableGpus();

        timer_ = std::make_unique<Timer>();

        timer_->Start(1000, [this]() {
            Update();
        });
    }


    void GpuPlugin::Update() {
        if (usage_ == nullptr) {
            SetActionText("?");
            return;
        }

        uint32_t utilization = usage_->GetGpuUsage();

        std::stringstream stream;
        stream << std::to_string(utilization) << "%";

        SetActionText(stream.str());
    }

    GpuPlugin::~GpuPlugin() {
        timer_->Stop();
    }

    void GpuPlugin::WillAppearForAction(const std::string &inAction, const std::string &inContext,
                                        const nlohmann::json &inPayload, const std::string &inDeviceID) {
        std::scoped_lock<std::mutex> lock(mutex_);
        contexts_.insert(inContext);

        if (inPayload.contains("settings")) {
            Gpu gpu = inPayload["settings"]["gpuInfo"];
            HandleSelectedGpu(gpu);
        }
    }

    void GpuPlugin::WillDisappearForAction(const std::string &inAction, const std::string &inContext,
                                           const nlohmann::json &inPayload, const std::string &inDeviceID) {
        std::scoped_lock<std::mutex> lock(mutex_);
        contexts_.erase(inContext);
    }

    void GpuPlugin::SetActionText(const std::string& text) {
        for (const std::string& context : contexts_) {
            mConnectionManager->SetTitle(text, context, kESDSDKTarget_HardwareAndSoftware);
        }
    }

    void GpuPlugin::FindAvailableGpus() {
        IDXGIFactory1* factory = nullptr;
        winrt::com_ptr<IDXCoreAdapterFactory> coreFactory;

        HRESULT result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);

        if (FAILED(result)) {
            ESDLog("Failed to create factory object.");
            return;
        }

        if (FAILED(DXCoreCreateAdapterFactory(coreFactory.put()))) {
            ESDLog("Failed to create core factory object.");
            return;
        }

        std::string amd = "amd", advancedMicroDevices = "advanced micro devices", nvidia = "nvidia";

        UINT index = 0;
        IDXGIAdapter1* adapter = nullptr;
        winrt::com_ptr<IDXCoreAdapter> coreAdapter = nullptr;

        uint32_t nvidiaGpuCount = 0, amdGpuCount = 0;

        while (factory->EnumAdapters1(index, &adapter) != DXGI_ERROR_NOT_FOUND) {
            DXGI_ADAPTER_DESC desc;
            adapter->GetDesc(&desc);
            std::wstring wDescription = desc.Description;

            std::string description = ConvToString(wDescription);
            std::string gpuName = description;

            std::transform(description.begin(), description.end(), description.begin(),
                           [](char c) { return std::tolower(c); });

            if (FAILED(coreFactory->GetAdapterByLuid(desc.AdapterLuid, coreAdapter.put()))) {
                ESDLog("Failed to get core adapter.");
                adapter->Release();
                ++index;
                continue;
            }

            if (!coreAdapter->IsPropertySupported(DXCoreAdapterProperty::IsIntegrated) || !coreAdapter->IsPropertySupported(DXCoreAdapterProperty::IsHardware)) {
                ESDLog("Adapter does not support required properties.");
                adapter->Release();
                ++index;
                continue;
            }

            bool isIntegrated = false;
            bool isHardwareAdapter;

            if (FAILED(coreAdapter->GetProperty(DXCoreAdapterProperty::IsIntegrated, &isIntegrated))) {
                ESDLog("Failed to get integrated property.");
                adapter->Release();
                ++index;
                continue;
            }

            if (FAILED(coreAdapter->GetProperty(DXCoreAdapterProperty::IsHardware, &isHardwareAdapter))) {
                ESDLog("Failed to get hardware property.");
                adapter->Release();
                ++index;
                continue;
            }

            if (!isHardwareAdapter) {
                ESDLog("Adapter is not a hardware adapter.");
                adapter->Release();
                ++index;
                continue;
            }

            if (isIntegrated) {
                ESDLog("Adapter is iGPU.");
                adapter->Release();
                ++index;
                continue;
            }

            std::pair<UINT, Gpu> item;

            item.first = desc.DeviceId;

            if (description.find(nvidia) != std::string::npos) {
                item.second = {GpuVendor::Nvidia, gpuName, nvidiaGpuCount, item.first};
                gpus_.insert(item);
                std::string gpuLog = gpuName + " found";
                ESDLog(gpuLog);
                nvidiaGpuCount++;
            }
            else if (description.find(amd) != std::string::npos || description.find(advancedMicroDevices) != std::string::npos) {
                item.second = {GpuVendor::Amd, gpuName, amdGpuCount, item.first};
                gpus_.insert(item);
                std::string gpuLog = gpuName + " found";
                ESDLog(gpuLog);
                amdGpuCount++;
            } else {
                ESDLog("Found unsupported display adapter");
            }

            ++index;

            adapter->Release();
        }
        factory->Release();
    }

    void
    GpuPlugin::SendToPlugin(const std::string &inAction, const std::string &inContext, const nlohmann::json &inPayload,
                            const std::string &inDeviceID) {
        nlohmann::json payload;
        if (inPayload.at("propertyInspectorLoaded").get<bool>()) {
            payload["gpus"] = gpus_;
            payload["selected"] = selectedGpu_;
            mConnectionManager->SendToPropertyInspector(inAction, inContext, payload);
        }

        if (inPayload.at("receiveSelection").get<bool>()) {
            Gpu gpu = inPayload["gpuInfo"];
            HandleSelectedGpu(gpu);
            mConnectionManager->SendToPropertyInspector(inAction, inContext, payload);
            mConnectionManager->SetSettings(inPayload, inContext);
        }
    }

    void GpuPlugin::HandleSelectedGpu(const Gpu &gpu) {
        switch (gpu.vendor) {
            case GpuVendor::Nvidia:
                usage_ = std::make_unique<NvidiaGpuUsage>(gpu.index);
                break;
            case GpuVendor::Amd:
                usage_ = std::make_unique<AmdGpuUsage>(gpu.index);
                break;
            case GpuVendor::Unknown:
                usage_ = nullptr;
                break;
        }
        selectedGpu_ = gpu;
    }

    void GpuPlugin::KeyDownForAction(const std::string &inAction, const std::string &inContext,
                                     const nlohmann::json &inPayload, const std::string &inDeviceID) {
        if (!usage_) return;

        usage_->LaunchAssociatedApp();
    }
}