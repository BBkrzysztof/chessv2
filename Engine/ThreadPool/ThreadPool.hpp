#pragma once
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

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
    explicit ThreadPool(unsigned threadCount = std::thread::hardware_concurrency())
        : stop(false), roundRobin(0) {
        if (!threadCount) threadCount = 1;
        workers.reserve(threadCount);
        for (unsigned i = 0; i < threadCount; ++i) {
            workers.emplace_back(std::make_unique<Worker>());
            auto &w = *workers.back();
            w.id = i;
            w.owner = this;
            w.thread = std::thread([this, i] { this->workerLoop(i); });
        }
    }

    ~ThreadPool() {
        // sygnalizuj zamknięcie bez trzymania żadnych locków
        stop.store(true, std::memory_order_release);
        for (auto &w: workers) {
            w->condition.notify_all();
        }
        // join bez locków — bezpiecznie
        for (auto &w: workers) {
            if (w->thread.joinable()) w->thread.join();
        }
    }

    [[nodiscard]] unsigned size() const noexcept {
        return static_cast<unsigned>(workers.size());
    }

    // Zawsze wzywaj notify_one po dodaniu zadania
    template<class F>
    void submit(F &&f) {
        auto idx = roundRobin.fetch_add(1, std::memory_order_relaxed) % workers.size();
        auto &w = *workers[idx];
        {
            std::lock_guard<std::mutex> lk(w.mutex);
            w.tasks.emplace_back(std::forward<F>(f));
        }
        w.condition.notify_one();
    }

    bool helpOneRound() {
        std::function<void()> job;

        const auto n = workers.size();
        if (!n) return false;

        for (size_t k = 0; k < n; ++k) {
            auto &w = *workers[(roundRobin.fetch_add(1, std::memory_order_relaxed)) % n];
            if (w.mutex.try_lock()) {
                if (!w.tasks.empty()) {
                    job = std::move(w.tasks.back());
                    w.tasks.pop_back();
                    w.mutex.unlock();
                    job();
                    return true;
                }
                w.mutex.unlock();
            }
        }
        return false;
    }

private:
    friend struct Worker;

    void workerLoop(unsigned self) {
        auto &me = *workers[self];

        // losowa kolejność ofiar do kradzieży (prosty RNG na wątku)
        std::mt19937_64 rng(self * 1469598103934665603ULL + 0x9e3779b97f4a7c15ULL);
        std::uniform_int_distribution<unsigned> dist(0, size() ? (size() - 1) : 0);

        std::function<void()> job;

        for (;;) {
            {
                std::unique_lock<std::mutex> lk(me.mutex);
                me.condition.wait(lk, [&] {
                    return stop.load(std::memory_order_acquire) || !me.tasks.empty();
                });

                if (stop.load(std::memory_order_relaxed) && me.tasks.empty()) {
                    break; // porządne wyjście
                }

                if (!me.tasks.empty()) {
                    job = std::move(me.tasks.front());
                    me.tasks.pop_front();
                } else {
                    job = nullptr;
                }
            }

            if (job) {
                job();
                job = nullptr;
                continue;
            }

            if (size() > 1) {
                for (unsigned tries = 0; tries < workers.size(); ++tries) {
                    unsigned victim = dist(rng);
                    if (victim == self) continue;
                    auto &w = *workers[victim];

                    if (w.mutex.try_lock()) {
                        if (!w.tasks.empty()) {
                            job = std::move(w.tasks.back()); // kradniemy z końca
                            w.tasks.pop_back();
                            w.mutex.unlock();
                            job();
                            job = nullptr;
                            break;
                        }
                        w.mutex.unlock();
                    }
                }
            }
        }
    }

    std::vector<std::unique_ptr<Worker> > workers;
    std::atomic<bool> stop;
    std::atomic<unsigned> roundRobin;
};
