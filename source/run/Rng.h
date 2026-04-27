/**
 * @file Rng.h
 * @brief Seedable RNG service for run state, dungeon layout, AI, loot, and FX.
 *
 * `Rng` wraps `std::mt19937_64` with the few helpers gameplay code actually
 * needs (Range / Float01 / Pick). `RngStreams` packs four independent streams
 * derived from one root seed via splitmix64 so loot rolls cannot perturb
 * dungeon layout determinism, and AI jitter cannot desync particle FX.
 */

#pragma once

#include <cstdint>
#include <random>
#include <vector>

namespace mnd
{

class Rng
{
 public:
    Rng() : m_engine(0xDEADBEEFCAFEBABEULL) {}
    explicit Rng(std::uint64_t seed) : m_engine(seed) {}

    void          Seed(std::uint64_t seed) { m_engine.seed(seed); }
    std::uint64_t Next() { return m_engine(); }

    /// Inclusive integer range in [lo, hi].
    int Range(int lo, int hi)
    {
        std::uniform_int_distribution<int> dist(lo, hi);
        return dist(m_engine);
    }

    float Float01()
    {
        std::uniform_real_distribution<float> dist(0.0F, 1.0F);
        return dist(m_engine);
    }

    float FloatRange(float lo, float hi)
    {
        std::uniform_real_distribution<float> dist(lo, hi);
        return dist(m_engine);
    }

    /// Pick uniformly from a non-empty container. Caller guarantees non-empty.
    template <class T>
    const T &Pick(const std::vector<T> &items)
    {
        return items[static_cast<std::size_t>(Range(0, static_cast<int>(items.size()) - 1))];
    }

 private:
    std::mt19937_64 m_engine;
};

/**
 * @brief Four independent RNG streams derived from a single root seed.
 *
 * Sub-streams are isolated so adding a loot roll mid-run cannot shift the
 * dungeon layout, and so debug consumers (FX) cannot pull from gameplay
 * streams. Reseed with `RngStreams::Reseed(seed)` at run start; reseed the
 * `layout` stream alone per floor with `seed ^ floor` if you want each floor's
 * layout fully determined by `(seed, floor)`.
 */
class RngStreams
{
 public:
    RngStreams() { Reseed(0xC0FFEE0123456789ULL); }

    void Reseed(std::uint64_t seed);

    [[nodiscard]] std::uint64_t Seed() const { return m_rootSeed; }

    Rng layout;  ///< Dungeon room pick + stitching.
    Rng ai;      ///< AI decisions, jitter, attack timing.
    Rng loot;    ///< Loot table rolls, rarity tiers.
    Rng fx;      ///< Particle jitter, screenshake, cosmetic-only RNG.

 private:
    std::uint64_t m_rootSeed = 0;
};

}  // namespace mnd
