// Path.cpp

#include "Path.h"

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//		
///		@brief : Checks to see if the path's extension matches the passed in val.
//-----------------------------------------------------------------------------------------------------------------------------
bool Path::ExtensionMatches(const char* extension) const
{
    if (!extension)
        return false;

    return m_val.find_last_of(extension) != std::string::npos;
}
