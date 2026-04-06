#pragma once

#include "formatter.h"

namespace logger
{

    class EffectiveFormatter : public Formatter // 有效格式化器
    {
    public:
        void Format(const LogMsg &msg, MemoryBuf *dest) override; // 格式化日志消息
    };

} // namespace logger
