#pragma once
// ComponentSystem.h

// TODO: ECS is for the Summer!

#include <bitset>
#include <vector>
#include "../Components/Component.h"

// Systems need to operate only on entities that have a certain signature:
// - The signature is going to be a series of bits. The number of bits should equal the
//   max number of component types there are. (How do I set this at compile time?).

using Signature = std::bitset<32>;

namespace mcp
{
    class ComponentSystem
    {
    public:
        virtual ~ComponentSystem() = default;

    };
}
