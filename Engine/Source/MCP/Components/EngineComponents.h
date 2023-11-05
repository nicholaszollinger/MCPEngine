#pragma once
// EngineComponents.h

// This header is included to ensure that all of the engine components are registered to the Game project
// regardless if they use them directly in code.
//      - The game could just have the components loaded from data, and if they try to load them without 
//        being registered, things will break.
//      - This is something that I would like to fix structurally, like making a defined registration step
//        instead of my 'automatic' version, but right now, this is the easy fix.

// Components
#include "ColliderComponent.h"
#include "ImageComponent.h"
#include "InputComponent.h"
#include "PrimitiveComponent.h"
#include "Rect2DComponent.h"
#include "ScriptComponent.h"
#include "TextComponent.h"
#include "TransformComponent.h"

// UI Widgets
#include "MCP/UI/BarWidget.h"
#include "MCP/UI/ButtonWidget.h"
#include "MCP/UI/CanvasWidget.h"
#include "MCP/UI/HorizontalLayoutWidget.h"
#include "MCP/UI/ImageWidget.h"
#include "MCP/UI/SelectionWidget.h"
#include "MCP/UI/SliderWidget.h"
#include "MCP/UI/TextFieldWidget.h"
#include "MCP/UI/TextWidget.h"
#include "MCP/UI/ToggleWidget.h"
#include "MCP/UI/VerticalLayoutWidget.h"
#include "MCP/UI/Widget.h"