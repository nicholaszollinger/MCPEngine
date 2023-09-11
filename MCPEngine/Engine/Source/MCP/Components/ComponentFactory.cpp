// ComponentFactory.cpp

#include "ComponentFactory.h"

// Engine Components
#include "ColliderComponent.h"
#include "ImageComponent.h"
#include "PrimitiveComponent.h"
#include "Rect2DComponent.h"
#include "ScriptComponent.h"
#include "TransformComponent.h"

// Tiled Components
#include "Platform/Tiled/TiledMapComponent.h"

void mcp::ComponentFactory::RegisterEngineComponentTypes()
{
    //RegisterComponentType<ColliderComponent>(STRINGIFY(ColliderComponent));
    //RegisterComponentType<ImageComponent>(STRINGIFY(ImageComponent));
    //RegisterComponentType<PrimitiveComponent>(STRINGIFY(PrimitiveComponent));
    //RegisterComponentType<Rect2DComponent>(STRINGIFY(Rect2DComponent));
    //RegisterComponentType<ScriptComponent>(STRINGIFY(ScriptComponent));
    //RegisterComponentType<TransformComponent>(STRINGIFY(TransformComponent));
    //RegisterComponentType<TiledMapComponent>(STRINGIFY(TiledMapComponent));
}
