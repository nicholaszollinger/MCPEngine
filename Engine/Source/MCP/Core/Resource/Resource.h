#pragma once
// Resource.h

namespace mcp
{
    struct ResourceLoadData
    {
        const char* pFilePath = nullptr;
        const char* pPackageName = nullptr;
        bool isPersistent = false;
    };

    class Resource
    {
    protected:
        ResourceLoadData m_loadData;
        void* m_pResource = nullptr;

    public:
        Resource() = default;
        virtual ~Resource() = default;

        Resource(const Resource& right) = default;
        Resource& operator=(const Resource& right) = default;
        Resource(Resource&& right) = default;
        Resource& operator=(Resource&& right) = default;

    public:
        virtual void Load(const char* pFilePath, const char* pPackageName = nullptr, const bool isPersistent = false) = 0;

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Get the raw resource ptr.
        //-----------------------------------------------------------------------------------------------------------------------------
        [[nodiscard]] virtual void* Get() const { return m_pResource; }

        //-----------------------------------------------------------------------------------------------------------------------------
        ///		@brief : Returns if the resource is loaded in memory.
        //-----------------------------------------------------------------------------------------------------------------------------
        [[nodiscard]] virtual bool IsValid() const {return m_pResource != nullptr; }

    protected:
        //-----------------------------------------------------------------------------------------------------------------------------
        //		NOTES:
        //      The implementation is going to be some templated call ResourceManager::Get()->FreeResource<SomeType>().
        //
        ///		@brief : How this resource is supposed to be freed.
        //-----------------------------------------------------------------------------------------------------------------------------
        virtual void Free() = 0;
    };
}