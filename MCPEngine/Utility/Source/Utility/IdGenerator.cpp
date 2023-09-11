// IdGenerator.cpp
#include "IdGenerator.h"

#include <functional>
#include <string>

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Creates an Id by hashing the name of the Type. This is to be used in conjunction with a Macro, so that
///             we can define id's based on the 'stringified' name of the particular Type.  
//-----------------------------------------------------------------------------------------------------------------------------
uint64_t GenerateId(const char* pTypeName)
{
    return std::hash<std::string>{}(pTypeName);
} 