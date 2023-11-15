#pragma once

// ResourceManager.h

//-----------------------------------------------------------------------------------------------------------------------------
// Goal: This is going to be a singleton instance class, which holds references to each of the loaded
//      assets. In my 275 project, I had the GraphicsManager hold onto graphics assets. Here, I want to
//      have an interface for just loading and unloading any kind of asset.
//
//-----------------------------------------------------------------------------------------------------------------------------

#include <unordered_map>
#include "PackageManager.h"
#include "Resource.h"
#include "MCP/Core/System.h"
#include "MCP/Debug/Log.h"

namespace mcp
{
    struct DataHeader;

    template<typename ResourceType>
    struct ResourcePtr
    {
        ResourcePtr(ResourceType* pResource)
            : pResource(pResource)
            , refCount(1)
            , isPersistent(false)
        {
            //
        }

        ResourcePtr(ResourceType* pResource, const bool isPersistent)
            : pResource(pResource)
            , refCount(1)
            , isPersistent(isPersistent)
        {
            //
        }

        ResourceType* pResource;    // The resource in memory.
        uint32_t refCount;          // How many references to this resource are in the program.
        bool isPersistent;          // Persistent means the resource will stay in memory even when the refCount goes to zero.
                                    // Can be useful if the refCount can routinely go to zero.
    };

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : This is the implementation details of the ResourceManager. Defining the functions LoadFromDiskImpl() and
    ///         FreeResourceImpl() is required to successfully load and unload assets of a certain type.
    ///		@tparam ResourceType :
    ///     @tparam RequestType : Type that we use to determine if the Resource exists in our container or not. This contains data
    ///             that is required to find the correct resource in memory. Could just be a filepath, or something more.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ResourceType, typename RequestType>
    class ResourceContainer
    {
        using ResourceMap = std::unordered_map<RequestType, ResourcePtr<ResourceType>, RequestType>;
        ResourceMap m_resources;

    public:
        ResourceContainer() = default;
        ~ResourceContainer();

        ResourceType* AddFromDisk(const RequestType& request);
        void RemoveRef(const RequestType& request);

    private:
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //      Interestingly enough, I can not have the function definition here, I can use declare and then in a
        //      cpp somewhere, I can define it. I get the underline saying that it is mad, but 
        //
        ///		@brief : Load an asset of a certain ResourceType.
        ///         \n Example of Definition for an SDL_Texture:
        ///         \n\n template <>         <- For the ResourceType
        ///         \n template <>         <- For the Args
        ///         \n SDL_Texture* ResourceContainer<SDL_Texture>::LoadFromDiskImpl(const char* pFilePath, void* pRenderer, Vec2Int& sizeOut) {}
        ///		@param request : The request data for this resource.
        ///		@returns : Ptr to the loaded resource, or nullptr if it fails.
        //-----------------------------------------------------------------------------------------------------------------------------
        ResourceType* LoadFromDiskImpl(const RequestType& request);

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Load an asset of a certain ResourceType from a rawData in memory.
        ///		@param pRawData : ptr to the array of raw bytes.
        ///		@param dataSize : size of the rawData.
        ///		@param request : The request data for this resource.
        ///		@returns : Ptr to the loaded resource, or nullptr if it fails.
        //-----------------------------------------------------------------------------------------------------------------------------
        ResourceType* LoadFromRawDataImpl(char* pRawData, const int dataSize, const RequestType& request);

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Frees the resource from memory using a specific implementation.
        //-----------------------------------------------------------------------------------------------------------------------------
        void FreeResourceImpl(ResourceType*);
        
        ResourcePtr<ResourceType>* GetResourcePtr(const RequestType& key);
    };

    class ResourceManager final : public System
    {
    private:
        // This is the asset zip file. In the future, this may look different. This is purpose build right now.
        static constexpr const char* kAssetDirectory = "AssetsPkg.zip";

        MCP_DEFINE_SYSTEM(ResourceManager)

    public:
        template<typename ResourceType, typename DiskRequestType>
        ResourceType* LoadFromDisk(const DiskRequestType& request);

        template<typename ResourceType, typename RequestType>
        void FreeResource(const RequestType& request);

        static ResourceManager* Get();
        static ResourceManager* AddFromData(const XMLElement) { return BLEACH_NEW(ResourceManager); }

    private:
        virtual bool Init() override;
        virtual void Close() override;
        
        template<typename ResourceType, typename RequestType>
        ResourceContainer<ResourceType, RequestType>& GetResourceContainer();
    };

    template<typename ResourceType, typename RequestType>
    ResourceContainer<ResourceType, RequestType>::~ResourceContainer()
    {
        for (auto& [pPath, pResourceData] : m_resources)
        {
            FreeResourceImpl(pResourceData.pResource);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Add a new Resource using the request data. If we already loaded the resource from an equal request, then we
    ///         will return the previously loaded resource.
    ///		@param request : The data that is necessary for loading the Resource.
    ///		@returns : Ptr to the Resource in memory, or nullptr if the load failed.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ResourceType, typename RequestType>
    ResourceType* ResourceContainer<ResourceType, RequestType>::AddFromDisk(const RequestType& request)
    {
        static_assert(std::is_convertible_v<RequestType, DiskResourceRequest>, "RequestType must be convertible to DiskResourceRequest!");

        ResourcePtr<ResourceType>* pResourcePtr = GetResourcePtr(request);

        // If we have the resource already, increase the refCount and return the resource.
        if (pResourcePtr)
        {
            pResourcePtr->refCount += 1;
            return pResourcePtr->pResource;
        }

        ResourceType* pResource = nullptr;

        // If we have a path, then it is intended that we use it.
        if (request.packagePath.IsValid())
        {
            auto* pData = PackageManager::Get()->GetRawData(request.packagePath.GetCStr(), request.path.GetCStr());
            if (!pData)
            {
                return nullptr;
            }

            pResource = LoadFromRawDataImpl(pData->pData, pData->size, request);
        }

        // Otherwise, load from disk.
        else
        {
            pResource = LoadFromDiskImpl(request);
            if (!pResource)
            {
                MCP_ERROR("ResourceManager", "Failed to load Resource at filePath: ", request.path.GetCStr());
                return nullptr;
            }
        }

        // Add a new Resource<ResourceType> to our ResourceMap.
        m_resources.emplace(std::make_pair(request, ResourcePtr<ResourceType>(pResource, request.isPersistent)));

        return pResource;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Remove a reference to a resource that was loaded with the Request. If the refCount to this Resource becomes 0,
    ///         the Resource will be freed.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ResourceType, typename RequestType>
    void ResourceContainer<ResourceType, RequestType>::RemoveRef(const RequestType& request)
    {
        auto* pResourcePtr = GetResourcePtr(request);

        if (!pResourcePtr)
        {
            MCP_WARN("ResourceContainer", "Tried to free a resource at filepath = '", *request.path, "' that wasn't in memory!");
            return;
        }

        pResourcePtr->refCount -= 1;

        // If our refCount is now 0 and it isn't marked as persistent,
        // then free the resource and erase it from our ResourceMap.
        if (pResourcePtr->refCount == 0 && !pResourcePtr->isPersistent)
        {
            FreeResourceImpl(pResourcePtr->pResource);
            m_resources.erase(request);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Try to get the ResourcePtr from our ResourceMap at the pFilePath.
    ///		@returns : Pointer to the ResourcePtr, or nullptr if no ResourcePtr was found.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ResourceType, typename RequestType>
    ResourcePtr<ResourceType>* ResourceContainer<ResourceType, RequestType>::GetResourcePtr(const RequestType& key)
    {
        auto result = m_resources.find(key);
        if (result != m_resources.end())
        {
            return &((*result).second);
        }

        return nullptr;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : Load a resource from Disk.
    ///		@tparam ResourceType : Type of resource we are loading.
    ///		@tparam DiskRequestType : Type of request needed to load the resource.
    ///		@param request : Data required to load the resource that we want.
    ///		@returns : Pointer to the resource, or nullptr if it failed.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ResourceType, typename DiskRequestType>
    ResourceType* ResourceManager::LoadFromDisk(const DiskRequestType& request)
    {
        static_assert(std::is_convertible_v<DiskRequestType, DiskResourceRequest>, "RequestType must be convertible to DiskResourceRequest!");
        // Get a reference to the container that is associated with this type.
        auto& container = GetResourceContainer<ResourceType, DiskRequestType>();

        ResourceType* pResource = container.AddFromDisk(request);

        return pResource;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Remove a resource from memory, using the filepath as a key.
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ResourceType, typename RequestType>
    void ResourceManager::FreeResource(const RequestType& request)
    {
        // Get a reference to the container that is associated with this type.
        auto& container = GetResourceContainer<ResourceType, RequestType>();
        container.RemoveRef(request);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //      This is a nifty trick for holding onto template data types. I don't want to be having a different ResourceManager
    //      for every kind of Resource imaginable, I just want one resource manager to be able to handle any kind of resource.
    //
    ///		@brief : Get the static container of loaded resources. The first time this is called for a ResourceType, a static
    ///          ResourceContainer is allocated.   
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ResourceType, typename RequestType>
    ResourceContainer<ResourceType, RequestType>& ResourceManager::GetResourceContainer()
    {
        static ResourceContainer<ResourceType, RequestType> sContainer;
        return sContainer;
    }
}


