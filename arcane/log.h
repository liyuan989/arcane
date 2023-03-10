#ifndef ARCANE_LOG_H
#define ARCANE_LOG_H

#include <string>
#include <sstream>
#include <functional>
#include <atomic>

namespace arcane {
 
class LogPolicy {
public:
    static LogPolicy& GetInstance();

    void Unmute();
    void Mute();
    bool IsMute();

    LogPolicy(const LogPolicy&) = delete;
    LogPolicy& operator=(const LogPolicy&) = delete;

private:
    LogPolicy();

    std::atomic<bool> is_mute_;
};

class Log {
public:
    enum LogLevel {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
    };

    using OutputFunc = std::function<void (LogLevel,
                                           const std::string&,
                                           int,
                                           const std::string&)>;

    Log(LogLevel level, const std::string& filename, int line);
    ~Log();

    template <typename T>
    std::ostream& operator<<(const T& data) {
        return stream_ << data;
    }

    static void SetOutputFunc(OutputFunc func);
    static void SetLogLevel(LogLevel level);
    static void SetLogLevel(const std::string& str);
    static void SetTraceId(const std::string& id);
    static std::string GetTraceId();
    static LogLevel GetLogLevel();

private:
    LogLevel level_;
    const std::string& filename_;
    int line_;
    std::ostringstream stream_;

    static LogLevel global_level_;
};

#define LOG_TRACE \
if (edgedetect::Log::GetLogLevel() <= edgedetect::Log::TRACE) \
    edgedetect::Log(edgedetect::Log::TRACE, __FILE__, __LINE__)

#define LOG_DEBUG \
if (edgedetect::Log::GetLogLevel() <= edgedetect::Log::DEBUG) \
    edgedetect::Log(edgedetect::Log::DEBUG, __FILE__, __LINE__)

#define LOG_INFO \
if (edgedetect::Log::GetLogLevel() <= edgedetect::Log::INFO) \
    edgedetect::Log(edgedetect::Log::INFO, __FILE__, __LINE__)

#define LOG_WARN \
if (edgedetect::Log::GetLogLevel() <= edgedetect::Log::WARN) \
    edgedetect::Log(edgedetect::Log::WARN, __FILE__, __LINE__)

#define LOG_ERROR \
if (edgedetect::Log::GetLogLevel() <= edgedetect::Log::ERROR) \
    edgedetect::Log(edgedetect::Log::ERROR, __FILE__, __LINE__)

#define LOG_FATAL \
if (edgedetect::Log::GetLogLevel() <= edgedetect::Log::FATAL) \
    edgedetect::Log(edgedetect::Log::FATAL, __FILE__, __LINE__)

} // namespace arcane

#endif

