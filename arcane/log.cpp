#include <arcane/log.h>

#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <sys/time.h>

#include <arcane/thread_utils.h>

namespace arcane {

namespace detail {

thread_local char t_time_buf[64];
thread_local std::string trace_id;

std::string GetLocalTime() {
    struct timeval time_val;
    gettimeofday(&time_val, NULL);
    struct tm tm_time;
    localtime_r(&time_val.tv_sec, &tm_time);
    snprintf(t_time_buf, sizeof(t_time_buf), 
             "%4d-%02d-%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
             static_cast<int>(time_val.tv_usec));
    return t_time_buf;
}

std::string ExtractFileName(const std::string& path) {
   size_t pos = path.find_last_of('/');
   return path.substr(pos + 1);
}

void DefaultOutput(Log::LogLevel level, 
                   const std::string& filename, 
                   int line, 
                   const std::string& msg) {
    std::ostringstream stream;
    stream << GetLocalTime() << " " << GetTidString() << " ";
    switch (level) {
        case Log::TRACE:
            stream << "[TRACE] ";
            break;
        case Log::DEBUG:
            stream << "[DEBUG] ";
            break;
        case Log::INFO:
            stream << "[INFO] ";
            break;
        case Log::WARN:
            stream << "[WARN] ";
            break;
        case Log::ERROR:
            stream << "[ERROR] ";
            break;
        case Log::FATAL:
            stream << "[FATAL] ";
            break;
        default:
            assert(0);
    }
    if (!trace_id.empty()) {
        stream << "[traceid:" << trace_id << "] ";
    }
    stream << msg << " - " << ExtractFileName(filename) << ':' << line;
    std::cout << stream.str() << std::endl;
    if (level == Log::FATAL) {
        abort();
    }
}

}  // namespace detail

LogPolicy::LogPolicy()
        : is_mute_(true) {
}

void LogPolicy::Unmute() {
    is_mute_.store(false);
}

void LogPolicy::Mute() {
    is_mute_.store(true);
}

bool LogPolicy::IsMute() {
    return is_mute_.load();
}

LogPolicy& LogPolicy::GetInstance() {
    static LogPolicy instance;
    return instance;
}

Log::OutputFunc g_OutputFunc = detail::DefaultOutput;

Log::LogLevel Log::global_level_ = Log::INFO;

Log::Log(Log::LogLevel level, const std::string& filename, int line)
    : level_(level),
      filename_(filename),
      line_(line) {
}

Log::~Log() {
    if (!LogPolicy::GetInstance().IsMute()) {
        g_OutputFunc(level_, filename_, line_, stream_.str());
    }
}

void Log::SetOutputFunc(Log::OutputFunc func) {
    g_OutputFunc = func;
}

void Log::SetLogLevel(Log::LogLevel level) {
    global_level_ = level;
}

void Log::SetLogLevel(const std::string& str) {
    if (str == "trace") {
        global_level_ = TRACE;
    } else if (str == "debug") {
        global_level_ = DEBUG;
    } else if (str == "info") {
        global_level_ = INFO;
    } else if (str == "warn") {
        global_level_ = WARN;
    } else if (str == "error") {
        global_level_ = ERROR;
    } else if (str == "fatal") {
        global_level_ = FATAL;
    } else {
        global_level_ = INFO;
        std::cout << "Unknown log level, use [INFO] level default" << std::endl;
    }
}

void Log::SetTraceId(const std::string& id) {
    detail::trace_id = id;
}

std::string Log::GetTraceId() {
    return detail::trace_id;
}

Log::LogLevel Log::GetLogLevel() {
    return global_level_;
}

} // namespace arcane

