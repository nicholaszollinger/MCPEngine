// Component.cpp

#include "Component.h"

#include "MCP/Objects/Object.h"
#include "MCP/Scene/Scene.h"

namespace mcp
{
    Component::Component(Object* pObject)
        : m_pOwner(pObject)
        , m_isActive(true)
    {
        //RegisterComponentFactoryFunction(this);
    }


    MessageManager* Component::GetMessageManager() const
    {
        return m_pOwner->GetScene()->GetMessageManager();
    }

}
