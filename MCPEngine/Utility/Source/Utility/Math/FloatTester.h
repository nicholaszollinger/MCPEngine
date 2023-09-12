#pragma once

// C++ 20 code that is being removed for this project.
//#include <concepts>
//
//template<typename Type>
//concept FloatingPointType = std::is_floating_point_v<Type>;

//-----------------------------------------------------------------------------------------------------------------------------
///		@brief : Default value to compare the 'equalness' of two floats.
//-----------------------------------------------------------------------------------------------------------------------------
constexpr float kDefaultCloseEnough = 0.001f;

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Check to see if two floating point values are 'approximately' equal.
///		@tparam Type : Floating point type.
///     @param left : left hand value.
///     @param right: right hand value.
///     @param closeEnough : How close do the floats need to be to each other to be considered equal? Default is 'kDefaultCloseEnough'
///		@returns : True if the left and right floating point values are 'approximately' the same.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename Type>
bool CheckEqualFloats(const Type left, const Type right, const float closeEnough = kDefaultCloseEnough)
{
    static_assert(std::is_floating_point_v<Type> && "Type must be a floating point type!");

    // I cast the 'close enough' value to match the type, that way we don't have warnings for float vs double, etc.
    const Type closeConversion = static_cast<Type>(closeEnough);
    return std::abs(left - right) < closeConversion || std::abs(right - left) < closeConversion;
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//      THIS WAS COMMENTED OUT BECAUSE I DON'T KNOW IF I WOULD EVER USE IT.
//              I am leaving it here just in case I want it later. It needs to be downgraded to C++17.
//      I may have an issue when discerning between the arguments and the close enough value.
//
///		@brief : Check to see if more than two floating point values are all 'approximately' equal.
///		@tparam Args : Floating point parameter pack of values.
///     @param args : Values to check their
///     @param closeEnough : How close do the floats need to be to each other to be considered equal? Default is 0.001f.
///		@returns : True if all of the floating point values are 'approximately' equal.
//-----------------------------------------------------------------------------------------------------------------------------
//template<FloatingPointType ...Args>
//bool CheckEqualFloats(const Args... args, const float closeEnough = kDefaultCloseEnough)
//{
//    const size_t size = sizeof...(args);
//
//    std::array<std::common_type_t<Args...>, size> testArray{ (args)... };
//    auto check = testArray[0];
//
//    auto checkEqual = [check, closeEnough](const auto val)->bool
//    {
//        return CheckEqualFloats(check, val, closeEnough);
//    };
//
//    for (size_t i = 0; i < size; ++i)
//    {
//        const bool result = checkEqual(testArray[i]);
//        if (!result)
//            return false;
//    }
//
//    return true;
//}
