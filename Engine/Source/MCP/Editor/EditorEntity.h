#pragma once
// EditorEntity.h

#include "MCP/Core/Event/ApplicationEvent.h"
//#include "MCP/Scene/SceneEntity.h"

namespace mcp
{
    class EditorEntity
    {
        Vec2 m_grabRelativePos; // Where the mouse hit the entity 
        bool m_selected;        // Whether this entity is currently Selected

    public:
        EditorEntity() = default;
        virtual ~EditorEntity() = default;

        void HandleMouseButton(MouseButtonEvent& event);
        void HandleMouseMove(MouseMoveEvent& event);

        virtual bool CheckForIntersection() = 0;

    };
}