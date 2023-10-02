#pragma once
// Component.h

#include "ComponentFactory.h"
#include "MCP/Core/Event/Message.h"

namespace mcp
{
    class Object;
    struct Message;
    class MessageManager;

    // TODO: Attribute to require other components:
    // This should take in what? The name of the components? Or their static Ids? <- That feels better.
    //#define MCP_REQUIRE_COMPONENTS(...)

    #define MCP_DEFINE_COMPONENT_DEFAULT_CONSTRUCTOR(ComponentName) ComponentName(Object* pObject) : Component(pObject) {}

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Generate a unique ComponentId based on the name of the ComponentType. This creates two getter functions to get
    ///         the Id of the Component, one is a virtual getter (GetTypeId()) for getting the id from a Component* instance, and one
    ///         is a static getter (GetStaticTypeId()) for getting the id from the type.
    ///		@param ComponentName : Name of the ComponentType. We want a type name not a string!
    //-----------------------------------------------------------------------------------------------------------------------------
#define MCP_DEFINE_COMPONENT_ID(ComponentName)                                                                                                          \
private:                                                                                                                                                \
    static inline const mcp::ComponentTypeId kComponentTypeId = mcp::ComponentFactory::RegisterComponentType<ComponentName>(#ComponentName);            \
                                                                                                                                                        \
public:                                                                                                                                                 \
    static mcp::ComponentTypeId GetStaticTypeId() { return kComponentTypeId; }                                                                          \
    virtual mcp::ComponentTypeId GetTypeId() const override { return kComponentTypeId; }                                                                \
private:                                                                                                                                                \

    class Component
    {
    protected:
        Object* m_pOwner;
        bool m_isActive;

    public:
        Component(Object* pObject);

        // TODO: Figure out what you want to do with copying and moving components.
        //      - The crucial part of the components is that they operate on
        //        a unique entity. So we can't copy the entity id over.

        virtual ~Component() = default;

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : Init is called right after this component is added to its Owner. Only components that have been attached
        ///             prior are guaranteed to be on the Owner. Use PostLoadInit() if you want to initialize some data that depends
        ///             on other components that might not be loaded yet or dependent on the scene.
        ///		@returns : True if the initialization was successful, false otherwise. 
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual bool Init() { return true; }

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : PostLoadInit is called once the Scene has completed loading.
        ///		@returns : True if the initialization was successful, false otherwise. 
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual bool PostLoadInit() { return true; }
        virtual void ReceiveMessage([[maybe_unused]] const Message& msg) {}

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //		
        ///		@brief : Called when the object as been queued for deletion.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void OnDestroy() {}

        virtual void SetIsActive(const bool isActive) { m_isActive = isActive; }

        [[nodiscard]] virtual ComponentTypeId GetTypeId() const = 0;
        [[nodiscard]] Object* GetOwner() const { return m_pOwner; }
        [[nodiscard]] MessageManager* GetMessageManager() const;
        [[nodiscard]] bool IsActive() const { return m_isActive; }

    protected:
        void SendMessage(const MessageId messageId);
        void ListenForMessage(const MessageId messageId);
        void StopListeningToMessage(const MessageId messageId) const;
    };
}
