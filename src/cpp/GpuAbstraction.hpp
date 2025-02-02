#pragma once
#include "napi.h"
#include <string>
#include <napi.h>


namespace nthompson {

    enum class GpuVendor : int32_t {
        Nvidia,
        Amd,
        Unknown
    };

    struct Gpu : public Napi::ObjectWrap<Gpu> {
        GpuVendor vendor;
        std::string name;
        uint32_t index;
        std::string deviceId;
    };
}

