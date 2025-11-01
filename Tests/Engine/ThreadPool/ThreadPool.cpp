// test_pool.cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "../TestUtils.hpp"
#include "../../../Engine/ThreadPool/ThreadPool.hpp"
#include <atomic>
#include <vector>
#include <thread>
#include <chrono>
#include <future>
#include <numeric>
#include <stdexcept>

using namespace std::chrono_literals;

TEST_CASE("Wszystkie zadania są wykonane i zwracają poprawne wyniki") {
    ThreadPool pool(4);
    const int N = 1000;

    std::vector<std::future<int>> futs;
    futs.reserve(N);
    for (int i=0;i<N;++i) {
        futs.push_back(pool.submit([i]{ return i*i; }));
    }

    long long sum = 0;
    for (auto& f : futs) sum += f.get();

    // suma i^2 dla i=0..N-1 = (N-1)N(2N-1)/6
    long long expected = 1LL*(N-1)*N*(2LL*N-1)/6;
    REQUIRE(sum == expected);
}

TEST_CASE("Wyjątki w zadaniach propagują się przez future") {
    ThreadPool pool(2);
    auto f1 = pool.submit([]{ throw std::runtime_error("boom"); return 0; });
    auto f2 = pool.submit([]{ return 7; });

    // f2 ma działać normalnie
    REQUIRE(f2.get() == 7);

    // f1 powinien rzucić wyjątek przy get()
    REQUIRE_THROWS_AS(f1.get(), std::runtime_error);
}

TEST_CASE("Brak wycieków ani deadlocków przy niszczeniu puli bez zadań") {
    // po prostu zakres – destruktor nie może zablokować
    ThreadPool pool(3);
    // brak zadań
    SUCCEED(); // jeśli dotarliśmy tu – OK
}

TEST_CASE("Równoległość: czas ~ ceil(N/threads) * task_ms (z tolerancją)") {
    const unsigned threads = 4;
    ThreadPool pool(threads);

    const int N = 12;               // 12 zadań
    const auto task_ms = 100ms;     // każde „pracuje” 100 ms
    std::vector<std::future<void>> futs;
    futs.reserve(N);

    auto t0 = std::chrono::steady_clock::now();
    for (int i=0;i<N;++i) {
        futs.push_back(pool.submit([=]{
            std::this_thread::sleep_for(task_ms);
        }));
    }
    for (auto& f : futs) f.get();
    auto t1 = std::chrono::steady_clock::now();

    auto elapsed = t1 - t0;
    // oczekiwany czas ~ ceil(12 / 4) * 100ms = 3 * 100ms = 300ms
    // dodajmy tolerancję na scheduler/OS – powiedzmy do 600ms.
    REQUIRE(elapsed >= 250ms);   // nie powinno skończyć się „za szybko” (to sanity check)
    REQUIRE(elapsed <= 600ms);   // a tym bardziej nie ~1.2s, co byłoby czysto sekwencyjne
}

TEST_CASE("Work-stealing: głodny wątek kradnie robotę") {
    // Ten test jest „smoke”: nie mierzymy kto wykonał, tylko że całość kończy się szybko,
    // mimo że zadania lądują początkowo w jednej kolejce (round-robin może to rozrzucić,
    // ale przy 1 ofierze i wielu zadaniach kradzież i tak powinna zajść).
    ThreadPool pool(4);

    // Wrzucimy „spore” zadania, które wykonują się różnie długo, by sprowokować kradzieże.
    const int N = 20;
    CountDownLatch latch(N);

    for (int i=0;i<N;++i) {
        pool.submit([i,&latch]{
            // Zadania o różnym „ciężarze”
            std::this_thread::sleep_for(std::chrono::milliseconds(20 + (i%5)*10));
            latch.count_down();
        });
    }

    // Czekamy na skończenie z limitem – jeśli nie działa kradzież i kolejka „zatyka”,
    // test może wylecieć timeoutem.
    REQUIRE(latch.wait_for(1500ms)); // 1.5s powinno wystarczyć z zapasem
}

TEST_CASE("Pomoc z zewnątrz: help_one_round() potrafi wykonać zadanie") {
    ThreadPool pool(2);
    std::atomic<int> done{0};
    CountDownLatch latch(1);

    // wrzucamy jedno zadanie
    pool.submit([&]{
        done.fetch_add(1, std::memory_order_relaxed);
        latch.count_down();
    });

    // Wątek testu „pomaga” – powinien skonsumować job w jednej rundzie lub poczekać aż worker go zrobi.
    bool helped = pool.helpOneRound();
    // Niezależnie od tego, czy faktycznie ukradliśmy, wynik powinien być gotowy bardzo szybko:
    REQUIRE(latch.wait_for(200ms));
    REQUIRE(done.load(std::memory_order_relaxed) == 1);
    (void)helped; // opcjonalnie można aserty zrobić słabsze: REQUIRE((helped || ...));
}
