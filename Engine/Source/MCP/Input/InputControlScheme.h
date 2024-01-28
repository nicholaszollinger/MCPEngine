#pragma once
// InputControlScheme.h
// TODO: I didn't have time to complete the InputSystem the way I wanted.

#include "InputAction.h"
#include "InputControl.h"

namespace mcp
{
    //-----------------------------------------------------------------------------------------------------------------------------
    //		NOTES:
    //		
    ///		@brief : A Container of all Mappings for a control scheme. 
    //-----------------------------------------------------------------------------------------------------------------------------
    class InputControlScheme
    {
        std::unordered_map<InputControl, const InputAction*> m_mappings;

    public:
        //void MapControl(const InputAction* pAction, const Control control);
        //void UnMapControl(const InputAction* pAction, const Control control);
    };
}