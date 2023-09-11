// ColliderFactory.cpp

#include "ColliderFactory.h"

// This file serves as a way to compile all of the types of colliders so that their
// Ids are registered.
#include "Box2DCollider.h"

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Dummy Function. HOWEVER, this needs to be called for the simple reason of enforcing this cpp file to be
///             compiled.
//-----------------------------------------------------------------------------------------------------------------------------
void mcp::ColliderFactory::RegisterEngineColliderTypes()
{
    //
}