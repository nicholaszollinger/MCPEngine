// RNG.cpp

#include "RNG.h"

RandomNumberGenerator::RandomNumberGenerator(const unsigned seed)
    : m_engine(seed)
{
    //
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Seeds the Random Number Generator using the current time.
//-----------------------------------------------------------------------------------------------------------------------------
void RandomNumberGenerator::Seed()
{
    m_engine.Seed();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Seed the RandomNumberGenerator using a specific seed. Great for reusing random behaviors.
//-----------------------------------------------------------------------------------------------------------------------------
void RandomNumberGenerator::Seed(const unsigned seed)
{
    m_engine.Seed(seed);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Return a random number. To be used the same way as rand() in the standard library.
///		@returns : Random unsigned long long value.
//-----------------------------------------------------------------------------------------------------------------------------
uint64_t RandomNumberGenerator::Rand()
{
    return m_engine.Rand();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns random result of either true or false.
//-----------------------------------------------------------------------------------------------------------------------------
bool RandomNumberGenerator::FlipACoin()
{
    return Rand() % 2 == 0 ? false : true;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns and int value in the range [min, max].
//-----------------------------------------------------------------------------------------------------------------------------
int RandomNumberGenerator::RandRange(const int min, const int max)
{
    return m_engine.RandRange(min, max);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns a float value in the range [min, max].
//-----------------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::RandRange(const float min, const float max)
{
    return m_engine.RandRange(min, max);
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns and float value in the range [0.f, 1.f].
//-----------------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::NormalizedRand()
{
    return m_engine.NormalizedRand();
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Returns and float value in the range [-1.f, 1.f].
//-----------------------------------------------------------------------------------------------------------------------------
float RandomNumberGenerator::SignedNormalizedRand()
{
    return m_engine.SignedNormalizedRand();
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