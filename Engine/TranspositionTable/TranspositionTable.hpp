#pragma once

#include <atomic>
#include <memory>
#include <cstring>
#include "../Evaluation/Evaluation.hpp"

enum class TTFlag : uint8_t {
    NONE = 0,
    EXACT = 1,
    LOWER = 2,
    UPPER = 3
};


class TranspositionTable {
public:
    static constexpr size_t BUCKET_SIZE = 4;

    struct TTProbeResult {
        bool hit = false;
        int32_t score = 0;
        uint8_t depth = 0;
        TTFlag flag = TTFlag::NONE;
        Move::Move move = 0;
    };

    TranspositionTable() = default;

    explicit TranspositionTable(const size_t capacity_mb) { resizeMb(capacity_mb); }

    void resizeMb(size_t mb) {
        if (mb < 1) mb = 1;
        const size_t bytes = mb * 1024ull * 1024ull;

        const size_t bucketsSize = bytes / (BUCKET_SIZE * sizeof(uint64_t));
        size_t pow2 = 1;
        while (pow2 < bucketsSize) pow2 <<= 1;
        this->buckets = std::max<size_t>(pow2, 256);

        entries = std::make_unique<std::atomic<uint64_t>[]>(this->buckets * BUCKET_SIZE);
        for (size_t i = 0; i < this->buckets * BUCKET_SIZE; ++i) {
            entries[i].store(0, std::memory_order_relaxed);
        }
        generation.store(1, std::memory_order_relaxed);
    }

    void clear() {
        if (!entries) return;
        for (size_t i = 0; i < this->buckets * BUCKET_SIZE; ++i)
            entries[i].store(0, std::memory_order_relaxed);
        generation.store(1, std::memory_order_relaxed);
    }

    void newSearch() {
        const uint8_t gen = this->generation.load(std::memory_order_relaxed);
        this->generation.store(static_cast<uint8_t>((gen + 1) & 0x3F), std::memory_order_relaxed);
    }

    TTProbeResult probe(
        const BitBoard &key,
        const uint8_t depth_min,
        const int ply,
        const int alpha,
        const int beta
    ) const {
        TTProbeResult r{};
        if (!this->entries) return r;

        const uint32_t idx = static_cast<uint32_t>(key) & static_cast<uint32_t>(this->buckets - 1);
        const uint16_t key16 = hi16(key);

        for (int way = 0; way < BUCKET_SIZE; ++way) {
            const uint64_t raw = this->entries[idx * BUCKET_SIZE + way].load(std::memory_order_relaxed);
            if (raw == 0) continue;

            const Entity d = decode(raw);

            if (d.key16 != key16) continue;
            if (d.depth < depth_min) continue;

            r.hit = true;
            r.depth = d.depth;
            r.flag = static_cast<TTFlag>(d.flag);
            r.move = d.move;

            const int s = static_cast<int>(static_cast<int16_t>(d.score16));
            r.score = Evaluation::fromTtScore(s, ply);

            if (r.flag == TTFlag::EXACT) return r;
            if (r.flag == TTFlag::LOWER && r.score >= beta) return r;
            if (r.flag == TTFlag::UPPER && r.score <= alpha) return r;

            return r;
        }
        return r;
    }

    void store(
        const BitBoard &key,
        const uint8_t depth,
        const int &score,
        TTFlag flag,
        const Move::Move &move,
        const int ply
    ) {
        if (!this->entries) return;

        const uint32_t idx = static_cast<uint32_t>(key) & static_cast<uint32_t>(this->buckets - 1);
        const uint16_t key16 = hi16(key);
        const uint8_t gen = this->generation.load(std::memory_order_relaxed) & 0x3F;

        Entity e{};
        e.key16 = key16;
        e.depth = depth;
        e.flag = static_cast<uint8_t>(flag);
        e.generation = gen;
        e.move = move;

        const int tt = Evaluation::toTtScore(score, ply);
        e.score16 = static_cast<uint16_t>(static_cast<int16_t>(std::clamp(tt, -32767, 32767)));

        const uint64_t raw = encode(e);

        int victim = -1;
        int worstScore = 1e9;

        for (int way = 0; way < BUCKET_SIZE; ++way) {
            const size_t pos = idx * BUCKET_SIZE + way;
            const uint64_t cur = this->entries[pos].load(std::memory_order_relaxed);

            if (cur == 0) {
                this->entries[pos].store(raw, std::memory_order_relaxed);
                return;
            }

            const Entity d = decode(cur);

            if (d.key16 == key16) {
                if (depth >= d.depth || is_newer(gen, d.generation)) {
                    this->entries[pos].store(raw, std::memory_order_relaxed);
                }
                return;
            }

            const int age_penalty = is_newer(d.generation, gen) ? 8 : 0; // jeśli stary -> większa kara
            const int score_victim = static_cast<int>(d.depth) - age_penalty;
            if (score_victim < worstScore) {
                worstScore = score_victim;
                victim = static_cast<int>(pos);
            }
        }

        if (victim >= 0) {
            this->entries[static_cast<size_t>(victim)].store(raw, std::memory_order_relaxed);
        }
    }

private:
    struct Entity {
        Move::Move move;
        uint16_t score16;
        uint8_t depth;
        uint8_t flag;
        uint8_t generation;
        uint16_t key16;
    };

    static uint64_t encode(const Entity &e) {
        return (static_cast<uint64_t>(e.key16) << 48) |
               (static_cast<uint64_t>(e.generation) << 42) |
               (static_cast<uint64_t>(e.flag & 0x3) << 40) |
               (static_cast<uint64_t>(e.depth) << 32) |
               (static_cast<uint64_t>(e.score16) << 16) |
               (static_cast<uint64_t>(e.move));
    }

    static Entity decode(const BitBoard &x) {
        Entity d{};
        d.move = static_cast<uint16_t>(x & 0xFFFFu);
        d.score16 = static_cast<uint16_t>((x >> 16) & 0xFFFFu);
        d.depth = static_cast<uint8_t>((x >> 32) & 0xFFu);
        d.flag = static_cast<uint8_t>((x >> 40) & 0x3u);
        d.generation = static_cast<uint8_t>((x >> 42) & 0x3Fu);
        d.key16 = static_cast<uint16_t>((x >> 48) & 0xFFFFu);
        return d;
    }

    static uint16_t hi16(const uint64_t &key) {
        return static_cast<uint16_t>((key >> 48) & 0xFFFFu);
    }

    static bool is_newer(const uint8_t a, const uint8_t b) {
        return static_cast<uint8_t>(a - b) < 32;
    }


    size_t buckets{0};
    std::unique_ptr<std::atomic<uint64_t>[]> entries;
    std::atomic<uint8_t> generation{1};
};
