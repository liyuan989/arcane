#ifndef ARCANE_RWLOCK_H
#define ARCANE_RWLOCK_H

#include <pthread.h>

namespace arcane {

class RWLock {
public:
    RWLock() {
        pthread_rwlock_init(&rwlock_, nullptr);
    }

    ~RWLock() {
        pthread_rwlock_destroy(&rwlock_);
    }

    RWLock(const RWLock&) = delete;
    RWLock& operator=(const RWLock&) = delete;

    void ReadLock() {
        pthread_rwlock_rdlock(&rwlock_);
    }

    void WriteLock() {
        pthread_rwlock_wrlock(&rwlock_);
    }

    void Unlock() {
        pthread_rwlock_unlock(&rwlock_);
    }

private:
    pthread_rwlock_t rwlock_;
};

} // namespace arcane

#endif

