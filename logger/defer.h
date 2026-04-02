
#pragma once
#include <functional>
namespace logger
{
    class ExecuteOnScopeExit
    {
    public:
        ExecuteOnScopeExit() = default;
        ExecuteOnScopeExit(ExecuteOnScopeExit &&) = default;
        ExecuteOnScopeExit &operator=(ExecuteOnScopeExit &&) = default;
        ExecuteOnScopeExit(const ExecuteOnScopeExit &) = delete;
        ExecuteOnScopeExit &operator=(const ExecuteOnScopeExit &) = delete;
        template <typename F, typename... Args>
        ExecuteOnScopeExit(F &&f, Args &&...args)
        {
            func_ = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        }
        ~ExecuteOnScopeExit() noexcept
        {
            if (func_)
            {
                func_();
            }
        }

    private:
        std::function<void()> func_;
    };
} // namespace logger
#define _LOG_CONCAT(a, b) a##b
#define _MAKE_DEFER_(line) logger::ExecuteOnScopeExit _LOG_CONCAT(defer, line) = [&]()
#undef LOG_DEFER
#define LOG_DEFER _MAKE_DEFER_(__LINE__)

/*
当编译器遇到 LOG_DEFER 时，会展开为 _MAKE_DEFER_(__LINE__)
_MAKE_DEFER_(__LINE__) 会展开为 logger::ExecuteOnScopeExit defer<行号> = [&]()
后面的代码块 { delete ptr; } 会成为 lambda 表达式的函数体
当 defer<行号> 变量离开作用域时，其析构函数会被调用
ExecuteOnScopeExit 的析构函数会执行存储的 lambda 表达式，从而自动执行 delete ptr
*/