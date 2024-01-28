#pragma once
// UnorderedDenseArray.h

#include <cassert>
#include <unordered_map>
#include <vector>
#include "Utility/Logging/Log.h"

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES
//
///		@brief : A Dense array is a std::vector whose order does not matter, and whose values are accessed through a keyValue.
///             Under the hood we have 2 std::unordered_maps that take care of mapping the values to indexes in the array and
///             vice versa. On removing an item from the array, the last element is swapped into the place of the removal
///             and the mappings are updated accordingly.
///
///		@tparam KeyType : Key to access values from.
///		@tparam ValueType : Value we are storing.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename KeyType, typename ValueType>
class UnorderedDenseArray
{
    std::vector<ValueType> m_array;
    std::unordered_map<KeyType, size_t> m_keyToIndexMap;
    std::unordered_map<size_t, KeyType> m_indexToKeyMap;

public:
    UnorderedDenseArray(const size_t reserveSize)
    {
        // Reserve space in each of our containers.
        m_array.reserve(reserveSize);
        m_keyToIndexMap.reserve(reserveSize);
        m_indexToKeyMap.reserve(reserveSize);
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : 
    ///		@returns : 
    //-----------------------------------------------------------------------------------------------------------------------------
    ValueType* Add(const KeyType& key)
    {
        const size_t newIndex = m_array.size();
        m_keyToIndexMap[key] = newIndex;
        m_indexToKeyMap[newIndex] = key;

        auto& valueRef = m_array.emplace_back();
        return &valueRef;
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //
    ///		@brief : Add a previously made ValueType to the array and map it to the Key.
    //-----------------------------------------------------------------------------------------------------------------------------
    void Add(const KeyType& key, const ValueType& value)
    {
        if (const auto result = m_keyToIndexMap.find(key); result != m_keyToIndexMap.end())
            return;

        const size_t newIndex = m_array.size();
        m_keyToIndexMap[key] = newIndex;
        m_indexToKeyMap[newIndex] = key;

        m_array.push_back(value);
    }

    void Remove(const KeyType& key)
    {
        const auto result = m_keyToIndexMap.find(key);
        if (result == m_keyToIndexMap.end())
        {
            _WARN("UnorderedDenseArray", "Attempting to remove a value that doesn't exist on with key: ", key);
            return;
        }

        const size_t indexOfValueToRemove = result->second;
        const size_t indexOfBack = m_array.size() - 1;

        // Swap the 'deleted' component with the index of the last element.
        std::swap(m_array[indexOfValueToRemove], m_array.back());

        // Update the mappings.
        const KeyType swappedKey = m_indexToKeyMap[indexOfBack];    // Get the Key we swapped into the removed space
        m_keyToIndexMap[swappedKey] = indexOfValueToRemove;         // Set that Key's index to be the index of the Value we removed.
        m_indexToKeyMap[indexOfValueToRemove] = swappedKey;         // Set the Key at the removed index to the be the Key we swapped.

        // Erase the removed data.
        m_indexToKeyMap.erase(indexOfBack);
        m_keyToIndexMap.erase(key);
        m_array.pop_back();
    }

    ValueType* Get(const KeyType& key)
    {
        const auto result = m_keyToIndexMap.find(key);
        if (result == m_keyToIndexMap.end())
        {
            _WARN("UnorderedDenseArray", "Attempting to get a value that doesn't exist on with key: ", key);
            return nullptr;
        }

        const size_t index = result->second;
        assert(index < m_array.size());         // Debug check to make sure that everything is working properly.
        return m_array[index];
    }

    [[nodiscard]] const std::vector<ValueType>& GetArray() const { return m_array; }

    // Sorting method???
};