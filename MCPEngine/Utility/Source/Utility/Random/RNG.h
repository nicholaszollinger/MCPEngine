#pragma once
// RNG.h

#include <cstdint>
#include <limits>
#include "Types/Vector2.h"
 
//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES
//
///		@brief : Class for generating random numbers.
//-----------------------------------------------------------------------------------------------------------------------------
class RandomNumberGenerator
{
    static constexpr uint64_t kRandMax = std::numeric_limits<uint64_t>::max();
    uint64_t m_internalSeed[2];

public:
    // Constructors
    RandomNumberGenerator();
    RandomNumberGenerator(const unsigned seed);

    // Seeding
    void Seed();
    void Seed(const unsigned seed);

    // RNG Functions
    uint64_t Rand();
    bool FlipACoin();
    int RandRange(const int min, const int max);
    float RandRange(const float min, const float max);
    float NormalizedRand();
    float SignedNormalizedRand();
    Vec2 RandVectorInRange(const Vec2& minRange, const Vec2& maxRange);
};

using Rng = RandomNumberGenerator;