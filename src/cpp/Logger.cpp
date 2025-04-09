#include "Logger.hpp"

Napi::Value Logger::Init(const Napi::CallbackInfo &info)
{
    std::scoped_lock<std::mutex> lock(mutex_);
    errorQueue_ = Napi::ThreadSafeFunction::New(info.Env(), info[0].As<Napi::Function>(), "logCallback", 0, 1);
    return Napi::String::New(info.Env(), "Logger initialized");
}

std::shared_ptr<Logger> Logger::Instance()
{
    std::scoped_lock<std::mutex> lock(mutex_);
    if (instance_ == nullptr)
    {
        instance_ = std::shared_ptr<Logger>();
    }
    return instance_;
}

void Logger::Log(LogLevel level, const std::string &message)
{
    errorQueue_.BlockingCall([level, message](Napi::Env env, Napi::Function jsCallback)
                             { jsCallback.Call({Napi::Number::New(env, static_cast<double>(level)), Napi::String::New(env, message)}); });
}
