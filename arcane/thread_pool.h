#ifndef ARCANE_THREAD_POOL_H
#define ARCANE_THREAD_POOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <thread>
#include <memory>
#include <exception>

#include <arcane/mutex.h>
#include <arcane/condition.h>
#include <arcane/lock_guard.h>
#include <arcane/log.h>

namespace arcane {

using ThreadPoolTask = std::function<void ()>;

template <typename Queue = std::deque<ThreadPoolTask>>
class ThreadPool {
public:
    using Task = ThreadPoolTask;

    ThreadPool(size_t num_threads, size_t max_queue_size = 0)
        : running_(false),
          mutex_(),
          not_empty_(mutex_),
          not_full_(mutex_),
          num_threads_(num_threads),
          max_queue_size_(max_queue_size) {
    }

    ~ThreadPool() {
        if (running_) {
            stop();       
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    void start() {
        running_ = true;
        threads_.reserve(num_threads_);
        for (size_t i = 0; i < num_threads_; ++i) {
            std::shared_ptr<std::thread> p(new std::thread(&ThreadPool::RunInThread, this));
            threads_.push_back(p);
        }
        if (threads_.empty() && thread_init_callback_) {
            thread_init_callback_(); 
        }
    }

    void stop() {
        {
            LockGuard<Mutex> guard(mutex_);
            running_ = false;
            not_empty_.NotifyAll();
            not_full_.NotifyAll();
        }
        for (auto thread : threads_) {
            thread->join();
        }
    }

    void RunTask(const Task& task) {
        if (threads_.empty()) {
            task();
        } else {
            LockGuard<Mutex> guard(mutex_);
            while (IsFull() && running_) {
                not_full_.Wait();
            }
            if (!running_) {
                return;
            }
            queue_.push_back(task);
            not_empty_.Notify();
        }
    }

    void SetThreadInitCallback(const Task& cb) {
        thread_init_callback_ = cb;
    }

    size_t QueueSize() const {
        LockGuard<Mutex> guard(mutex_);
        return queue_.size();
    }

private:
    void RunInThread() {
        try {
            if (thread_init_callback_) {
                thread_init_callback_();
            }
            while (running_) {
                Task task = Take();
                if (task) {
                    task();
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR << "exception caught in ThreadPool, reason: " << e.what();            
            abort();
        } catch (...) {
            LOG_ERROR << "unknown exception caught in ThreadPool";
            throw;
        }
    }

    Task Take() {
        LockGuard<Mutex> guard(mutex_);
        while (queue_.empty() && running_) {
            not_empty_.Wait();
        }
        Task task;
        if (!queue_.empty()) {
            task = queue_.front();
            queue_.pop_front();
            if (max_queue_size_ > 0) {
                not_full_.Notify();
            }
        }
        return task;
    }

    bool IsFull() const {
        return max_queue_size_ > 0 && queue_.size() >= max_queue_size_; 
    }

    bool running_;
    mutable Mutex mutex_;
    Condition not_empty_;
    Condition not_full_;
    Task thread_init_callback_;
    size_t num_threads_;
    std::vector<std::shared_ptr<std::thread>> threads_;
    size_t max_queue_size_;
    Queue queue_;
};

} // namespace arcane

#endif

