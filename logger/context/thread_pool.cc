#include "thread_pool.h"

namespace logger {
namespace context {

    ThreadPool::ThreadInfo::~ThreadInfo() {
        if (ptr && ptr->joinable()) { // 线程可加入
            ptr->join(); // 等待线程执行完成
        }
    }

    void ThreadPool::AddThread() {
        ThreadInfoPtr thread_ptr = std::make_shared<ThreadInfo>(); // 创建线程信息指针
        auto func = [this]() {
            for (;;) {
                Task task;
                {
                    std::unique_lock<std::mutex> lock(task_mutex_); // 加锁

                    this->task_cv_.wait(lock, [this]
                             { return this->is_shutdown_ || (!this->tasks_.empty()); }); // 等待任务或关闭

                    if (this->is_shutdown_) // 关闭
                    {
                        break;
                    }

                    if (this->tasks_.empty()) // 任务队列为空
                    {
                        continue;
                    }

                    task = std::move(this->tasks_.front()); // 移动任务
                    this->tasks_.pop(); // 弹出任务
                }
                task(); // 执行任务
            }
        };

        thread_ptr->ptr = std::make_shared<std::thread>(std::move(func)); // 创建线程指针
        this->worker_threads_.push_back(std::move(thread_ptr)); // 移动线程信息指针

    }
    void ThreadPool::Stop()
    {
        if (is_available_.load())
        {
            is_shutdown_.store(true);
            task_cv_.notify_all();
            is_available_.store(false);
        }
        this->worker_threads_.clear();
    }

    bool ThreadPool::Start()
    {
        if (this->is_available_.load())
        {
            return false;
        }
        this->is_available_.store(true);
        uint32_t thread_count = thread_count_.load();
        for (uint32_t i = 0; i < thread_count; ++i)
        {
            AddThread();
        }
        return true;
    }

}   // namespace logger
}   // namespace context
