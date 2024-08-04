//
// Created by Noah Thompson on 5/25/2024.
//

#pragma once

#include <StreamDeckSDK/ESDPlugin.h>
#include <StreamDeckSDK/ESDConnectionManager.h>
#include <memory>
#include <thread>
#include <mutex>
#include <set>
#include <dxgi.h>
#include <algorithm>
#include <functional>
#include "Windows/Nvidia/NvidiaGpuUsage.h"
#include "Windows/Amd/AmdGpuUsage.h"

namespace nthompson {

    class Timer {
    public:
        void Start(int32_t interval, const std::function<void()>& func);
        void Stop();
    private:
        std::thread thread_;
        std::mutex mutex_;
        std::atomic<bool> running_ = false;
    };

    class GpuPlugin : public ESDBasePlugin {
    public:
        GpuPlugin();
        ~GpuPlugin() override;

        void Update();

        void SetActionText(std::string text);

        void WillAppearForAction(const std::string& inAction,
                                 const std::string& inContext,
                                 const nlohmann::json& inPayload,
                                 const std::string& inDeviceID) override;

        void WillDisappearForAction(
                const std::string& inAction,
                const std::string& inContext,
                const nlohmann::json& inPayload,
                const std::string& inDeviceID) override;
    private:
        std::unique_ptr<IGpuUsage> usage_ = nullptr;
        std::unique_ptr<Timer> timer_;
        std::mutex mutex_;
        std::set<std::string> contexts_;
    };

} // nthompson
