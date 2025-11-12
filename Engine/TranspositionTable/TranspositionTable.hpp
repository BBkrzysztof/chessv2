#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>
#include <optional>
#include <cstring>
#include <cassert>

// Typ pomocniczy
using PackedMove = uint16_t;

enum class TTFlag : uint8_t {
    NONE = 0,
    EXACT = 1,
    LOWER = 2,
    UPPER = 3
};

struct TTProbeResult {
    bool hit = false;
    int32_t score = 0;
    uint8_t depth = 0;
    TTFlag flag = TTFlag::NONE;
    PackedMove move = 0;
};

class TranspositionTable {
public:
    explicit TranspositionTable(size_t sizeEntries = (1<<20)) {
        assert((sizeEntries & (sizeEntries - 1)) == 0 && "sizeEntries must be power of two");
        size = sizeEntries;
        mask = size - 1;

        // allocate atomic arrays (przenośne; std::vector<std::atomic<...>> powoduje problems)
        keys = std::make_unique<std::atomic<uint64_t>[]>(size);
        values = std::make_unique<std::atomic<uint64_t>[]>(size);

        for (size_t i = 0; i < size; ++i) {
            keys[i].store(0, std::memory_order_relaxed);
            values[i].store(0, std::memory_order_relaxed);
        }
        currentAge = 0;
    }

    void clear() {
        for (size_t i = 0; i < size; ++i) {
            keys[i].store(0, std::memory_order_relaxed);
            values[i].store(0, std::memory_order_relaxed);
        }
        currentAge = 0;
    }

    void newSearchIteration() {
        currentAge = (currentAge + 1) & 0x3F;
    }

    TTProbeResult probe(uint64_t key, uint8_t minDepth) const {
        TTProbeResult res;
        const size_t idx = static_cast<size_t>(key & mask);

        uint64_t k1 = keys[idx].load(std::memory_order_acquire);
        if (k1 == 0) return res;
        if ((k1 ^ key) != 0) return res;

        uint64_t packed = values[idx].load(std::memory_order_relaxed);
        uint64_t k2 = keys[idx].load(std::memory_order_acquire);
        if (k1 != k2) return res;

        unpackValue(packed, res.score, res.depth, res.flag, res.move);
        if (res.depth >= minDepth) {
            res.hit = true;
            return res;
        }

        // depth too small => miss but keep move for ordering
        return res;
    }

    void store(uint64_t key, uint8_t depth, int32_t score, TTFlag flag, PackedMove move) {
        const size_t idx = static_cast<size_t>(key & mask);
        const uint64_t packed = packValue(score, depth, flag, move, currentAge);

        uint64_t existingKey = keys[idx].load(std::memory_order_acquire);
        if (existingKey != 0 && (existingKey ^ key) == 0) {
            uint64_t existingPacked = values[idx].load(std::memory_order_relaxed);
            uint8_t existingDepth = unpackDepth(existingPacked);
            if (!(existingDepth > depth && (currentAge == unpackAge(existingPacked)))) {
                values[idx].store(packed, std::memory_order_relaxed);
                keys[idx].store(key, std::memory_order_release);
            }
            return;
        }

        if (existingKey == 0) {
            values[idx].store(packed, std::memory_order_relaxed);
            keys[idx].store(key, std::memory_order_release);
            return;
        } else {
            uint64_t existingPacked = values[idx].load(std::memory_order_relaxed);
            uint8_t existingDepth = unpackDepth(existingPacked);
            uint8_t existingAge = unpackAge(existingPacked);
            uint8_t ageGap = static_cast<uint8_t>((currentAge - existingAge) & 0x3F);

            bool replace = false;
            if (depth > existingDepth) replace = true;
            else if (depth == existingDepth && ageGap > 0) replace = true;
            else if (ageGap > 10) replace = true;

            if (replace) {
                values[idx].store(packed, std::memory_order_relaxed);
                keys[idx].store(key, std::memory_order_release);
            }
        }
    }

    size_t entryCount() const { return size; }

private:
    size_t size;
    size_t mask;
    // replaced std::vector<std::atomic<uint64_t>> by unique_ptr to array of atomics
    std::unique_ptr<std::atomic<uint64_t>[]> keys;
    std::unique_ptr<std::atomic<uint64_t>[]> values;
    std::atomic<uint8_t> currentAgeAtomic{0};
    uint8_t currentAge = 0;

    static uint64_t packValue(int32_t score, uint8_t depth, TTFlag flag, PackedMove move, uint8_t age) {
        uint64_t s = static_cast<uint32_t>(static_cast<uint32_t>(score) ^ 0x80000000u);
        uint64_t res = 0;
        res |= (uint64_t)(move & 0xFFFFu);
        res |= (s & 0xFFFFFFFFull) << 16;
        res |= (uint64_t)(depth) << 48;
        res |= (uint64_t)(static_cast<uint8_t>(flag) & 0x3u) << 56;
        res |= (uint64_t)(age & 0x3Fu) << 58;
        return res;
    }

    static void unpackValue(uint64_t packed, int32_t &outScore, uint8_t &outDepth, TTFlag &outFlag, PackedMove &outMove) {
        outMove = static_cast<PackedMove>(packed & 0xFFFFu);
        uint32_t s = static_cast<uint32_t>((packed >> 16) & 0xFFFFFFFFu);
        outScore = static_cast<int32_t>(s ^ 0x80000000u);
        outDepth = static_cast<uint8_t>((packed >> 48) & 0xFFu);
        outFlag = static_cast<TTFlag>((packed >> 56) & 0x3u);
    }

    static uint8_t unpackDepth(uint64_t packed) {
        return static_cast<uint8_t>((packed >> 48) & 0xFFu);
    }
    static uint8_t unpackAge(uint64_t packed) {
        return static_cast<uint8_t>((packed >> 58) & 0x3Fu);
    }
};
