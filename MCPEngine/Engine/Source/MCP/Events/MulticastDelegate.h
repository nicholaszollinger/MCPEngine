#pragma once

#include <functional>
#include "MCP/Application/Debug/Log.h"

namespace mcp
{
    template<typename...Args>
    class MulticastDelegate
    {
    public:
        using Callback = std::function<void(Args...)>;

    private:
        std::unordered_map<void*, Callback> m_listeners;

    public:
        void AddListener(void* pOwner, const Callback& callback)
        {
            // Make sure that we don't already have callback from that owner.
            if (auto result = m_listeners.find(pOwner); result != m_listeners.end())
            {
                LogWarning("Attempted to add second callback to Delegate from the same owner.");
                return;
            }

            m_listeners.emplace(pOwner, callback);
        }

        void RemoveListener(void* pOwner)
        {
            if (auto result = m_listeners.find(pOwner); result != m_listeners.end())
            {
                m_listeners.erase(pOwner);
            }

            else
            {
                // TODO: Make a better warning message?
                LogWarning("Attempted to remove listener from Delegate that doesn't exist.");
            }
        }

        void Broadcast(Args...args)
        {
            for (auto& [pOwner, callback] : m_listeners)
            {
                callback(args...);
            }
        }
    };
}