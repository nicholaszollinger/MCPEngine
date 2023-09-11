// IUpdateable.cpp

#include "IUpdateable.h"

namespace mcp
{
    IUpdateable::IUpdateable()
        : m_updateableId(s_idCounter++)
    {
        //
    }
}