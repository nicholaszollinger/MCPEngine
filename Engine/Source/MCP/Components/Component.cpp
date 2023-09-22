// Component.cpp

#include "Component.h"

#include "MCP/Scene/Object.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    Component::Component(Object* pObject)
        : m_pOwner(pObject)
        , m_isActive(true)
    {
        //
    }

    MessageManager* Component::GetMessageManager() const
    {
        return m_pOwner->GetScene()->GetMessageManager();
    }
}
