
#pragma once

#include <iostream>
#include <source_location>


#ifdef NDEBUG
    #define LOG_DBG(logger) if (true) {} else logger.Log(LogLevel::Debug)
    #define LOG_INF(logger) logger.Log(LogLevel::Info)
    #define LOG_WRN(logger) logger.Log(LogLevel::Warning)
    #define LOG_ERR(logger) logger.Log(LogLevel::Error)
#else
    #define LOG_DBG(logger) logger.Log(LogLevel::Debug)
    #define LOG_INF(logger) logger.Log(LogLevel::Info)
    #define LOG_WRN(logger) logger.Log(LogLevel::Warning)
    #define LOG_ERR(logger) logger.Log(LogLevel::Error)
#endif


enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};


class Logger
{
private:

    bool showMetaInformation_ = false;

public:

    // Constructor.
    Logger(bool showMetaInformation);
    // Destructor.
    ~Logger();

    // Copy/move constructor/assignment.
    Logger(const Logger& logger) = delete;
    Logger(Logger&& logger) noexcept = delete;
    Logger& operator=(const Logger& logger) = delete;
    Logger& operator=(Logger&& logger) noexcept = delete;

    // Default parameter is evaluated at the place where the function is called.
    std::ostream& Log(LogLevel logLevel, const std::source_location& sourceLocation = std::source_location::current()) const;
};
