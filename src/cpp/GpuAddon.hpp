#include <napi.h>
#include "GpuAbstraction.hpp"
#include "windows/IGpuUsage.hpp"

namespace nthompson {
    class GpuAddon : public Napi::Addon<GpuAddon> {
        public:
            GpuAddon(Napi::Env env, Napi::Object exports);
    };
}