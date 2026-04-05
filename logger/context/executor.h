#pragma once

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "thread_pool.h"

namespace logger {
namespace context{
    using Task = std::function<void(void)>;
    using TaskRunnerTag = uint64_t; // 任务运行器标签
    using RepeatedTaskId = uint64_t; // 重复任务ID
    
    class Executor { // 执行器
        private:
            class ExecutorTimer // 执行器定时器
            {
            public:
                struct InternalState
                {                                                                           // 内部状态
                    std::chrono::time_point<std::chrono::high_resolution_clock> time_point; // 上次运行时间
                    Task task;                                                              // 任务
                    RepeatedTaskId repeated_id;                                             // 重复任务ID

                    bool operator<(const InternalState &other) const
                    {
                        return time_point > other.time_point;
                    }
                };

                ExecutorTimer();  // 构造函数
                ~ExecutorTimer(); // 析构函数

                ExecutorTimer &operator=(const ExecutorTimer &other) = delete; // 禁用赋值运算符
                ExecutorTimer(ExecutorTimer &&other) = delete;                 // 禁用移动构造函数

                bool Start(); // 启动定时器;
                void Stop();  // 停止定时器;

                void PostDelayedTask(Task task, const std::chrono::microseconds &delta); // 提交延迟任务
                void PostTask_(Task task,
                                        std::chrono::microseconds delta,
                                        RepeatedTaskId repeated_task_id,
                                        uint64_t repeat_num);
                RepeatedTaskId PostRepeatedTask(Task task, const std::chrono::microseconds &delta, uint64_t repeat_num); // 提交重复任务

                void CancelRepeatedTask(RepeatedTaskId task_id); // 取消重复任务
            
            private:
                void Run_(); // 运行任务

                void PostRepeatedTask_(Task task, const std::chrono::microseconds &delta, RepeatedTaskId repeated_task_id, uint64_t repeat_num); 

                RepeatedTaskId GetNextRepeatedTaskId_() { return repeated_task_id_++; }; // 获取下一个重复任务ID

            private:
                std::priority_queue<InternalState> queue_; // 任务队列
                std::mutex mutex_; // 互斥锁
                std::condition_variable cond_; // 条件变量
                std::atomic<bool> running_; // 是否运行运行
                std::unique_ptr<ThreadPool> thread_pool_;  // 线程池

                std::atomic<RepeatedTaskId> repeated_task_id_; // 下一个重复任务ID
                std::unordered_set<RepeatedTaskId> repeated_id_state_set_; // 重复任务ID集合
            };
        
            class ExecutorContext // 执行器上下文
            {
            public:
                ExecutorContext() = default; // 构造函数
                ~ExecutorContext() = default; // 析构函数

                ExecutorContext(ExecutorContext &&other) = default; // 移动构造函数
                ExecutorContext &operator=(ExecutorContext &&other) = default; // 移动赋值运算符

                TaskRunnerTag AddTaskRunner(const TaskRunnerTag &tag); // 添加任务运行器标签

            private:
                using TaskRunner = ThreadPool; // 任务运行器
                using TaskRunnerPtr = std::unique_ptr<TaskRunner>; // 任务运行器指针
                friend class Executor;

            private:
                TaskRunner* GetTaskRunner(const TaskRunnerTag &tag);
                TaskRunnerTag GetNextRunnerTag(); 
            private:
                std::unordered_map<TaskRunnerTag, TaskRunnerPtr> task_runner_dict_; // 任务运行器字典
                std::mutex mutex_; // 互斥锁
            };
    
        public:
            Executor();
            ~Executor();
            Executor(const Executor& other) = delete;
            Executor& operator=(const Executor& other) = delete;

            TaskRunnerTag AddTaskRunner(const TaskRunnerTag &tag); // 添加任务运行器标签
            void PostTask(const TaskRunnerTag &runner_tag, Task task); // 提交任务

            template <typename R, typename P>
            void PostDelayedTask(const TaskRunnerTag &runner_tag, Task task, const std::chrono::duration<R, P> &delta) // 提交延迟任务
            {
                Task func = std::bind(&Executor::PostTask, this, runner_tag, std::move(task));

                executor_timer_->Start(); // 启动定时器
                executor_timer_->PostDelayedTask(std::move(func), std::chrono::duration_cast<std::chrono::microseconds>(delta)); // 提交延迟任务
            }

            template <typename R, typename P>
            RepeatedTaskId PostRepeatedTask(const TaskRunnerTag &runner_tag,
                                            Task task,
                                            const std::chrono::duration<R, P> &delta,
                                            uint64_t repeat_num) // 提交重复任务
            {
                Task func = std::bind(&Executor::PostTask, this, runner_tag, std::move(task));

                executor_timer_->Start(); // 启动定时器

                return executor_timer_->PostRepeatedTask(std::move(func),
                                                         std::chrono::duration_cast<std::chrono::microseconds>(delta), repeat_num); // 提交重复任务
            }
             void CancelRepeatedTask(RepeatedTaskId task_id) { executor_timer_->CancelRepeatedTask(task_id); }

             template <typename F, typename... Args>
             auto PostTaskAndGetResult(const TaskRunnerTag &runner_tag,
                                       F &&f,
                                       Args &&...args) -> std::shared_ptr<std::future<std::result_of_t<F(Args...)>>> // 提交任务并获取结果
             {
                 ExecutorContext::TaskRunner *task_runner = executor_context_->GetTaskRunner(runner_tag); // 获取任务运行器
                 auto ret = task_runner->RunRetTask(std::forward<F>(f), std::forward<Args>(args)...); // 提交任务并获取结果
                 return ret;
             }

         private:
             std::unique_ptr<ExecutorContext> executor_context_; // 执行器上下文
             std::unique_ptr<ExecutorTimer> executor_timer_; // 执行器定时器 
    };

} // namespace context
} // namespace logger
