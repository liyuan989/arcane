#ifndef ARCANE_CONDITION_H
#define ARCANE_CONDITION_H

#include <stdint.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#include <arcane/mutex.h>

namespace arcane {

class Condition {
public:
    explicit Condition(Mutex& mutex)
        : mutex_(mutex) {
        pthread_cond_init(&cond_, nullptr);
    }

    ~Condition() {
         pthread_cond_destroy(&cond_);
    }

    Condition(const Condition&) = delete;
    Condition& operator=(const Condition&) = delete;

    void Wait() {
        Mutex::ConditionGuard guard(mutex_); 
        pthread_cond_wait(&cond_, mutex_.GetPthreadMutex());
    }

    // returns true if time out, false otherwise.
    bool TimedWaitMicroseconds(int64_t microseconds) {
        struct timespec t;
        clock_gettime(CLOCK_REALTIME, &t);

        constexpr const int64_t kNanoSecondsPerSecond = 1e9;
        int64_t nanoseconds = microseconds * 1000 + t.tv_nsec;
        t.tv_sec += static_cast<time_t>(nanoseconds / kNanoSecondsPerSecond);
        t.tv_nsec += static_cast<long>(nanoseconds % kNanoSecondsPerSecond);

        Mutex::ConditionGuard guard(mutex_);
        return pthread_cond_timedwait(&cond_, mutex_.GetPthreadMutex(), &t) == ETIMEDOUT;
    }

    // returns true if time out, false otherwise.
    bool TimedWaitSeconds(int seconds) {
        constexpr const int64_t kMicrosecondsPerSecond = 1e6;
        int microseconds = seconds * kMicrosecondsPerSecond;
        return TimedWaitMicroseconds(microseconds); 
    }

private:
    Mutex& mutex_;
    pthread_cond_t cond_;
};

} // namespace arcane

#endif

