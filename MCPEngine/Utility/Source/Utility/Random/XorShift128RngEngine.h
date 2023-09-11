#pragma once
// XorShift128RngEngine.h
#include <cstdint>
#include <limits>

namespace RandomInternal
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      The random number generator is adapted from xorshift128+:
    //          https://en.wikipedia.org/wiki/Xorshift
    //
    ///		@brief : 
    //-----------------------------------------------------------------------------------------------------------------------------
    class XorShiftRngEngine
    {
        static constexpr uint64_t kRandMax = std::numeric_limits<uint64_t>::max();
        uint64_t m_internalSeed[2];

    public:
        XorShiftRngEngine();
        XorShiftRngEngine(const unsigned seed);

        void Seed();
        void Seed(const unsigned seed);

        uint64_t Rand();
        bool FlipACoin();
        int RandRange(const int min, const int max);
        float RandRange(const float min, const float max);
        float NormalizedRand();
        float SignedNormalizedRand();
        //Vec2 RandVectorInRange(const Vec2& minRange, const Vec2& maxRange);
    };
}
