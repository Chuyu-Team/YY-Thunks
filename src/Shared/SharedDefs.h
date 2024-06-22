#pragma once
#include <type_traits>

#define YY_APPLY_ENUM_CALSS_BIT_OPERATOR(_ENUM)                      \
    inline constexpr _ENUM& operator|=(_ENUM& _eLeft, _ENUM _eRight) \
    {                                                                \
        using _Type = std::underlying_type<_ENUM>::type;             \
        (_Type&)_eLeft |= (_Type)_eRight;                            \
        return _eLeft;                                               \
    }                                                                \
                                                                     \
    inline constexpr _ENUM operator|(_ENUM _eLeft, _ENUM _eRight)    \
    {                                                                \
        using _Type = std::underlying_type<_ENUM>::type;             \
        auto _Result = (_Type)_eLeft | (_Type)_eRight;               \
        return _ENUM(_Result);                                       \
    }                                                                \
                                                                     \
    inline constexpr _ENUM operator&(_ENUM _eLeft, _ENUM _eRight)    \
    {                                                                \
        using _Type = std::underlying_type<_ENUM>::type;             \
        return _ENUM((_Type)_eLeft & (_Type)_eRight);                \
    }                                                                \
                                                                     \
    inline constexpr _ENUM operator~(_ENUM _eLeft)                   \
    {                                                                \
        using _Type = std::underlying_type<_ENUM>::type;             \
        return _ENUM(~(_Type)_eLeft);                                \
    }                                                                \
                                                                     \
    inline constexpr bool HasFlags(_ENUM _eLeft, _ENUM _eRight)      \
    {                                                                \
        using _Type = std::underlying_type<_ENUM>::type;             \
        return (_Type)_eLeft & (_Type)_eRight;                       \
    }
