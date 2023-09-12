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
#include "MCP/Core/GlobalManager.h"
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
    //-----------------------------------------------------------------------------------------------------------------------------
    template<typename ResourceType>
    class ResourceContainer
    {
        using ResourceMap = std::unordered_map<const char*, ResourcePtr<ResourceType>>;

    private:
        ResourceMap m_resources;

    public:
        ResourceContainer() = default;

        ~ResourceContainer()
        {
            for (auto& [pPath, pResourceData] : m_resources)
            {
                FreeResourceImpl(pResourceData.pResource);
            }
        }

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : If we already have a resource loaded that matches the filePath, then we will just add to the refCount and return
        ///             a pointer to the loaded asset. If we don't have it, then we will load the asset either from disk or from a loaded
        ///             package.
        ///		@tparam Args : Any type of arguments that are necessary.
        ///		@param pFilePath : Path to the asset in memory. Either directly on the disk or inside a package.
        ///		@param isPersistent : Whether the asset is going to stay in memory after the refCount hits zero.
        ///		@param pPackagePath : Path to the package that stores the asset. If this is nullptr, then we load from disk.
        ///		@param args : Any other arguments that are needed to load the asset.
        ///		@returns : Ptr to the Resource in memory.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename...Args>
        ResourceType* Add(const char* pFilePath, const bool isPersistent, const char* pPackagePath, Args&&...args)
        {
            auto* pResourceData = GetResourcePtr(pFilePath);

            // If we have the resource already, increase the refCount and return the resource.
            if (pResourceData)
            {
                pResourceData->refCount += 1;
                return pResourceData->pResource;
            }

            ResourceType* pResource = nullptr;

            // If we have a path, then it is intended that we use it.
            if (pPackagePath)
            {
                auto* pData = PackageManager::Get()->GetRawData(pPackagePath, pFilePath);
                if (!pData)
                {
                    return nullptr;
                }

                pResource = LoadFromRawDataImpl(pData->pData, pData->size, args...);
            }

            // Otherwise, load from disk.
            else
            {
                pResource = LoadFromDiskImpl(pFilePath, args...);
                if (!pResource)
                {
                    LogError("Failed to load Resource at filePath: '%'", pFilePath);
                    return nullptr;
                }
            }

            // Add a new Resource<ResourceType> to our ResourceMap.
            m_resources.emplace(std::make_pair(pFilePath, ResourcePtr<ResourceType>(pResource, isPersistent)));

            return pResource;
        }

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Remove a reference to this Resource. If the refCount to this Resource becomes 0, the Resource will be freed.
        //-----------------------------------------------------------------------------------------------------------------------------
        void RemoveRef(const char* pFilePath)
        {
            auto* pResourceData = GetResourcePtr(pFilePath);

            if (!pResourceData)
            {
                LogWarning("Tried to free a resource at filepath = '%' that wasn't in memory!", pFilePath);
                return;
            }

            pResourceData->refCount -= 1;

            // If our refCount is now 0 and it isn't marked as persistent,
            // then free the resource and erase it from our ResourceMap.
            if (pResourceData->refCount == 0 && !pResourceData->isPersistent)
            {
                FreeResourceImpl(pResourceData->pResource);
                m_resources.erase(pFilePath);
            }
        }

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
        ///		@tparam Args : Any type of arguments that you need.
        ///		@param pFilePath : Path to the asset in memory.
        ///		@param args : Any other parameters that you may need.
        ///		@returns : Ptr to the loaded resource, or nullptr if it fails.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename...Args>
        ResourceType* LoadFromDiskImpl(const char* pFilePath, Args&&...args);

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Load an asset of a certain ResourceType from a rawData in memory.
        ///		@tparam Args : Any type of arguments that you need.
        ///		@param pRawData : ptr to the array of raw bytes.
        ///		@param dataSize : size of the rawData.
        ///		@param args : Any other parameters that you may need.
        ///		@returns : Ptr to the loaded resource, or nullptr if it fails.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename...Args>
        ResourceType* LoadFromRawDataImpl(char* pRawData, const int dataSize, Args&&...args);

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Frees the resource from memory using a specific implementation.
        //-----------------------------------------------------------------------------------------------------------------------------
        void FreeResourceImpl(ResourceType*);

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Try to get the ResourcePtr from our ResourceMap at the pFilePath.
        ///		@returns : Pointer to the ResourcePtr, or nullptr if no ResourcePtr was found.
        //-----------------------------------------------------------------------------------------------------------------------------
        ResourcePtr<ResourceType>* GetResourcePtr(const char* pFilePath)
        {
            auto result = m_resources.find(pFilePath);
            if (result != m_resources.end())
            {
                return &((*result).second);
            }

            return nullptr;
        }
    };

    class ResourceManager final : public IProcess
    {
    private:
        // This is the asset zip file. In the future, this may look different. This is purpose build right now.
        static constexpr const char* kAssetDirectory = "AssetsPkg.zip";

        DEFINE_GLOBAL_MANAGER(ResourceManager)

    public:
        ResourceManager(const ResourceManager&) = delete;
        ResourceManager(ResourceManager&&) = delete;
        ResourceManager& operator=(const ResourceManager&) = delete;
        ResourceManager& operator=(ResourceManager&&) = delete;

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Load a resource.
        ///		@tparam ResourceType : Type of resource that we want to load. Ex: A texture.
        ///		@tparam Args : Any other parameters types that are required for the asset loading.
        ///		@param loadData : Data that defines how and where we load the asset from.
        ///		@param args : Other parameters that pertain to a specific type.
        ///		@returns : A pointer reference to the asset.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename ResourceType, typename ... Args>
        ResourceType* Load(const ResourceLoadData& loadData, Args&& ...args)
        {
            // Get a reference to the container that is associated with this type.
            auto& container = GetResourceContainer<ResourceType>();

            ResourceType* pResource = container.Add(loadData.pFilePath, loadData.isPersistent, loadData.pPackageName, args...);

            return pResource;
        }

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //
        ///		@brief : Remove a resource from memory, using the filepath as a key.
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename ResourceType>
        void FreeResource(const char* pFilePath)
        {
            // Get a reference to the container that is associated with this type.
            auto& container = GetResourceContainer<ResourceType>();
            container.RemoveRef(pFilePath);
        }

    private:
        ResourceManager() = default;

        virtual bool Init() override;
        virtual void Close() override;

        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //      This is a nifty trick for holding onto template data types. I don't want to be having a different ResourceManager
        //      for every kind of Resource imaginable, I just want one resource manager to be able to handle any kind of resource.
        //
        ///		@brief : Get the static container of loaded resources. The first time this is called for a ResourceType, a static
        ///          ResourceContainer is allocated.   
        //-----------------------------------------------------------------------------------------------------------------------------
        template<typename ResourceType>
        ResourceContainer<ResourceType>& GetResourceContainer()
        {
            static ResourceContainer<ResourceType> sContainer;
            return sContainer;
        }
    };
}


