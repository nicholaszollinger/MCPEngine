#pragma once
// Object.h

#include <BleachNew.h>
#include <cstdint>
#include <vector>
#include "MCP/Debug/Log.h"
#include "MCP/Components/Component.h"

// TODO: Need to create a type for the ComponentType that checks to make sure it follows the necessary requirements.

namespace mcp
{
    class Scene;
    class WorldLayer;

    using ObjectId = uint32_t;
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : An Object is an entity that can store components.
    //-----------------------------------------------------------------------------------------------------------------------------
    class Object
    {
        static inline ObjectId s_idCounter = 0;

        std::vector<Component*> m_components;
        Scene* m_pScene;
        ObjectId m_objectId;
        bool m_isActive;
        bool m_isQueuedForDeletion;

    public:
        explicit Object(Scene* pScene);
        ~Object();

        bool PostLoadInit() const;
        void Destroy();

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
        ComponentType* AddComponent(ConstructorParams&&...params)
        {
            // Check to see if we already have the component on this Object.
            if (GetComponent<ComponentType>() != nullptr)
            {
                MCP_WARN("Object", "Failed to add Component! Tried to add ComponentType that already exists on Object!");
                return nullptr;
            }

            // Create the new component and call its LoadSceneData() function. This is how we are going to handle Component Dependencies.
            ComponentType* pNewComponent = BLEACH_NEW(ComponentType(this, params...));
            if (!pNewComponent->Init())
            {
                MCP_ERROR("Object", "Failed to add Component! Initialization of Component failed!");
                BLEACH_DELETE(pNewComponent);
                return nullptr;
            }

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
        void RemoveComponent()
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
        ComponentType* GetComponent()
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

        void SetActive(const bool isActive);

        [[nodiscard]] bool IsQueuedForDeletion() const { return m_isQueuedForDeletion; }
        [[nodiscard]] ObjectId GetId() const { return m_objectId; }
        [[nodiscard]] bool IsActive() const { return m_isActive; }
        [[nodiscard]] Scene* GetScene() const { return m_pScene; }
        [[nodiscard]] WorldLayer* GetWorld() const;
    };
}