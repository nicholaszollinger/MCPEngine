#pragma once
// GameInstance.h
#include "MCP/Core/TypeFactory.h"

namespace mcp
{
    class GameInstance;
    using GameInstanceFactory = TypeFactory<GameInstance>;

#define MCP_REGISTER_GAME_INSTANCE(typename) MCP_REGISTER_TYPE(GameInstanceFactory, typename)

    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : The Game Instance class can be derived from to include any persistent data that you want global in your game.
    //-----------------------------------------------------------------------------------------------------------------------------
    class GameInstance
    {
    public:
        GameInstance() = default;
        virtual ~GameInstance() = default;
        GameInstance(const GameInstance& right) = delete;
        GameInstance(GameInstance&& right) = delete;
        GameInstance& operator=(const GameInstance& right) = delete;
        GameInstance& operator=(GameInstance&& right) = delete;

        virtual TypeId GetTypeId() const = 0;
    };
}