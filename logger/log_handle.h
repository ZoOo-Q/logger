#pragma once
#include <atomic>
#include <cstdint>
#include <initializer_list> // 用于初始化日志句柄
#include <memory>
#include <string>

#include <vector>

#include "log_common.h"
#include "log_msg.h"

namespace logger
{

class LogSink; // 日志接收器
using LogSinkPtr = std::shared_ptr<LogSink>;
using LogSinkPtrList = std::initializer_list<LogSinkPtr>; // 日志接收器指针列表，用于初始化日志句柄
class LogHandle { // 日志句柄
    public:
        explicit LogHandle(LogSinkPtrList sinks);

        explicit LogHandle(LogSinkPtr sink);

        template <typename It>
        LogHandle(It begin, It end) : LogHandle(LogSinkPtrList(begin, end)) {}

        ~LogHandle() = default;

        LogHandle(const LogHandle &other) = delete;
        LogHandle &operator=(const LogHandle &other) = delete;

        void SetLevel(LogLevel lvl);

        LogLevel GetLevel() const;

        void log(LogLevel lvl, SourceLocation loc, StringView msg);

    protected:  
        bool ShouldLog(LogLevel lvl) const noexcept { return lvl >= level_ && !sinks_.empty(); }; // 判断是否应该记录日志

        void Log_(const LogMsg& msg); // 记录日志

    private:
        std::atomic<LogLevel> level_;
        std::vector<LogSinkPtr> sinks_;

};



}// namespace logger
