#include "GpuAddon.hpp"
#include "GpuAbstraction.hpp"
#include "napi.h"

Napi::Object GpuAddon::Init(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "GpuQuery"), GpuQuery::GetClass(env));
    exports.Set(Napi::String::New(env, "logCallback"), Napi::Function::New(env, Logger::Init));
    return exports;
}