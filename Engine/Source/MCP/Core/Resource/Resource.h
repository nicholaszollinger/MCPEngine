#pragma once
// Resource.h

#include "MCP/Debug/Assert.h"
#include "Utility/Generic/Hash.h"
#include "Utility/String/StringId.h"

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      The purpose of these ResourceRequests is that they are going to be used as the keys to our resource containers. For that
    //      they need to act as a Hash-able type and must be able to be equated. I previously just had resources tied to their filepath
    //      but that breaks down when resources like Fonts are introduced, not to mention other resources like Hardware resources.
    //      THIS SHOULD DEFINITELY BE A CONCEPT FOR WHEN WE UPDATE TO C++20!
    //
    //
    //      For DiskResources, the StringId system will have to be local to the Resource System, over using the global StringIds in the future.
    //		
    ///		@brief : Data required to load a resource of a specific type from disk. For most resources, this is just a filepath. If you need
    ///         more info (like a font size for Fonts), create a sub-struct adding the data you need, as well as a 'Hash'
    ///         operator and an equal operator.
    //-----------------------------------------------------------------------------------------------------------------------------
    struct DiskResourceRequest
    {
        StringId path {};
        StringId packagePath {};
        bool isPersistent = false;

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Hash function.
        //-----------------------------------------------------------------------------------------------------------------------------
        uint64_t operator()(const DiskResourceRequest& request) const
        {
            return std::hash<const void*>{}(request.path.GetConstPtr());
        }

        bool operator==(const DiskResourceRequest& right) const
        {
            return path == right.path;
        }
    };

#define MCP_DEFINE_RESOURCE_DESTRUCTOR(ResourceTypeName)      \
    virtual ~ResourceTypeName() override                      \
    {                                                         \
        if (m_pResource)                                      \
        {                                                     \
            Free();                                           \
            m_pResource = nullptr;                            \
        }                                                     \
    }

    template<typename ResourceRequestType>
    class Resource
    {
    protected:
        ResourceRequestType m_request;   // Data used to request the specific resource we want.
        void* m_pResource = nullptr;     // Raw pointer to the resource.

    public:
        Resource() = default;
        virtual ~Resource();

        Resource(const Resource& right) = default;
        Resource& operator=(const Resource& right) = default;
        Resource(Resource&& right) = default;
        Resource& operator=(Resource&& right) = default;
        
        [[nodiscard]] bool Load(const ResourceRequestType& request);
        [[nodiscard]] virtual void* Get() const { return m_pResource; }
        [[nodiscard]] virtual bool IsValid() const { return m_pResource; }

    protected:
        virtual void* LoadResourceType() = 0;
        virtual void Free() = 0;
    };

    template <typename ResourceRequestType>
    Resource<ResourceRequestType>::~Resource()
    {
        // We shouldn't have a valid resource at this point, it should be freed in the derived class destructor.
        MCP_CHECK_MSG(!m_pResource, "Failed to free Resource after destruction! You should be calling Free() in the overriden virtual destructor and setting it to nullptr!");
    }

    template <typename ResourceRequestType>
    bool Resource<ResourceRequestType>::Load(const ResourceRequestType& request)
    {
        if (m_pResource)
            Free();

        m_request = request;
        m_pResource = LoadResourceType();
        return m_pResource;
    }

    using DiskResource = Resource<DiskResourceRequest>;

    struct ResourceLoadData
    {
        const char* pFilePath = nullptr;
        const char* pPackageName = nullptr;
        bool isPersistent = false;
    };

    //class Resource
    //{
    //protected:
    //    ResourceLoadData m_loadData;
    //    void* m_pResource = nullptr;

    //public:
    //    Resource() = default;
    //    virtual ~Resource() = default;

    //    Resource(const Resource& right) = default;
    //    Resource& operator=(const Resource& right) = default;
    //    Resource(Resource&& right) = default;
    //    Resource& operator=(Resource&& right) = default;

    //public:
    //    virtual void Load(const char* pFilePath, const char* pPackageName = nullptr, const bool isPersistent = false) = 0;

    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    ///		@brief : Get the raw resource ptr.
    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    [[nodiscard]] virtual void* Get() const { return m_pResource; }

    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    ///		@brief : Returns if the resource is loaded in memory.
    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    [[nodiscard]] virtual bool IsValid() const {return m_pResource != nullptr; }

    //protected:
    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    //		NOTES:
    //    //      The implementation is going to be some templated call ResourceManager::Get()->FreeResource<SomeType>().
    //    //
    //    ///		@brief : How this resource is supposed to be freed.
    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    virtual void Free() = 0;
    //};
}
