// CollisionProfile.cpp

#include "CollisionProfile.h"

namespace mcp
{
    CollisionProfile::CollisionProfile()
        : collisionResponses{ }                             
        , myCollisionChannel(CollisionChannel::kDefault)
    {
        //
    }
}