#ifndef ARCANE_MULTI_FUTURE_H
#define ARCANE_MULTI_FUTURE_H

#include <vector>
#include <atomic>
#include <functional>
#include <utility>

#include <arcane/thread_pool.h>
#include <arcane/mutex.h>
#include <arcane/lock_guard.h>
#include <arcane/condition.h>

namespace arcane {

template<typename T, typename Queue = std::deque<ThreadPoolTask>>
class MultiFuture {
public:
    using Task = std::function<T ()>;

    MultiFuture(ThreadPool<Queue>& pool, const std::vector<Task>& tasks)
        : done_(false),
          pool_(pool),
          mutex_(),
          cond_(mutex_),
          finish_num_(0),
          result_(tasks.size()) {
        for (size_t i = 0; i < tasks.size(); ++i) {
            pool_.RunTask(std::bind(&MultiFuture::RunInThread, this, tasks[i], i));
        }
    }

    MultiFuture(const MultiFuture&) = delete;
    MultiFuture& operator=(const MultiFuture&) = delete;
    
    std::vector<T> Get() {
        LockGuard<Mutex> guard(mutex_);
        while (!done_) {
            cond_.Wait();
        }
        return result_;
    }

    std::pair<bool, std::vector<T>> Get(int64_t microseconds) {
        LockGuard<Mutex> guard(mutex_);
        while (!done_) {
            if (cond_.TimedWaitMicroseconds(microseconds)) {
                std::vector<T> tmp;
                return std::make_pair<bool, std::vector<T>>(false, tmp);
            }
        }
        return std::make_pair<bool, std::vector>(true, result_);
    }

private:
    void RunInThread(Task task, size_t index) {
        if (result_.empty()) {
            return;
        }
        result_[index] = task();
        ++finish_num_; 
        if (finish_num_.load() >= result_.size()) {
            LockGuard<Mutex> guard(mutex_);
            done_ = true;
            cond_.Notify();
        }
    }

    bool done_;
    ThreadPool<Queue>& pool_;
    Mutex mutex_;
    Condition cond_;
    std::atomic<size_t> finish_num_;
    std::vector<T> result_;
};

} // namespace arcane

#endif

