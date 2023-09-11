#pragma once
// RNG.h

#include <cstdint>
#include "MersenneTwisterRngEngine.h"
#include "XorShift128RngEngine.h"
#include "../Vector2.h"
 
//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      I have the using statement as a hold over from a previous project, and it allows me to plug in different implementations.
//      TODO: Once I get the XorShift Engine up and running, I should use that.
//
///		@brief : 
//-----------------------------------------------------------------------------------------------------------------------------
class RandomNumberGenerator
{
    using RngEngine = RandomInternal::XorShiftRngEngine;
public:
    RandomNumberGenerator() = default;
    RandomNumberGenerator(const unsigned seed);

    void Seed();
    void Seed(const unsigned seed);

    uint64_t Rand();
    bool FlipACoin();
    int RandRange(const int min, const int max);
    float RandRange(const float min, const float max);
    float NormalizedRand();
    float SignedNormalizedRand();
    Vec2 RandVectorInRange(const Vec2& minRange, const Vec2& maxRange);

private:
    RngEngine m_engine;
};