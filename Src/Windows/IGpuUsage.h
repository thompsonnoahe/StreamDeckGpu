//
// Created by Noah Thompson on 5/26/2024.
//

#pragma once

#include <cstdint>

namespace nthompson {

    class IGpuUsage {
        public:
            IGpuUsage() = default;
            virtual ~IGpuUsage() = default;
            virtual uint32_t GetGpuUsage() = 0;
            virtual void LaunchAssociatedApp() = 0;
    };
}
