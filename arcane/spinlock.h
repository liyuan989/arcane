#ifndef ARCANE_SPINLOCK_H
#define ARCANE_SPINLOCK_H

#include <stdint.h>
#include <pthread.h>

#include <arcane/thread_utils.h>

namespace arcane {

class SpinLock {
public:
    SpinLock()
        : holder_(0) {
        pthread_spin_init(&spin_, PTHREAD_PROCESS_PRIVATE);
    }

    ~SpinLock() {
        pthread_spin_destroy(&spin_);
    }

    SpinLock(const SpinLock&) = delete;
    SpinLock& operator=(const SpinLock&) = delete;

    void Lock() {
        pthread_spin_lock(&spin_);
        SetHolder();
    }

    void Unlock() {
        ResetHolder();
        pthread_spin_unlock(&spin_);
    }

    bool IsLocked() const {
        return !(holder_ == 0);
    }

    bool IsLockedByCurrentThread() const {
        return holder_ == GetTid();
    }

private:
    void SetHolder() {
        holder_ = GetTid();
    } 

    void ResetHolder() {
        holder_ = 0;
    }

    pthread_spinlock_t spin_;
    int64_t holder_;
};

} // namespace arcane

#endif

