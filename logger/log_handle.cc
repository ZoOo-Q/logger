#include "log_handle.h"

#include "formatter/formatter.h"
#include "sinks/sink.h"
namespace logger
{
    LogHandle::LogHandle(LogSinkPtrList sinks) : level_(LogLevel::kInfo)
    {
        for (auto &sink : sinks)
        {
            sinks_.push_back(std::move(sink));
        }
    }

    LogHandle::LogHandle(LogSinkPtr sink) : level_{LogLevel::kInfo}
    {
        sinks_.push_back(std::move(sink));
    }

    void LogHandle::SetLevel(LogLevel lvl) { level_ = lvl; }

    LogLevel LogHandle::GetLevel() const { return level_; }

    void LogHandle::log(LogLevel lvl, SourceLocation loc, StringView msg)
    {
        if (!ShouldLog(lvl))
        {
            return;
        }
        LogMsg msg_(loc, lvl, msg);
        Log_(msg_);
    }
    void LogHandle::Log_(const LogMsg &log_msg)
    {
        for (auto &sink : sinks_)
        {
            sink->Log(log_msg);
        }
    }

}
