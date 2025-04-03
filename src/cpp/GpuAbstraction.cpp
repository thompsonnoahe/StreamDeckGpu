#include "GpuAbstraction.hpp"


Napi::Function GpuQuery::GetClass(Napi::Env env) {
    return DefineClass(env, "GpuQuery", {
            InstanceMethod("getGpus", &GpuQuery::GetGpus),
    });
}

Napi::Value GpuQuery::GetGpus(const Napi::CallbackInfo &info) {
    if (gpus_.empty()) {
        gpus_ = QueryGpus(info);
    }

    Napi::Array array = Napi::Array::New(info.Env(), gpus_.size());

    Napi::Function gpuClass = GpuWrapper::GetClass(info.Env());

    for (size_t i = 0; i < gpus_.size(); i++)
    {
        Napi::Object object = gpuClass.New({
            Napi::External<Gpu>::New(info.Env(), &gpus_[i])
        });
        array.Set(i, object);
    }

    return array;
}

std::vector<Gpu> GpuQuery::QueryGpus(const Napi::CallbackInfo &info)
{
    std::vector<Gpu> devices;
    // Use DirectX to find the GPUs
    // This is so we don't load the nvml.dll on an AMD system (would cause a crash due to missing DLL)
    // Or vice versa for AMD's ADLX library

    IDXGIFactory1* factory = nullptr;
    winrt::com_ptr<IDXCoreAdapterFactory> coreFactory;

    HRESULT result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);

    if (FAILED(result)) {
        Napi::Error::New(info.Env(), "Failed to create DXGI factory").ThrowAsJavaScriptException();
        return devices;
    }

    if (FAILED(DXCoreCreateAdapterFactory(coreFactory.put()))) {
        Napi::Error::New(info.Env(), "Failed to create core adapter factory").ThrowAsJavaScriptException();
        return devices;
    }

    std::string amd = "amd", advancedMicroDevices = "advanced micro devices", nvidia = "nvidia";

    UINT index = 0;
    IDXGIAdapter1* adapter = nullptr;
    winrt::com_ptr<IDXCoreAdapter> coreAdapter = nullptr;
    uint32_t nvidiaGpuCount = 0, amdGpuCount = 0;

    while (factory->EnumAdapters1(index, &adapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC desc;
        
        if (FAILED(adapter->GetDesc(&desc))) {
            Napi::Error::New(info.Env(), "Failed to get adapter description").ThrowAsJavaScriptException();
            adapter->Release();
            ++index;
            continue;
        }

        std::wstring wDescription = desc.Description;

        using convert_type = std::codecvt_utf8<wchar_t>;
        std::wstring_convert<convert_type, wchar_t> converter;

        std::string description = converter.to_bytes(wDescription);

        std::transform(description.begin(), description.end(), description.begin(),
                        [](char c) { return std::tolower(c); });

        if (FAILED(coreFactory->GetAdapterByLuid(desc.AdapterLuid, coreAdapter.put()))) {
            adapter->Release();
            ++index;
            continue;
        }

        if (!coreAdapter->IsPropertySupported(DXCoreAdapterProperty::IsIntegrated) || !coreAdapter->IsPropertySupported(DXCoreAdapterProperty::IsHardware)) {
            adapter->Release();
            ++index;
            continue;
        }

        bool isIntegrated = false;
        bool isHardwareAdapter;

        if (FAILED(coreAdapter->GetProperty(DXCoreAdapterProperty::IsIntegrated, &isIntegrated))) {
            adapter->Release();
            ++index;
            continue;
        }

        if (FAILED(coreAdapter->GetProperty(DXCoreAdapterProperty::IsHardware, &isHardwareAdapter))) {
            adapter->Release();
            ++index;
            continue;
        }

        if (!isHardwareAdapter) {
            adapter->Release();
            ++index;
            continue;
        }

        if (isIntegrated) {
            adapter->Release();
            ++index;
            continue;
        }

        if (description.find(nvidia) != std::string::npos) {
            nvidiaGpuCount++;
        }
        else if (description.find(amd) != std::string::npos || description.find(advancedMicroDevices) != std::string::npos) {
            amdGpuCount++;
        } 

        ++index;

        adapter->Release();
    }
    factory->Release();


    // If we do find some NVIDIA gpus, it's safe to load NVML
    if (nvidiaGpuCount > 0) {
        std::vector<Gpu> nvidiaGpus = NvidiaGpuMetrics::GetGpus();
        devices.insert(devices.end(), nvidiaGpus.begin(), nvidiaGpus.end());
    }

    // Same goes for AMD
    if (amdGpuCount > 0) {
        std::vector<Gpu> amdGpus = AmdGpuMetrics::GetGpus();
        devices.insert(devices.end(), amdGpus.begin(), amdGpus.end());
    }

    return devices;
}


GpuWrapper::GpuWrapper(const Napi::CallbackInfo &info) : Napi::ObjectWrap<GpuWrapper>(info)
{
    gpu_ = info[0].As<Napi::External<Gpu>>().Data();
}

Napi::Function GpuWrapper::GetClass(Napi::Env env)
{
    return DefineClass(env, "Gpu", {
            InstanceAccessor("vendor", &GpuWrapper::GetVendor, nullptr),
            InstanceAccessor("name", &GpuWrapper::GetName, nullptr, napi_enumerable),
            InstanceAccessor("index", &GpuWrapper::GetIndex, nullptr),
            InstanceAccessor("deviceId", &GpuWrapper::GetDeviceId, nullptr, napi_enumerable),
            InstanceAccessor("usage", &GpuWrapper::GetGpuUsage, nullptr),
            InstanceAccessor("memory", &GpuWrapper::GetGpuMemory, nullptr),
            InstanceAccessor("usedMemory", &GpuWrapper::GetGpuMemoryUsed, nullptr),
            InstanceAccessor("temperature", &GpuWrapper::GetGpuTemperature, nullptr),
            InstanceAccessor("power", &GpuWrapper::GetGpuPower, nullptr),
            InstanceMethod("launchAssociatedApp", &GpuWrapper::LaunchAssociatedApp),
    });
}

Napi::Value GpuWrapper::GetVendor(const Napi::CallbackInfo &info)
{
    if (gpu_ == nullptr)
    {
        return Napi::Number::New(info.Env(), static_cast<double>(GpuVendor::Unknown));
    }
    return Napi::Number::New(info.Env(), static_cast<double>(gpu_->vendor));
}

Napi::Value GpuWrapper::GetName(const Napi::CallbackInfo &info)
{
    if (gpu_ == nullptr)
    {
        return Napi::String::New(info.Env(), "");
    }
    return Napi::String::New(info.Env(), gpu_->name);
}

Napi::Value GpuWrapper::GetIndex(const Napi::CallbackInfo &info)
{
    if (gpu_ == nullptr)
    {
        return Napi::Number::New(info.Env(), -1);
    }
    return Napi::Number::New(info.Env(), gpu_->index);
}

Napi::Value GpuWrapper::GetDeviceId(const Napi::CallbackInfo &info)
{
    if (gpu_ == nullptr)
    {
        return Napi::String::New(info.Env(), "");
    }
    return Napi::String::New(info.Env(), gpu_->deviceId);
}

Napi::Value GpuWrapper::GetGpuUsage(const Napi::CallbackInfo &info)
{
    if (gpu_ == nullptr)
    {
        return Napi::Number::New(info.Env(), -1);
    }
    if (gpu_->metrics == nullptr)   
    {
        return Napi::Number::New(info.Env(), -1);
    }

    return Napi::Number::New(info.Env(), gpu_->metrics->GetGpuUsage());
}

Napi::Value GpuWrapper::GetGpuTemperature(const Napi::CallbackInfo &info)
{
    if (gpu_ == nullptr)
    {
        return Napi::Number::New(info.Env(), -1);
    }
    if (gpu_->metrics == nullptr)   
    {
        return Napi::Number::New(info.Env(), -1);
    }

    return Napi::Number::New(info.Env(), gpu_->metrics->GetGpuTemperature());
}

Napi::Value GpuWrapper::GetGpuMemoryUsed(const Napi::CallbackInfo &info)
{
    if (gpu_ == nullptr)
    {
        return Napi::Number::New(info.Env(), -1);
    }
    if (gpu_->metrics == nullptr)   
    {
        return Napi::Number::New(info.Env(), -1);
    }

    return Napi::Number::New(info.Env(), gpu_->metrics->GetUsedMemory());
}

Napi::Value GpuWrapper::GetGpuMemory(const Napi::CallbackInfo &info)
{
    if (gpu_ == nullptr)
    {
        return Napi::Number::New(info.Env(), -1);
    }
    if (gpu_->metrics == nullptr)   
    {
        return Napi::Number::New(info.Env(), -1);
    }

    return Napi::Number::New(info.Env(), gpu_->metrics->GetTotalMemory());
}

Napi::Value GpuWrapper::GetGpuPower(const Napi::CallbackInfo &info)
{
    if (gpu_ == nullptr) 
    {
        return Napi::Number::New(info.Env(), -1);
    }
    if (gpu_->metrics == nullptr)   
    {
        return Napi::Number::New(info.Env(), -1);
    }

    return Napi::Number::New(info.Env(), gpu_->metrics->GetGpuPowerUsage());
}

void GpuWrapper::LaunchAssociatedApp(const Napi::CallbackInfo &info)
{
    if (gpu_ == nullptr)
        return;
    
    if (gpu_->metrics == nullptr)
        return;

    gpu_->metrics->LaunchAssociatedApp();
}

void GpuWrapper::Finalize(Napi::Env env)
{
    if (gpu_ == nullptr)
        return;

    if (gpu_->metrics != nullptr)
    {
        gpu_->metrics->Shutdown();
        delete gpu_->metrics;
    }

    if (gpu_->name != nullptr && gpu_->deviceId != nullptr && gpu_->vendor == GpuVendor::Nvidia) {
        delete[] gpu_->name;
        delete[] gpu_->deviceId;
    }
}