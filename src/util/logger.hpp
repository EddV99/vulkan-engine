#pragma once
namespace Logger {
enum LoggingLevel { LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG };

class Logger {
public:
private:
  LoggingLevel logLevel;
};
}; // namespace Logger
