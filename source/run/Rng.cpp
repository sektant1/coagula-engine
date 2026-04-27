#include "run/Rng.h"

namespace mnd
{
namespace
{
// splitmix64 — Vigna's mix used by xoroshiro/xoshiro authors to expand a single
// seed into independent stream seeds. Cheap and avoids correlated state across
// sub-streams.
std::uint64_t SplitMix64(std::uint64_t x)
{
    x += 0x9E3779B97F4A7C15ULL;
    x = (x ^ (x >> 30U)) * 0xBF58476D1CE4E5B9ULL;
    x = (x ^ (x >> 27U)) * 0x94D049BB133111EBULL;
    return x ^ (x >> 31U);
}
}  // namespace

void RngStreams::Reseed(std::uint64_t seed)
{
    m_rootSeed = seed;
    layout.Seed(SplitMix64(seed ^ 0x1111111111111111ULL));
    ai.Seed(SplitMix64(seed ^ 0x2222222222222222ULL));
    loot.Seed(SplitMix64(seed ^ 0x3333333333333333ULL));
    fx.Seed(SplitMix64(seed ^ 0x4444444444444444ULL));
}

}  // namespace mnd
