// MersenneTwisterRngEngine.cpp

#include "MersenneTwisterRngEngine.h"

#include <random>

namespace RandomInternal
{
    MersenneTwisterRngEngine::MersenneTwisterRngEngine()
    {
        Seed();
    }

    MersenneTwisterRngEngine::MersenneTwisterRngEngine(const unsigned seed)
    {
        Seed(seed);
    }

    void MersenneTwisterRngEngine::Seed()
    {
        std::random_device randomDevice;
        m_engine.seed(randomDevice());
    }

    void MersenneTwisterRngEngine::Seed(const unsigned seed)
    {
        m_engine.seed(seed);
    }

    uint64_t MersenneTwisterRngEngine::Rand()
    {
        return m_engine();
    }

    int MersenneTwisterRngEngine::RandRange(const int min, const int max)
    {
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(m_engine);
    }

    float MersenneTwisterRngEngine::RandRange(const float min, const float max)
    {
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(m_engine);
    }

    float MersenneTwisterRngEngine::NormalizedRand()
    {
        std::uniform_real_distribution<float> distribution(0.f, 1.0f);
        return distribution(m_engine);
    }

    float MersenneTwisterRngEngine::SignedNormalizedRand()
    {
        std::uniform_real_distribution<float> distribution(-1.f, 1.f);
        return distribution(m_engine);
    }
}
