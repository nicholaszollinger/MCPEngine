#pragma once
// AudioData.h

#include "Utility/Types/Vector2.h"

//-----------------------------------------------------------------------------------------------------------------------------
//  NOTE: The choice of Data here is based on when I was using SDL. I may need to rethink this.
//
//-----------------------------------------------------------------------------------------------------------------------------

namespace mcp
{
    struct AudioData
    {
        AudioData()
            : pPlaybackTarget(nullptr)
            , volume(128)
            , loops(0)
            , channel(-1)
            , playOnLoad(false)
        {
            //
        }

        Vec2Int* pPlaybackTarget;   // Where we are playing the Audio from.
        int volume;                 // Min=0, Max=128. Default is 128.
        int loops;                  // Default is 0, meaning it doesn't loop. -1 means to loop indefinitely.
        int channel;                // What channel is this piece of Audio being played on? Default is -1, which tells the AudioManager to play on any open channel.
        bool playOnLoad;            // Determines whether this will attempt to play OnSceneLoad(). Default is false.
    };
}