#pragma once
// ConceptTypes.h

#include <type_traits>

//-----------------------------------------------------------------------------------------------------------------------------
//  NOTES:
//  The goal here is to house all of my type constraints that I *would* use c++20 concepts for, but instead I will use some
//      horrendous looking TMP type stuff that I have to look up to try and decipher.
//      I will be posting sources to the research I find.
//  If I decide to transition to C++20 later, this can be updated to use actual concepts.
//
//  Sources:
//  - https://stackoverflow.com/questions/54468896/implementation-of-concepts-in-c17
//-----------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Creates a struct interface that can be used to see if a Type satisfies some condition.
///         \n  Example Declaration for an 'AddableToStringType':
///         \n  GENERATE_TYPE_CHECK(AddableToStringType, std::declval<std::string>() += std::declval<Type>());
///         \n\n Example usage: static_assert(AddableToStringType<Type>::value, "Type must be able to be added to a string
///              with the '+=' operator!");
///		@param TypeName : Name of the type you want to create.
///		@param Requirement : The compile time boolean requirement to evaluate.
//-----------------------------------------------------------------------------------------------------------------------------
#define GENERATE_TYPE_CHECK(TypeName, Requirement)                                            \
namespace ConceptTypesInternal                                                                \
{                                                                                             \
    template<typename Type, typename = void>                                                  \
    struct TypeName##Helper : std::false_type {};                                             \
                                                                                              \
    template<typename Type>                                                                   \
    struct TypeName##Helper<Type, std::void_t<decltype(Requirement)>> : std::true_type {};    \
}                                                                                             \
                                                                                              \
template<typename Type, typename = void>                                                      \
struct TypeName : std::false_type {};                                                         \
                                                                                              \
template<typename Type>                                                                       \
struct TypeName<Type> : ConceptTypesInternal::TypeName##Helper<Type> {}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : Creates a TMP interface that can be used to see if two types (LeftType and a RightType) satisfy
///             some condition. \n NOTE: This only checks the single permutation of: \n if (Condition(LeftType, RightType)) == true.
///
///             \n\n TODO Add example case!
///
///		@param CheckName : Name of the type check you want to perform.
///     @param LeftTypeName : Name you want to give the left type.
///     @param RightTypeName : Name you want to give the right type.
///		@param Requirement : The compile time boolean requirement to evaluate.
//-----------------------------------------------------------------------------------------------------------------------------
#define GENERATE_DEPENDENT_TYPE_CHECK(CheckName, LeftTypeName, RightTypeName, Requirement)                              \
namespace ConceptTypesInternal                                                                                          \
{                                                                                                                       \
    template <typename LeftTypeName, typename RightTypeName, typename = void>                                           \
    struct CheckName##Helper : std::false_type {};                                                                      \
                                                                                                                        \
    template <typename LeftTypeName, typename RightTypeName>                                                            \
    struct CheckName##Helper<LeftTypeName, RightTypeName, std::void_t<decltype(Requirement)>> : std::true_type {};      \
}                                                                                                                       \
                                                                                                                        \
template<typename LeftTypeName, typename RightTypeName, typename = void>                                                \
struct CheckName : std::false_type {};                                                                                  \
                                                                                                                        \
template<typename LeftTypeName, typename RightTypeName>                                                                 \
struct CheckName<LeftTypeName, RightTypeName> : ConceptTypesInternal::CheckName##Helper<LeftTypeName, RightTypeName> {}  


namespace ConceptTypesInternal
{
    template <typename LeftType, typename RightType, typename = void>
    struct MultipliableHelper : std::false_type {};

    template <typename LeftType, typename RightType>
    struct MultipliableHelper<LeftType, RightType, std::void_t<decltype(std::declval<LeftType>()* std::declval<RightType>())>> : std::true_type {};
}

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : DO NOT USE THIS TYPE TO CHECK.\n Use TypesAreMultipliable<LeftType, RightType> -> 2 TemplateParams only.
//-----------------------------------------------------------------------------------------------------------------------------
template<typename LeftType, typename RightType, typename = void>
struct TypesAreMultipliable : std::false_type {};

//-----------------------------------------------------------------------------------------------------------------------------
//		NOTES:
//
///		@brief : On construction, 'returns a boolean value' (std::true_type or std::false_type) on whether the value 'LeftType'
///              can be multiplied with 'RightType'. \n This will return 'true' if either ordering works, meaning (LeftType * RightType)
///              OR (RightType * LeftType)
///             \n\n EXAMPLE USAGE:
///             \n "static_assert(TypesAreMultipliable<ValueType, float>::value, "ValueType must be multipliable with a float value!");"\n
///		@tparam LeftType : The type that we are checking.
///		@tparam RightType : The type that you want to make sure can be multiplied with 'LeftType'
//-----------------------------------------------------------------------------------------------------------------------------
template<typename LeftType, typename RightType>
struct TypesAreMultipliable<LeftType, RightType> :
    std::conditional_t<ConceptTypesInternal::MultipliableHelper<LeftType, RightType>::value || ConceptTypesInternal::MultipliableHelper<RightType, LeftType>::value
    , std::true_type
    , std::false_type>
{};