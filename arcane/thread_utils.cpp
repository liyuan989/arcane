#include <arcane/thread_utils.h>

#include <inttypes.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace arcane {

namespace detail {

thread_local pid_t t_tid = 0;
thread_local char t_tid_buf[64]; 

} // namespace detail

int64_t GetTid() {
    if (detail::t_tid == 0) {
        detail::t_tid = static_cast<pid_t>(syscall(SYS_gettid));
    }
    return static_cast<int64_t>(detail::t_tid);
}

std::string GetTidString() {
    int64_t pid = GetTid();
    snprintf(detail::t_tid_buf, sizeof(detail::t_tid_buf), "%6" PRId64, pid);
    return detail::t_tid_buf;
}

} // namespace arcane

