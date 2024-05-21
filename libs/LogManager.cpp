#include <filesystem>
#include <stdexcept>
#include "LogManager.h"


LogManager::LogManager() {
    std::filesystem::create_directories("../../out/logs");
    const std::filesystem::path kLogsFilePath = R"(../../out/logs/log.txt)";
    auto fileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(kLogsFilePath.string(), 0, 0);
    fileSink->set_level(spdlog::level::trace);
    fileSink->set_pattern("%d/%m/%Y %-9T | ms from prev: %-5o | %^%-8l%$ | %v");

    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(spdlog::level::debug);
    consoleSink->set_pattern("%d/%m/%Y %-9T | %^%-8l%$ | %v");

    mCommonLogger = std::make_shared<spdlog::logger>("common_logger", spdlog::sinks_init_list({ fileSink, consoleSink }));
    spdlog::register_logger(mCommonLogger);
}

LogManager& LogManager::GetInstance() {
    static LogManager instance;
    return instance;
}

std::shared_ptr<spdlog::logger> LogManager::GetCommonLogger() {
    return mCommonLogger;
}

void LogManager::SetLogLevel(const std::string logLevelStr) {
    spdlog::level::level_enum logLevel;
    if (logLevelStr == "trace") {
        logLevel = spdlog::level::trace;
    }
    else if (logLevelStr == "debug") {
        logLevel = spdlog::level::debug;
    }
    else if (logLevelStr == "info") {
        logLevel = spdlog::level::info;
    }
    else if (logLevelStr == "warning") {
        logLevel = spdlog::level::warn;
    }
    else if (logLevelStr == "error") {
        logLevel = spdlog::level::err;
    }
    else if (logLevelStr == "critical") {
        logLevel = spdlog::level::critical;
    }
    else {
        throw std::runtime_error("Invalid log level specified in the JSON file");
    }
    mCommonLogger->set_level(logLevel);
}

LogManager::~LogManager() {
    spdlog::drop_all();
}
