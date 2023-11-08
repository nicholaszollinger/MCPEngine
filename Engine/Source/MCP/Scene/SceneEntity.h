#pragma once
// SceneEntity.h

#include <cstdint>
#include <vector>
#include "MCP/Core/Resource/Parsers/XMLParser.h"

namespace mcp
{
    class Scene;
    class SceneLayer;
    using EntityId = uint64_t;

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      TODO:
    //      I want to register a LoadFromData() function (the LoadObject function) that determines how this scene asset is loaded.
    //      I also want it to define the Layer it will be on.
    //		
    ///		@brief : 
    ///		@param EntityTypename : 
    //-----------------------------------------------------------------------------------------------------------------------------
#define MCP_DEFINE_SCENE_ENTITY(EntityTypename, sceneLayerType)                                                              \
    private:                                                                                                                 \
        

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Data necessary to construct a Scene Entity.
    //-----------------------------------------------------------------------------------------------------------------------------
    struct SceneEntityConstructionData
    {
        // Tag?
        bool startActive = true;
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : An entity is the base class object that can be in a Scene.
    //-----------------------------------------------------------------------------------------------------------------------------
    class SceneEntity
    {
        static inline EntityId s_idCounter = 0;

    protected:
        std::vector<SceneEntity*> m_children;

    private:
        EntityId m_id;
        SceneEntity* m_pParent;
        Scene* m_pScene = nullptr;
        // const StringId m_tag; POTENTIALLY. I haven't decided yet.
        bool m_isActive = true;
        bool m_isQueuedForDeletion = false;

    public:
        SceneEntity(const SceneEntityConstructionData& data);
        virtual ~SceneEntity() = default;
        // TODO:Decide what to do when copying and moving.

        // Parent/Child relationship
        void SetParent(SceneEntity* pEntity);
        void AddChild(SceneEntity* pEntity);
        void RemoveChild(SceneEntity* pEntity);

        // Lifetime
        virtual bool Init() = 0;
        virtual bool PostLoadInit() = 0;
        void Destroy();
        void DestroyEntityAndChildren();
        [[nodiscard]] bool IsQueuedForDeletion() const { return m_isQueuedForDeletion; }

        // TODO: Messages
        //virtual void ReceiveMessage([[maybe_unused]] const Message& msg) {}

        // Active State
        virtual void SetActive(const bool isActive);
        void ToggleActive();
        [[nodiscard]] bool IsActive() const;

        static SceneEntityConstructionData GetEntityConstructionData(const XMLElement element);

    protected:
        virtual void OnDestroy() = 0;
        virtual void OnActive() = 0;
        virtual void OnInactive() = 0;
        virtual void OnParentSet() = 0;
        virtual void OnChildAdded(SceneEntity* pChild) = 0;
        virtual void OnChildRemoved(SceneEntity* pChild) = 0;

        // TODO: Messages
        /*void SendMessage(const MessageId messageId);
        void ListenForMessage(const MessageId messageId);
        void StopListeningToMessage(const MessageId messageId);*/

    private:
        void OnParentActiveChanged(const bool parentActiveState);
    };

    
}
