//
// Created by Noah Thompson on 5/25/2024.
//

#include "GpuPlugin.h"

namespace nthompson {
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
        if (!glfwInit()) {
            ESDLog("Failed to init GLFW3");
        }

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        GLFWwindow* context = glfwCreateWindow(1, 1, "", nullptr, nullptr);

        glfwMakeContextCurrent(context);

        GLenum status = glewInit();

        if (status != GLEW_OK) {
            ESDLog("Failed to init GLEW");
        }

        std::string amd = "amd", advancedMicroDevices = "advanced micro devices", nvidia = "nvidia";
        std::string gpuVendor = reinterpret_cast<const char* const>(glGetString(GL_VENDOR));

        // Transform to lowercase
        std::transform(gpuVendor.begin(), gpuVendor.end(), gpuVendor.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        if (gpuVendor.find(amd) != std::string::npos || gpuVendor.find(advancedMicroDevices) != std::string::npos) {
            usage_ = std::make_unique<AmdGpuUsage>();
        }
        else if (gpuVendor.find(nvidia) != std::string::npos) {
            usage_ = std::make_unique<NvidiaGpuUsage>();
        }
        else {
            ESDLog("Unsupported GPU.");
            return;
        }

        timer_ = std::make_unique<Timer>();

        timer_->Start(1000, [this]() {
            Update();
        });
    }


    void GpuPlugin::Update() {
        if (mConnectionManager == nullptr) return;
        std::scoped_lock<std::mutex> lock(mutex_);
        uint32_t utilization = usage_->GetGpuUsage();
        for (const std::string& context : contexts_) {
            mConnectionManager->SetTitle(std::to_string(utilization) + "%", context, kESDSDKTarget_HardwareAndSoftware);
        }
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

}