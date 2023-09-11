#pragma once
// MersenneTwisterRngEngine.h

#include <cstdint>
#include <random>

namespace RandomInternal
{
    class MersenneTwisterRngEngine
    {
    public:
        MersenneTwisterRngEngine();
        MersenneTwisterRngEngine(const unsigned seed);

        void Seed();
        void Seed(const unsigned seed);
        uint64_t Rand();
        int RandRange(const int min, const int max);
        float RandRange(const float min, const float max);
        float NormalizedRand();
        float SignedNormalizedRand();

    private:
        std::mt19937 m_engine;
    };
}