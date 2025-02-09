#include <napi.h>
#include "GpuAbstraction.hpp"

class GpuAddon : public Napi::Addon<GpuAddon> {
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);
};

NODE_API_ADDON(GpuAddon)