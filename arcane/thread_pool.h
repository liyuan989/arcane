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

template<typename Lock = Mutex, typename Queue = std::deque<ThreadPoolTask>>
class ThreadPool {
public:
    using Task = ThreadPoolTask;

    ThreadPool()
        : running_(false),
          cond_(lock_) {
    }

    ~ThreadPool() {
        if (running_) {
            stop();       
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    void start(int num_threads) {
        running_ = true;
        threads_.reserve(num_threads);
        for (int i = 0; i < num_threads; ++i) {
            std::shared_ptr<std::thread> p(new std::thread(&ThreadPool::RunInThread, this));
            threads_.push_back(p);
        }
    }

    void stop() {
        {
            LockGuard<Lock> guard(lock_);
            running_ = false;
            cond_.NotifyAll();
        }
        for (auto thread : threads_) {
            thread->join();
        }
    }

    void RunTask(const Task& task) {
        if (threads_.empty()) {
            task();
        } else {
            LockGuard<Lock> guard(lock_);
            queue_.push_back(task);
            cond_.Notify();
        }
    }

private:
    void RunInThread() {
        try {
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
        LockGuard<Lock> guard(lock_);
        while (queue_.empty() && running_) {
            cond_.Wait();
        }
        Task task;
        if (!queue_.empty()) {
            task = queue_.front();
            queue_.pop_front();
        }
        return task;
    }

    bool running_;
    Lock lock_;
    Condition cond_;
    Queue queue_;
    std::vector<std::shared_ptr<std::thread>> threads_;
};

} // namespace arcane

#endif

