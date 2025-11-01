// test_utils.hpp
#pragma once
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>

struct CountDownLatch {
    explicit CountDownLatch(int n) : cnt(n) {}
    void count_down() {
        if (cnt.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            std::lock_guard<std::mutex> lk(mx);
            done = true;
            cv.notify_all();
        }
    }
    bool wait_for(std::chrono::milliseconds d) {
        std::unique_lock<std::mutex> lk(mx);
        return cv.wait_for(lk, d, [&]{ return done; });
    }
private:
    std::atomic<int> cnt;
    std::mutex mx; std::condition_variable cv; bool done=false;
};
