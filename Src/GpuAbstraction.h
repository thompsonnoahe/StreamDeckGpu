//
// Created by Noah Thompson on 8/7/2024.
//

#pragma once
#include <string>
#include <nlohmann/json.hpp>


namespace nthompson {

    enum class GpuVendor : int32_t {
        Nvidia,
        Amd,
        Unknown
    };

    struct Gpu {
        GpuVendor vendor;
        std::string name;
        uint32_t index;
        UINT deviceId;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Gpu, vendor, name, index, deviceId);
}

