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
private:                                                                                                                                                
    //-----------------------------------------------------------------------------------------------------------------------------

    struct ComponentConstructionData
    {
        bool startActive = true;
    };

    class Component
    {
        friend class Object;
        Object* m_pOwner;

    protected:
        bool m_isActive;

    public:
        Component(const bool startActive);
        Component(const ComponentConstructionData& data);

        // TODO: I defaulted to just removing the ability to copy/move/assign components.
        // There wasn't a need for now. But this is definitely something I want to be able to do in the future.
        
        Component(const Component& right) = delete;
        Component& operator=(const Component& right) = delete;
        Component(Component&& right) = delete;
        Component& operator=(Component&& right) = delete;
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

        void SetActive(const bool isActive);

        [[nodiscard]] virtual ComponentTypeId GetTypeId() const = 0;
        [[nodiscard]] Object* GetOwner() const { return m_pOwner; }
        [[nodiscard]] MessageManager* GetMessageManager() const;
        [[nodiscard]] bool IsActive() const;

    protected:
        void SendMessage(const MessageId messageId);
        void ListenForMessage(const MessageId messageId);
        void StopListeningToMessage(const MessageId messageId) const;
        virtual void OnActive() {}
        virtual void OnInactive() {}
        virtual void OnOwnerParentSet([[maybe_unused]] Object* pParent) {}

        static ComponentConstructionData GetComponentConstructionData(const XMLElement element);

    private:
        void OnOwnerActiveChanged(const bool objectActive);
    };
}
