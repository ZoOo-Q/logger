#include "executor.h"

namespace logger {
namespace context {

Executor::Executor() { // 构造函数 初始化
    executor_context_ = std::make_unique<ExecutorContext>(); // 创建执行器上下文
    executor_timer_ = std::make_unique<ExecutorTimer>(); // 创建执行器定时器
}

Executor::~Executor() { // 析构函数 释放资源
    executor_context_.reset(); // 释放执行器上下文
    executor_timer_.reset(); // 释放执行器定时器    
}

TaskRunnerTag Executor::AddTaskRunner(const TaskRunnerTag &tag) // 添加任务运行器标签
{
    return executor_context_->AddTaskRunner(tag); // 添加任务运行器标签
}

void Executor::PostTask(const TaskRunnerTag &runner_tag, Task task) // 提交任务
{
    auto task_runner = executor_context_->GetTaskRunner(runner_tag); // 获取任务运行器
    if (task_runner == nullptr) {
        return; // 任务运行器不存在
    }
    task_runner->RunTask(std::move(task)); // 提交任务
}

TaskRunnerTag Executor::ExecutorContext::AddTaskRunner(const TaskRunnerTag& tag) { // 添加任务运行器标签
    std::lock_guard<std::mutex> lock(mutex_); // 加锁保护任务运行器映射
    TaskRunnerTag latest_tag = tag; //  最新任务运行器标签
    while (task_runner_dict_.find(latest_tag) != task_runner_dict_.end()) {
        latest_tag = GetNextRunnerTag(); // 任务运行器标签已存在，递增
    }
    TaskRunnerPtr runner = std::make_unique<ThreadPool>(1);
    runner->Start(); // 启动线程池
    task_runner_dict_.emplace(latest_tag, std::move(runner)); // 添加任务运行器到映射
    return latest_tag; // 返回最新任务运行器标签
}

TaskRunnerTag Executor::ExecutorContext::GetNextRunnerTag()
{
    static uint64_t index = 0;
    ++index;
    return index;
}

Executor::ExecutorContext::TaskRunner *Executor::ExecutorContext::GetTaskRunner(const TaskRunnerTag &tag) // 获取任务运行器
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (task_runner_dict_.find(tag) == task_runner_dict_.end())
    {
        return nullptr;
    }
    return task_runner_dict_[tag].get();
}

Executor::ExecutorTimer::ExecutorTimer()
{
    thread_pool_ = std::make_unique<ThreadPool>(1);
    repeated_task_id_.store(0);
    running_.store(false);
}

Executor::ExecutorTimer::~ExecutorTimer()
{
    Stop();
}


bool Executor::ExecutorTimer::Start() // 启动定时器
{
    if (running_) {
        return true;
    }
    running_.store(true);
    bool rst = thread_pool_->Start(); // 启动线程池
    thread_pool_->RunTask(&Executor::ExecutorTimer::Run_, this); // 提交运行任务
    return rst;
}

void Executor::ExecutorTimer::Stop() {
    running_.store(false);
    cond_.notify_all();
    thread_pool_.reset();
}

void Executor::ExecutorTimer::Run_() {
    while (running_.load()) {
        // 运行任务
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty()) { // 任务队列为空，等待任务
            cond_.wait(lock); // 等待任务
            continue; // 继续等待任务
        }
        auto s = queue_.top(); // 获取任务队列顶部任务
        auto diff = s.time_point - std::chrono::high_resolution_clock::now(); // 计算任务延迟时间
        if (std::chrono::duration_cast<std::chrono::microseconds>(diff).count() > 0) { // 任务延迟时间大于0，等待任务延迟时间
            cond_.wait(lock, diff); // 等待任务延迟时间
            continue; // 继续等待任务
        } else {
            queue_.pop(); // 从任务队列中移除任务
            lock.unlock();
            s.task(); // 运行任务
        }
    }
}

void Executor::ExecutorTimer::PostDelayedTask(Task task, const std::chrono::microseconds& delta) { // 提交延迟任务
    InternalState s; // 内部状态
    s.time_point = std::chrono::high_resolution_clock::now() + delta; // 设置任务延迟时间
    s.task = std::move(task); // 移动任务到内部状态
    {
        std::unique_lock<std::mutex> lock(mutex_); // 加锁保护任务队列
        queue_.push(s); // 将任务状态压入任务队列
        cond_.notify_all();     // 通知所有线程
    }
}

RepeatedTaskId Executor::ExecutorTimer::PostRepeatedTask(Task task,
                                                         const std::chrono::microseconds& delta,
                                                         uint64_t repeat_num) { // 提交重复任务
    RepeatedTaskId id = GetNextRepeatedTaskId_(); // 获取下一个重复任务ID
    repeated_id_state_set_.insert(id); // 插入重复任务ID到集合
    PostRepeatedTask_(std::move(task), delta, id, repeat_num); // 提交重复任务
    return id;
}

void Executor::ExecutorTimer::CancelRepeatedTask(RepeatedTaskId task_id) {
    repeated_id_state_set_.erase(task_id);
}

void Executor::ExecutorTimer::PostTask_(Task task,
                                        std::chrono::microseconds delta,
                                        RepeatedTaskId repeated_task_id,
                                        uint64_t repeat_num) {
    PostRepeatedTask_(std::move(task), delta, repeated_task_id, repeat_num);
}

void Executor::ExecutorTimer::PostRepeatedTask_(Task task,
                                                const std::chrono::microseconds &delta,
                                                RepeatedTaskId repeated_task_id,
                                                uint64_t repeat_num)
{
    if (repeated_id_state_set_.find(repeated_task_id) == repeated_id_state_set_.end() || repeat_num == 0)
    {
        return;
    }
    task();

    Task func =
        std::bind(&Executor::ExecutorTimer::PostTask_, this, std::move(task), delta, repeated_task_id, repeat_num - 1);
    InternalState s;
    s.time_point = std::chrono::high_resolution_clock::now() + delta;
    s.repeated_id = repeated_task_id;
    s.task = std::move(func);

    {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.push(s);
        lock.unlock();
        cond_.notify_all();
    }
}
}
}