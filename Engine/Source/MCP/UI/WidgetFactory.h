#pragma once
// WidgetFactory.h

#include <functional>
#include <unordered_map>
#include "MCP/Core/Resource/Parsers/XMLParser.h"
#include "MCP/Debug/Log.h"
#include "Utility/Generic/Hash.h"

namespace mcp
{
    class Widget;
    using WidgetTypeId = uint64_t;


    class WidgetFactory
    {
        using FactoryFunction = std::function<Widget*(XMLElement)>;
        using FactoryFuncContainer = std::unordered_map<WidgetTypeId, FactoryFunction>;

    public:
        static Widget* CreateWidgetFromData(const char* pWidgetTypeName, const XMLElement element)
        {
            const WidgetTypeId id = HashString64(pWidgetTypeName);
            FactoryFuncContainer& factoryFunctions = GetFactoryContainer();

            const auto result = factoryFunctions.find(id);
            if (result == factoryFunctions.end())
            {
                MCP_ERROR("WidgetFactory", "Failed to add ", pWidgetTypeName, " from data! No matching WidgetTypeId was found!");
                return nullptr;
            }

            return result->second(element);
        }

        template<typename WidgetType>
        static WidgetTypeId RegisterWidgetType(const char* pWidgetTypeName)
        {
            const WidgetTypeId id = HashString64(pWidgetTypeName);
            FactoryFuncContainer& factoryFunctions = GetFactoryContainer();

            // Check to see if we already have that id in our map:
            const auto result = factoryFunctions.find(id);
            MCP_CHECK_MSG(result == factoryFunctions.end(), "Failed to register Widget! We already had a Widget by that id registered. This could mean that we have two of the same name! WidgetType: ", pWidgetTypeName);

            factoryFunctions.emplace(id, [](const XMLElement element) -> WidgetType* { return WidgetType::AddFromData(element); });
            return id;
        }

    private:
        static FactoryFuncContainer& GetFactoryContainer()
        {
            static FactoryFuncContainer container;
            return container;
        }
    };
}