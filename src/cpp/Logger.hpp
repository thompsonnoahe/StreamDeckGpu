#pragma once
#include <memory>
#include <mutex>
#include <napi.h>

enum class LogLevel
{
    Error,
    Warning,
    Info,
    Debug
};

class Logger
{
private:
    Logger();
    static inline std::shared_ptr<Logger> instance_;
    static inline std::mutex mutex_;
    static inline Napi::ThreadSafeFunction errorQueue_;

public:
    static Napi::Value Init(const Napi::CallbackInfo &info);
    static std::shared_ptr<Logger> Instance();
    void Log(LogLevel level, const std::string &message);
    ~Logger();
};

#define SD_LOG(level, message) Logger::Instance()->Log(level, message)