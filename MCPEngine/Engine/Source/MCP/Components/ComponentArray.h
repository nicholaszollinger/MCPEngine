#pragma once
// ComponentArray.h

// TODO: ECS is for the summer!
//      - I didn't want to lose the work that I have done here.
//#include <cassert>
//#include <unordered_map>
//#include <vector>
//#include "MCP/Application/Debug/Log.h"
//#include "MCP/Entity/Entity.h"

namespace mcp
{

    //template<typename ComponentType>
    //class ComponentArray
    //{
    //    std::vector<ComponentType> m_components;
    //    std::unordered_map<Entity, size_t> m_entityToIndexMap;
    //    std::unordered_map<size_t, Entity> m_indexToEntityMap;

    //public:
    //    ComponentArray(const size_t reserveSize = kMaxEntities)
    //    {
    //        m_components.reserve(reserveSize);
    //        m_entityToIndexMap.reserve(reserveSize);
    //        m_indexToEntityMap.reserve(reserveSize);
    //    }

    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    //		NOTES:
    //    //
    //    ///		@brief : Constructs a Component of type ComponentType with default values and returns a pointer
    //    ///         to the newly added Component.
    //    ///         \n NOTE: Entities cannot have more than one type of Component!
    //    ///		@returns : Pointer to the newly added Component or nullptr if the Entity already had a component of that type.
    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    ComponentType* AddComponent(const Entity& entity)
    //    {
    //        // Check to see if that entity already has a component of that type:
    //        if (const auto result = m_entityToIndexMap.find(entity); result != m_entityToIndexMap.end())
    //        {
    //            // If they do, log a warning and exit.
    //            LogWarning("Failed to add a component! Entity: '%' already has a component of that type!", entity);
    //            return nullptr;
    //        }

    //        const size_t newIndex = m_components.size();
    //        m_entityToIndexMap[entity] = newIndex;
    //        m_indexToEntityMap[newIndex] = entity;

    //        auto& componentRef = m_components.emplace_back(entity);
    //        return &componentRef;
    //    }

    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    //		NOTES:
    //    //
    //    ///		@brief : Remove a component of ComponentType from the entity.
    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    void RemoveComponent(const Entity& entity)
    //    {
    //        const auto result = m_entityToIndexMap.find(entity);
    //        if (result == m_entityToIndexMap.end())
    //        {
    //            LogWarning("Attempting to remove a component that doesn't exist on Entity: '%'", entity);
    //            return;
    //        }

    //        const size_t indexOFComponentToRemove = result->second;
    //        const size_t indexOfBack = m_components.size() - 1;

    //        // Swap the 'deleted' component with the index of the last element.
    //        std::swap(m_components[indexOFComponentToRemove], m_components.back());

    //        // Update the mappings.
    //        const Entity swappedEntity = m_indexToEntityMap[indexOfBack];     // Get the Entity we swapped into the removed space
    //        m_entityToIndexMap[swappedEntity] = indexOFComponentToRemove;     // Set that entity's index to be the index of the component we removed.
    //        m_indexToEntityMap[indexOFComponentToRemove] = swappedEntity;     // Set the entity at the removed index to the be the entity we swapped.

    //        // Erase the removed data.
    //        m_entityToIndexMap.erase(entity);
    //        m_components.pop_back();
    //    }

    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    //		NOTES:
    //    //
    //    ///		@brief : Get the component associated with the entity.
    //    ///		@returns : Pointer to the entity's component, or nullptr if the entity didn't have a component of that type.
    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    ComponentType* GetComponent(const Entity& entity)
    //    {
    //        const auto result = m_entityToIndexMap.find(entity);
    //        if (result == m_entityToIndexMap.end())
    //        {
    //            LogWarning("Attempting to get a component that doesn't exist on Entity: '%'", entity);
    //            return nullptr;
    //        }

    //        const size_t index = result->second;    // Get the index of the entity's component.
    //        assert(index < m_components.size());  // Debug check to make sure that everything is working properly.
    //        return m_components[index];
    //    }

    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    //		NOTES:
    //    //
    //    ///		@brief : Get the array of components as a const reference.
    //    //-----------------------------------------------------------------------------------------------------------------------------
    //    const std::vector<ComponentType>& GetArray() const { return m_components; }
    //};

}