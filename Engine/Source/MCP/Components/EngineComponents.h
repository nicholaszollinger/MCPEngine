#pragma once
// EngineComponents.h

// This header is included to ensure that all of the engine components are registered to the Game project
// regardless if they use them directly in code.
//      - The game could just have the components loaded from data, and if they try to load them without 
//        being registered, things will break.
//      - This is something that I would like to fix structurally, like making a defined registration step
//        instead of my 'automatic' version, but right now, this is the easy fix.

#include "ColliderComponent.h"
#include "ImageComponent.h"
#include "PrimitiveComponent.h"
#include "Rect2DComponent.h"
#include "ScriptComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"

// UI
#include "MCP/UI/ButtonWidget.h"
#include "MCP/UI/CanvasWidget.h"
#include "MCP/UI/ImageWidget.h"
#include "MCP/UI/TextWidget.h"
#include "MCP/UI/Widget.h"