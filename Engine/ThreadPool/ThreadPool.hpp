#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <future>

class ThreadPool;

struct Worker {
    unsigned id{};
    ThreadPool *owner{};
    std::deque<std::function<void()> > tasks;
    std::mutex mutex;
    std::condition_variable condition;
    std::thread thread;
};


class ThreadPool {
public:
    explicit ThreadPool(unsigned threadCount = std::thread::hardware_concurrency()) {
        if (!threadCount) {
            threadCount = 1;
        }

        this->stop = false;
        this->roundRobin = 0;

        this->workers.reserve(threadCount);
        for (unsigned i = 0; i < threadCount; ++i) {
            this->workers.emplace_back(std::make_unique<Worker>());
            auto &w = *this->workers.back();

            w.id = i;
            w.owner = this;
            w.thread = std::thread([this,i] { this->workerLoop(i); });
        }
    };

    ~ThreadPool() {
        this->stop.store(true, std::memory_order_relaxed);

        for (const auto &worker: this->workers) {
            std::lock_guard<std::mutex> lock(worker->mutex);
            worker->condition.notify_all();
        }
        for (const auto &worker: this->workers) {
            if (worker->thread.joinable()) {
                worker->thread.join();
            }
        }
    }

    unsigned size() const { return static_cast<unsigned>(this->workers.size()); }

    template<typename F, class... Args>
    auto submit(F &&f, Args &&... args) -> std::future<std::invoke_result_t<F, Args...> > {
        using return_type = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()> >(
            [fwdF = std::forward<F>(f), tup = std::make_tuple(std::forward<Args>(args)...)]() mutable -> return_type {
                return std::apply([&](auto &&... a)-> return_type {
                    return std::invoke(
                        std::move(fwdF), std::forward<decltype(a)>(a)...);
                }, std::move(tup));
            }
        );

        auto future = task->get_future();

        {
            const unsigned victim = this->roundRobin.fetch_add(1, std::memory_order_relaxed) % this->workers.size();
            std::lock_guard<std::mutex> lock(this->workers[victim]->mutex);
            this->workers[victim]->tasks.emplace_front([task] {
                (*task)();
            });
            this->workers[victim]->condition.notify_one();
        }

        return future;
    }


    bool helpOneRound() {
        const auto n = static_cast<unsigned>(this->workers.size());
        for (unsigned victim = 0; victim < n; ++victim) {
            std::function<void()> task;
            {
                std::lock_guard<std::mutex> lock(this->workers[victim]->mutex);
                if (!this->workers[victim]->tasks.empty()) {
                    task = std::move(this->workers[victim]->tasks.back());
                    this->workers[victim]->tasks.pop_back();
                }
            }

            if (task) {
                task();
                return true;
            }

            std::this_thread::yield();
            return false;
        }
    }

private:
    void workerLoop(const unsigned &id) {
        auto &worker = this->workers[id];

        std::mt19937_64 rng(id * 1469598103934665603ULL);

        while (!stop.load(std::memory_order_relaxed)) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(worker->mutex);
                if (!worker->tasks.empty()) {
                    task = std::move(worker->tasks.front());
                    worker->tasks.pop_front();
                }
            }
            if (task) {
                task();
                continue;
            }

            const auto n = static_cast<unsigned>(this->workers.size());
            const unsigned start = rng() % n;

            for (unsigned i = 0; i < n; ++i) {
                const unsigned vic = (start + i) % n;
                if (vic == id) continue;

                std::lock_guard<std::mutex> lock(this->workers[vic]->mutex);
                if (!this->workers[vic]->tasks.empty()) {
                    task = std::move(this->workers[vic]->tasks.back());
                    this->workers[vic]->tasks.pop_back();
                    break;
                }
            }

            if (task) {
                task();
                continue;
            }

            {
                std::unique_lock<std::mutex> lock(worker->mutex);
                worker->condition.wait_for(lock, std::chrono::milliseconds(1), [&] {
                    return !this->stop.load(std::memory_order_relaxed) || !worker->tasks.empty();
                });
            }
        }
    }

private:
    std::vector<std::unique_ptr<Worker> > workers;

    std::atomic<bool> stop;
    std::atomic<unsigned> roundRobin;
};
