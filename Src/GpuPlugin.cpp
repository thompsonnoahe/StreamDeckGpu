//
// Created by Noah Thompson on 5/25/2024.
//

#include "GpuPlugin.h"

namespace nthompson {
    // Convert a wide Unicode string to an UTF8 string
    std::string ConvToString(const std::wstring &wStr)
    {
        if(wStr.empty()) return {};
        int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &wStr[0], (int)wStr.size(), nullptr, 0, nullptr, nullptr);
        std::string strTo(sizeNeeded, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wStr[0], (int)wStr.size(), &strTo[0], sizeNeeded, nullptr, nullptr);
        return strTo;
    }

    void Timer::Start(int32_t interval, const std::function<void()>& func) {
        if (running_) return;
        running_ = true;
        thread_ = std::thread([this, &interval, func]() {
            while (running_) {
                std::scoped_lock<std::mutex> lock(mutex_);
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
        IDXGIFactory* factory = nullptr;

        HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);

        if (FAILED(result)) {
            ESDLog("Failed to create factory object.");
            return;
        }

        std::string amd = "amd", advancedMicroDevices = "advanced micro devices", nvidia = "nvidia";

        UINT index = 0;
        IDXGIAdapter* adapter = nullptr;

        nlohmann::json payload;

        while (factory->EnumAdapters(index, &adapter) != DXGI_ERROR_NOT_FOUND) {
            DXGI_ADAPTER_DESC desc;
            adapter->GetDesc(&desc);
            std::wstring wDescription = desc.Description;

            std::string description = ConvToString(wDescription);

            std::transform(description.begin(), description.end(), description.begin(),
                           [](char c) { return std::tolower(c); });

            if (description.find(nvidia) != std::string::npos) {
                usage_ = std::make_unique<NvidiaGpuUsage>();
                break;
            }
            else if (description.find(amd) != std::string::npos || description.find(advancedMicroDevices) != std::string::npos) {
                usage_ = std::make_unique<AmdGpuUsage>();
                break;
            } else {
                ESDLog("Found unsupported display adapter");
                usage_ = nullptr;
            }

            ++index;
        }


        timer_ = std::make_unique<Timer>();

        timer_->Start(1000, [this]() {
            Update();
        });
    }


    void GpuPlugin::Update() {
        if (mConnectionManager == nullptr) return;
        std::scoped_lock<std::mutex> lock(mutex_);

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
    }

    void GpuPlugin::WillDisappearForAction(const std::string &inAction, const std::string &inContext,
                                           const nlohmann::json &inPayload, const std::string &inDeviceID) {
        std::scoped_lock<std::mutex> lock(mutex_);
        contexts_.erase(inContext);
    }

    void GpuPlugin::SetActionText(std::string text) {
        for (const std::string& context : contexts_) {
            mConnectionManager->SetTitle(text, context, kESDSDKTarget_HardwareAndSoftware);
        }
    }

}