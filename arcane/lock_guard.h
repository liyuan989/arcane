#ifndef ARCANE_LOCK_GUARD_H
#define ARCANE_LOCK_GUARD_H

#include <arcane/rwlock.h>

namespace arcane {

template<typename Lock>
class LockGuard {
public:
    LockGuard(Lock& lock)
        : lock_(lock) {
        lock_.Lock();
    }

    ~LockGuard() {
        lock_.Unlock();
    }

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    Lock& lock_;
};

class ReadLockGuard {
public:
    ReadLockGuard(RWLock& lock)
        : lock_(lock) {
        lock_.ReadLock();
    }

    ~ReadLockGuard() {
        lock_.Unlock();
    }

    ReadLockGuard(const ReadLockGuard&) = delete;
    ReadLockGuard& operator=(const ReadLockGuard&) = delete;

private:
    RWLock& lock_;    
};

class WriteLockGuard {
public:
    WriteLockGuard(RWLock& lock)
        : lock_(lock) {
        lock_.WriteLock();
    }

    ~WriteLockGuard() {
        lock_.Unlock();
    }

    WriteLockGuard(const WriteLockGuard&) = delete;
    WriteLockGuard& operator=(const WriteLockGuard&) = delete;

private:
    RWLock& lock_;
};

} // namespace arcane

#endif

