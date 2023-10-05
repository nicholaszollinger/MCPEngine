#pragma once
// RNG.h

#include <cstdint>
#include <limits>
#include "../Types/Vector2.h"
 
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
    bool RandBool();

    template<typename NumberType>
    NumberType RandRange(const NumberType min, const NumberType max);

    float NormalizedRand();
    float SignedNormalizedRand();
    Vec2 RandVectorInRange(const Vec2& minRange, const Vec2& maxRange);
};

#pragma warning( push )
#pragma warning( disable : 4702 ) // Disabling the unreachable code warning.

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      This would look nicer in c++20 :)
//		
///		@brief : Returns a value in the range [min, max].
//-----------------------------------------------------------------------------------------------------------------------------
template <typename NumberType>
NumberType RandomNumberGenerator::RandRange(const NumberType min, const NumberType max)
{
    static_assert(std::is_integral_v<NumberType> || std::is_floating_point_v<NumberType>, "Value must be either floating point or integral type!");

    // Integral Rand range
    if constexpr (std::is_integral_v<NumberType>)
    {
        const NumberType range = max - min;
        return min + static_cast<NumberType>((Rand() % (range + 1)));
    }

    // Floating point rand range:
    const NumberType range = max - min;
    const auto roll = static_cast<NumberType>(NormalizedRand());
    
    return min + roll * range;
}

#pragma warning ( pop )

using Rng = RandomNumberGenerator;