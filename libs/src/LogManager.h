#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/async.h>
#include <memory>
#include <vector>

#define LOG_TRACE(...)    ::LogManager::GetInstance().GetCommonLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)    ::LogManager::GetInstance().GetCommonLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)     ::LogManager::GetInstance().GetCommonLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::LogManager::GetInstance().GetCommonLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::LogManager::GetInstance().GetCommonLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::LogManager::GetInstance().GetCommonLogger()->critical(__VA_ARGS__)

class LogManager {
private:
    std::shared_ptr<spdlog::logger> mCommonLogger;

    LogManager();

public:
    ~LogManager();

    static LogManager& GetInstance();

    LogManager(const LogManager&) = delete;

    LogManager& operator= (const LogManager&) = delete;

    std::shared_ptr<spdlog::logger> GetCommonLogger();

    void SetLogLevel(const std::string level);
};