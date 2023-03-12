#include <utility>

#include <arcane/log.h>
#include <arcane/future.h>
#include <arcane/multi_future.h>
#include <arcane/thread_utils.h>

int accumulate(int low, int high) {
    LOG_INFO << "tid:" << arcane::GetTid();
    int v = 0;
    for (int i = low; i <= high; ++i) {
        v += i;
    }
    return v;
}

void test() {
    arcane::ThreadPool<> pool(4); 
    pool.start();

    arcane::Future<int> future(pool, std::bind(accumulate, 1, 100));
    int v = future.Get();
    LOG_INFO << v;

    std::vector<arcane::MultiFuture<int>::Task> tasks;
    for (int i = 0; i < 10; ++i) {
        tasks.push_back(std::bind(accumulate, 1, 100 * i));
    }
    arcane::MultiFuture<int> multi_future(pool, tasks);
    std::vector<int> values = multi_future.Get();
    for (int value : values) {
        LOG_INFO << value;
    }
}

int main () {
    arcane::LogPolicy::GetInstance().Unmute();
    LOG_INFO << "test start...";
    test();
    LOG_INFO << "test end...";
    return 0;
}
