#pragma once 

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace logger {
namespace context {

    class ThreadPool {      
        public:
            explicit ThreadPool(int thread_count)
            {
                this->is_shutdown_.store(false);
                this->is_available_.store(false);
                this->thread_count_.store(thread_count);
            }

            ThreadPool(const ThreadPool& other) = delete;
            ThreadPool& operator=(const ThreadPool other) = delete;

            ~ThreadPool() {
                Stop();
            }

            void Stop();
            bool Start();
        
            template <typename F, typename... Args> 
            void RunTask(F&& f, Args&&... args) {
                if (this->is_shutdown_.load() || !this->is_available_.load()) {
                    return;
                }
                auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                {
                    std::lock_guard<std::mutex> lock(this->task_mutex_);
                    this->tasks_.emplace([task]() {(*task)();});
                }
                this->task_cv_.notify_all();    // 通知所有线程有新任务
            }
                
        template <typename F, typename... Args> 
        auto RunRetTask(F&& f, Args&&... args) -> std::shared_ptr<std::future<std::result_of_t<F(Args...)>>> { // 返回任务执行结果的future
            if (!this->is_available_.load() || this->is_shutdown_.load()) {
                return nullptr;
            }
            using return_type = std::result_of_t<F(Args...)>; // 任务执行结果类型
            auto task = 
                        std::make_shared<std::packaged_task<return_type()>>
                        (std::bind(std::forward<F>(f), std::forward<Args>(args)...)); // 创建任务
            std::future<return_type> res = task->get_future()
            {
                std::lock_guard<std::mutex> lock(this->task_mutex_); // 加锁保护任务队列
                this->tasks_.emplace([task]() {(*task)();}); // 将任务添加到队列
            }
            this->task_cv_.notify_all();    // 通知所有线程有新任务
            return std::make_shared<std::future<std::result_of_t<F(Args...)>>>(std::move(res)); // 返回任务执行结果的future
        }


        

        private:
            void AddThread();



            using ThreadPtr = std::shared_ptr<std::thread>;    
            using Task = std::function<void()>;

            struct ThreadInfo { // 线程信息结构体
                ThreadInfo() = default;
                ~ThreadInfo();

                ThreadPtr ptr{nullptr}; // 线程指针
            };

            using ThreadInfoPtr = std::shared_ptr<ThreadInfo>; // 线程信息指针


            std::vector<ThreadInfoPtr> worker_threads_; // 线程信息向量
            std::queue<Task> tasks_; // 任务队列
            std::mutex task_mutex_; // 任务队列互斥锁


            std::condition_variable task_cv_; // 任务队列条件变量
            
            std::atomic<uint32_t> thread_count_; // 线程数量
            std::atomic<bool> is_shutdown_;     // 是否关闭线程池
            std::atomic<bool> is_available_;   // 是否有线程可用    

    };



} // namespace context
} // namespace logger
