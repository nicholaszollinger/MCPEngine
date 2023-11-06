#pragma once
// GameInstance.h

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : The Game Instance class can be overriden to include any important data that you want global in your game.
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
    };
}