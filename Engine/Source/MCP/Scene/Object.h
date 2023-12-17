#pragma once
// Object.h

#include <BleachNew.h>
#include <cstdint>
#include <vector>

#include "SceneEntity.h"
#include "MCP/Debug/Log.h"
#include "MCP/Components/Component.h"

namespace mcp
{
    class Scene;
    class WorldLayer;
    using ObjectId = uint32_t;

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: In Engine v2, I realized that having only Objects have components kind of sucks, so this functionality is likely
    //      going to be merged up into SceneEntity.
    //
    ///		@brief : An Object is a SceneEntity that can store components.
    //-----------------------------------------------------------------------------------------------------------------------------
    class Object final : public SceneEntity
    {
        MCP_DEFINE_SCENE_ENTITY(Object)

        std::vector<Component*> m_components;

    public:
        Object() = default;
        explicit Object(const SceneEntityConstructionData& data);
        virtual ~Object() override;

        // Initialization:
        virtual bool Init() override;
        virtual bool PostLoadInit() override;

        // Component Management:
        void AddComponent(Component* pComponent);

        template<typename ComponentType, typename...ConstructorParams>
        ComponentType* AddComponent(ConstructorParams&&...params);

        template<typename ComponentType>
        void RemoveComponent();

        template<typename ComponentType>
        ComponentType* GetComponent();

        [[nodiscard]] virtual Object* GetChildByTag(const StringId tag) override;
        [[nodiscard]] virtual Object* GetChildByTag(const StringId tag) const override;
        [[nodiscard]] WorldLayer* GetWorld() const;

    protected:
        // Scene Entity Interface.
        virtual void OnDestroy() override;
        virtual void OnActive() override;
        virtual void OnInactive() override;
        virtual void OnParentSet() override;
        virtual void OnChildAdded([[maybe_unused]] SceneEntity* pChild) override {}
        virtual void OnChildRemoved([[maybe_unused]] SceneEntity* pChild) override {}

        [[nodiscard]] virtual Object* GetParent() const override { return SafeCastEntity<Object>(m_pParent); }
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO: Need to find a way to handle Component Dependencies.
    //
    ///		@brief : Add a Component of type ComponentType to the Object.
    ///		@tparam ComponentType : Type of Component you want to add.
    ///		@tparam ConstructorParams : Any parameters the Component needs to be constructed.
    ///		@param params : Parameters for the constructor of the Component. The Object* will be 'this' object.
    ///		@returns : Pointer to the new Component, or nullptr if a Component of that type already exists on the Object.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ComponentType, typename...ConstructorParams>
    ComponentType* Object::AddComponent(ConstructorParams&&...params)
    {
        // Check to see if we already have the component on this Object.
        if (GetComponent<ComponentType>() != nullptr)
        {
            MCP_WARN("Object", "Failed to add Component! Tried to add ComponentType that already exists on Object!");
            return nullptr;
        }

        // Create the new component, passing in the constructor parameters.
        ComponentType* pNewComponent = BLEACH_NEW(ComponentType(params...));
        pNewComponent->m_pOwner = this;

        // Initialize the Component
        if (!pNewComponent->Init())
        {
            MCP_ERROR("Object", "Failed to add Component! Initialization of Component failed!");
            BLEACH_DELETE(pNewComponent);
            return nullptr;
        }

        // If this object as a parent, we need to let the component respond.
        if (HasAParent())
        {
            pNewComponent->OnOwnerParentSet(GetParent());
        }

        // Set the Component to its starting active state.
        pNewComponent->OnOwnerActiveChanged(IsActive());

        // Add the Component* to our master list, then return the pointer to the new Component.
        m_components.push_back(pNewComponent); 
        return pNewComponent;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Remove a Component from the Object. If no object exists, nothing will happen.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ComponentType>
    void Object::RemoveComponent()
    {
        for (auto iterator = m_components.begin(); iterator != m_components.end(); ++iterator)
        {
            if (ComponentType::GetStaticTypeId() == (*iterator)->GetTypeId())
            {
                // Delete the value
                delete (*iterator);

                // Move the back component to fit into the removed components spot,
                (*iterator) = m_components.back();

                // Get rid of the last element.
                m_components.pop_back();
            }
        }

        MCP_WARN("Object", "Failed to remove Component! Object doesn't have a Component of that type!");
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Get the Object's component of ComponentType. If no component exists, then this will return nullptr. 
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ComponentType>
    ComponentType* Object::GetComponent()
    {
        for (auto& pComponent : m_components)
        {
            if (ComponentType::GetStaticTypeId() == pComponent->GetTypeId())
            {
                return static_cast<ComponentType*>(pComponent);
            }
        }

        return nullptr;
    }
}