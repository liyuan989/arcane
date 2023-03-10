#include <arcane/thread_utils.h>

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
    pid_t pid = GetTid();
    snprintf(detail::t_tid_buf, sizeof(detail::t_tid_buf), "%6d", static_cast<int>(pid));
    return detail::t_tid_buf;
}


} // namespace arcane

