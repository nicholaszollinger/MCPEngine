// RNG.cpp

#include "RNG.h"
#include <ctime>

RandomNumberGenerator::RandomNumberGenerator()
    : m_internalSeed{}
{
    Seed();
}

RandomNumberGenerator::RandomNumberGenerator(const unsigned seed)
    : m_internalSeed{}
{
    Seed(seed);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Seeds the Random Number Generator using the current time.
//-----------------------------------------------------------------------------------------------------------------------------
void RandomNumberGenerator::Seed()
{
    m_internalSeed[0] = std::time(nullptr);
    m_internalSeed[1] = std::time(nullptr);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Seed the RandomNumberGenerator using a specific seed. Great for reusing random behaviors.
//-----------------------------------------------------------------------------------------------------------------------------
void RandomNumberGenerator::Seed(const unsigned seed)
{
    m_internalSeed[0] = seed;
    m_internalSeed[1] = seed;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      Source: XorShift128+ https://en.wikipedia.org/wiki/Xorshift
//
///		@brief : Return a random number. To be used the same way as rand() in the standard library.
///		@returns : Random unsigned long long value.
//-----------------------------------------------------------------------------------------------------------------------------
uint64_t RandomNumberGenerator::Rand()
{
    // Xorshift_128+
    uint64_t x = m_internalSeed[0];
    const uint64_t y = m_internalSeed[1];
    m_internalSeed[0] = y;
    x ^= x << 23; // a
    m_internalSeed[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
    return m_internalSeed[1] + y;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns random result of either true or false.
//-----------------------------------------------------------------------------------------------------------------------------
bool RandomNumberGenerator::RandBool()
{
    return Rand() % 2;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns and float value in the range [0.f, 1.f].
//-----------------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::NormalizedRand()
{
    const float result = static_cast<float>(Rand()) / static_cast<float>(kRandMax);
    return result;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns and float value in the range [-1.f, 1.f].
//-----------------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::SignedNormalizedRand()
{
    static constexpr float kHalfRandMax = static_cast<float>(kRandMax) / 2.f;
    
    const float result = (static_cast<float>(Rand()) - kHalfRandMax) / kHalfRandMax;
    return result;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns a new vector with random values between the minRange and maxRange.
///		@param minRange : Minimum x and y values for the output vector.
///		@param maxRange : Maximum x and y values for the output vector.
//-----------------------------------------------------------------------------------------------------------------------------
Vec2 RandomNumberGenerator::RandVectorInRange(const Vec2& minRange, const Vec2& maxRange)
{
    Vec2 output;
    output.x = RandRange(minRange.x, maxRange.x);
    output.y = RandRange(minRange.y, maxRange.y);
    return output;
}