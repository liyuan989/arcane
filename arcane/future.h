#ifndef ARCANE_FUTURE_H
#define ARCANE_FUTURE_H

#include <stdint.h>
#include <functional>
#include <utility>

#include <arcane/thread_pool.h>
#include <arcane/mutex.h>
#include <arcane/lock_guard.h>
#include <arcane/condition.h>

namespace arcane {

template <typename T, typename Queue = std::deque<ThreadPoolTask>>
class Future {
public:
    using Task = std::function<T ()>;

    Future(ThreadPool<Queue>& pool, const Task& task)
        : done_(false),
          pool_(pool),
          mutex_(),
          cond_(mutex_) {
        pool_.RunTask(std::bind(&Future::RunInThread, this, task));
    }

    Future(const Future&) = delete;
    Future& operator=(const Future&) = delete;

    T Get() {
        LockGuard<Mutex> guard(mutex_);
        while (!done_) {
            cond_.Wait();
        }
        return result_;
    }

    std::pair<bool, T> Get(int64_t microseconds) {
        LockGuard<Mutex> guard(mutex_);
        while (!done_) {
            if (cond_.TimedWaitMicroseconds(microseconds)) {
                return std::make_pair<bool, std::vector<T>>(false, T());
            }
        }
        return std::make_pair<bool, T>(true, result_);
    }

private:
    void RunInThread(Task task) {
        T ret = task();
        LockGuard<Mutex> guard(mutex_);
        result_ = std::move(ret);
        done_ = true;
        cond_.Notify();
    }

    bool done_;
    ThreadPool<Queue>& pool_;
    Mutex mutex_;
    Condition cond_;
    T result_;
};

} // namespace arcane

#endif

