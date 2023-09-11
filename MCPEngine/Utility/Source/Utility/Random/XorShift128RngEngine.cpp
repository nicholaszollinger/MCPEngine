// XorShift128RngEngine.cpp

#include "XorShift128RngEngine.h"
#include <ctime>

namespace RandomInternal
{
    XorShiftRngEngine::XorShiftRngEngine()
        : m_internalSeed{ }
    {
        Seed();
    }
    
    XorShiftRngEngine::XorShiftRngEngine(const unsigned seed)
        : m_internalSeed{ }
    {
        Seed(seed);
    }
    
    void XorShiftRngEngine::Seed()
    {
        m_internalSeed[0] = std::time(nullptr);
        m_internalSeed[1] = std::time(nullptr);
    }
    
    void XorShiftRngEngine::Seed(const unsigned seed)
    {
        m_internalSeed[0] = 0;
        m_internalSeed[1] = seed;
    }
    
    uint64_t XorShiftRngEngine::Rand()
    {
        // Xorshift_128+
        uint64_t x = m_internalSeed[0];
        uint64_t const y = m_internalSeed[1];
        m_internalSeed[0] = y;
        x ^= x << 23; // a
        m_internalSeed[1] = x ^ y ^ (x >> 17) ^ (y >> 26); // b, c
        return m_internalSeed[1] + y;
    }
    
    bool XorShiftRngEngine::FlipACoin()
    {
        return Rand() % 2;
    }
    
    
    int XorShiftRngEngine::RandRange(const int min, const int max)
    {
        const int range = max - min;
        return min + static_cast<int>((Rand() % (range + 1)));
    }
    
    float XorShiftRngEngine::RandRange(const float min, const float max)
    {
        const float range = max - min;
        const float roll = NormalizedRand();
        
        return min + roll * range;
    }
    
    float XorShiftRngEngine::NormalizedRand()
    {
        const float result = static_cast<float>(Rand()) / static_cast<float>(kRandMax);
        return result;
    }
    
    float XorShiftRngEngine::SignedNormalizedRand()
    {
        static constexpr float kHalfRandMax = static_cast<float>(kRandMax) / 2.f;
    
        const float result = (static_cast<float>(Rand()) - kHalfRandMax) / kHalfRandMax;
        return result;
    }

}
