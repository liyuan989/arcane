#ifndef ARCANE_MUTEX_H
#define ARCANE_MUTEX_H

#include <stdint.h>
#include <pthread.h>

namespace arcane {

class Mutex {
public:
    Mutex() 
        : holder_(0) {
        pthread_mutex_init(&mutex_, nullptr);
    }

    ~Mutex() {
        pthread_mutex_destroy(&mutex_);
    }

    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    void Lock() {
        pthread_mutex_lock(&mutex_);
        SetHolder();
    }

    bool TryLock() {
        if (pthread_mutex_trylock(&mutex_) == 0) {
            SetHolder();
            return true;
        }
        return false;
    }

    void Unlock() {
        ResetHolder();
        pthread_mutex_unlock(&mutex_);
    }

    pthread_mutex_t* GetPthreadMutex() {
        return &mutex_;
    }

    bool IsLocked() const {
        return !(holder_ == 0);
    }

    bool IsLockedByCurrentThread() const {
        return holder_ == GetTid();
    }

private:
    friend class Condition;
    
    class ConditionGuard {
    public:
        ConditionGuard(Mutex& mutex) 
            : cond_mutex_(mutex) {
            cond_mutex_.ResetHolder();
        }

        ~ConditionGuard() {
            cond_mutex_.SetHolder();
        }

        ConditionGuard(const ConditionGuard&) = delete;
        ConditionGuard& operator=(const ConditionGuard&) = delete;

    private:
        Mutex& cond_mutex_;
    };

    void SetHolder() {
        holder_ = GetTid();
    }

    void ResetHolder() {
        holder_ = 0;
    }

    pthread_mutex_t mutex_;
    int64_t holder_
};

} // namespace arcane

#endif
