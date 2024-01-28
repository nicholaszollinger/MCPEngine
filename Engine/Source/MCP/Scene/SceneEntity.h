#pragma once
// SceneEntity.h

#include <cstdint>
#include <vector>
#include "MCP/Core/Resource/Parsers/XMLParser.h"
#include "MCP/Lua/LuaResource.h"

namespace mcp
{
    class Scene;
    class SceneLayer;
    using EntityId = uint64_t;
    using LayerId = uint32_t;

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Defines the LayerId for the SceneEntity. A SceneEntity can only belong to one SceneLayer.
    ///		@param EntityTypename : Type name of the Entity. Note: Not a string, just the type name.
    //-----------------------------------------------------------------------------------------------------------------------------
#define MCP_DEFINE_SCENE_ENTITY(EntityTypename)                                                                          \
private:                                                                                                                 \
    static constexpr mcp::LayerId kLayerId = HashString32(#EntityTypename);                                              \
public:                                                                                                                  \
    [[nodiscard]] static constexpr mcp::LayerId GetStaticLayerId() { return kLayerId; }                                  \
    [[nodiscard]] virtual mcp::LayerId GetLayerId() const override { return kLayerId; }                                  \
private:

    //-----------------------------------------------------------------------------------------------------------------------------
    ///		@brief : Data necessary to construct a Scene Entity.
    //-----------------------------------------------------------------------------------------------------------------------------
    struct SceneEntityConstructionData
    {
#if MCP_EDITOR
        XMLElement root;  
#endif
        StringId tag;
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
        static constexpr StringId kInvalidTag = StringId();

        std::vector<SceneEntity*> m_children;
        SceneEntity* m_pParent;

#if MCP_EDITOR
        // This is the element in the scene or scene asset file that we were loaded from.
        XMLElement m_entityRoot;
        bool m_dirty;
#endif

    private:
        EntityId m_id;
        SceneLayer* m_pLayer = nullptr;
        const StringId m_tag = kInvalidTag;
        bool m_isActive = true;
        bool m_isQueuedForDeletion = false;

    public:
        SceneEntity();
        SceneEntity(const SceneEntityConstructionData& data);
        virtual ~SceneEntity() = default;

        // TODO:
        // For now, Copying a Scene Entity is not allowed.
        // This would be a lengthy operation. All children would have to created recursively
        SceneEntity(const SceneEntity& right) = delete;
        SceneEntity& operator=(const SceneEntity& right) = delete;

        // TODO: Implement the Move Ctor/Assignment
        SceneEntity(SceneEntity&& right) = delete;
        SceneEntity& operator=(SceneEntity&& right) = delete;

        // Parent/Child Functions
        void SetParent(SceneEntity* pEntity);
        void AddChild(SceneEntity* pEntity);
        void RemoveChild(SceneEntity* pEntity);
        virtual SceneEntity* GetChildByTag(const StringId tag) const;
        virtual SceneEntity* GetChildByTag(const StringId tag);
        [[nodiscard]] bool HasAParent() const { return m_pParent; }
        [[nodiscard]] bool HasChildren() const { return !m_children.empty(); }

        // Lifetime
        virtual bool Init() = 0;
        virtual bool PostLoadInit() = 0;
        void Destroy();
        [[nodiscard]] bool IsQueuedForDeletion() const { return m_isQueuedForDeletion; }

        // TODO: Messages
        //virtual void ReceiveMessage([[maybe_unused]] const Message& msg) {}

        // Active State
        void SetActive(const bool isActive);
        void ToggleActive();
        [[nodiscard]] bool IsActive() const;

        // Scene Info
        void SetLayer(SceneLayer* pLayer);
        [[nodiscard]] Scene* GetScene() const;
        [[nodiscard]] SceneLayer* GetLayer() const;
        [[nodiscard]] virtual LayerId GetLayerId() const = 0;

        // Entity Info
        [[nodiscard]] EntityId GetId() const { return m_id; }
        [[nodiscard]] StringId GetTag() const { return m_tag; }
        [[nodiscard]] virtual SceneEntity* GetParent() const { return m_pParent; }

        // Utils
        static SceneEntityConstructionData GetEntityConstructionData(const XMLElement element);
        template<typename EntityType> static EntityType* SafeCastEntity(SceneEntity* pEntity);
        template<typename EntityType> static const EntityType* SafeCastEntity(const SceneEntity* pEntity);

#if MCP_EDITOR
        // Saving
        virtual void Save() {}
        void MarkDirty() { m_dirty = true; }
        [[nodiscard]] bool IsDirty() const { return m_dirty; }

#endif
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

#if MCP_EDITOR
        // Saving
        //void MarkDirty();
#endif

    private:
        void OnParentActiveChanged(const bool parentActiveState);
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the SceneEntity cast to the child SceneEntity type. In debug, this will assert that the cast is valid.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename EntityType>
    EntityType* SceneEntity::SafeCastEntity(SceneEntity* pEntity)
    {
        MCP_CHECK(pEntity->GetLayerId() == EntityType::GetStaticLayerId());
        return static_cast<EntityType*>(pEntity);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Returns the SceneEntity cast to the child SceneEntity type. In debug, this will assert that the cast is valid.
    //-----------------------------------------------------------------------------------------------------------------------------
    template <typename EntityType>
    const EntityType* SceneEntity::SafeCastEntity(const SceneEntity* pEntity)
    {
        MCP_CHECK(pEntity->GetLayerId() == EntityType::GetStaticLayerId());
        return static_cast<const EntityType*>(pEntity);
    }

}
