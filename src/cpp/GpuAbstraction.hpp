#pragma once
#include <cmath>
#include <cstdint>
#include <napi.h>
#include <string>
#include <napi.h>
#include <dxgi.h>
#include <dxcore.h>
#include <dxcore_interface.h>
#include <winrt/base.h>
#include <locale>
#include <codecvt>
#include "windows/Interfaces.hpp"
#include "windows/nvidia/NvidiaGpuMetrics.hpp"
#include "windows/amd/AmdGpuMetrics.hpp"

enum class GpuVendor : int32_t {
    Nvidia,
    Amd,
    Unknown,
};

struct Gpu
{
    GpuVendor vendor;
    const char* name;
    uint32_t index;
    const char* deviceId;
    IGpuMetrics* metrics;
};

class GpuWrapper : public Napi::ObjectWrap<GpuWrapper> {
    public:
        GpuWrapper(const Napi::CallbackInfo &info);
        static Napi::Function GetClass(Napi::Env env);
        Napi::Value GetVendor(const Napi::CallbackInfo &info);
        Napi::Value GetName(const Napi::CallbackInfo &info);
        Napi::Value GetIndex(const Napi::CallbackInfo &info);
        Napi::Value GetDeviceId(const Napi::CallbackInfo &info);
        Napi::Value GetGpuUsage(const Napi::CallbackInfo &info);
        Napi::Value GetGpuTemperature(const Napi::CallbackInfo &info);
        Napi::Value GetGpuMemoryUsed(const Napi::CallbackInfo &info);
        Napi::Value GetGpuMemory(const Napi::CallbackInfo &info);
        Napi::Value GetGpuPower(const Napi::CallbackInfo &info);
        void LaunchAssociatedApp(const Napi::CallbackInfo &info);
        void Finalize(Napi::Env env) override;
    private:
        Gpu* gpu_;
};

class GpuQuery : public Napi::ObjectWrap<GpuQuery>  {
    public: 
        GpuQuery(const Napi::CallbackInfo &info) : Napi::ObjectWrap<GpuQuery>(info) {};
        Napi::Value GetVendor(const Napi::CallbackInfo &info);
        static Napi::Function GetClass(Napi::Env env);
        Napi::Value GetGpus(const Napi::CallbackInfo &info);
    private:
        std::vector<Gpu> QueryGpus(const Napi::CallbackInfo &info);
        std::vector<Gpu> gpus_;
};

