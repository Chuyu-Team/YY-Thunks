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

#define __Version(Major, Minor, Build, Revision) (Major * 0x1000000000000ull + Minor * 0x100000000ull + Build * 0x10000ull + Revision)

// 版本号参考来源：
// https://github.com/MouriNaruto/MouriDocs/tree/main/docs/18

// Windows 2000 RTM x86
#define __WindowsNT5 __Version(5, 0, 2195, 0)

// Windows XP RTM x86
#define __WindowsNT5_1 __Version(5, 1, 2600, 0)

#define __WindowsNT5_1_SP1 __Version(5, 1, 2600, 1106)

#define __WindowsNT5_1_SP2 __Version(5, 1, 2600, 2180)

#define __WindowsNT5_1_SP3 __Version(5, 1, 2600, 5512)

// Windows 2003 RTM x86
#define __WindowsNT5_2 __Version(5, 2, 3790, 0)

// Windows XP RTM x64，Windows 2003 SP1
#define __WindowsNT5_2_SP1 __Version(5, 2, 3790, 1830)

// Windows Vista RTM
#define __WindowsNT6 __Version(6, 0, 6000, 0)

// Windows Vista SP1、Windows Server 2008 RTM
#define __WindowsNT6_SP1 __Version(6, 0, 6001, 0)

#define __WindowsNT6_SP2 __Version(6, 0, 6002, 0)

// Windows 7 RTM, Windows Server 2008 R2 RTM
#define __WindowsNT6_1 __Version(6, 1, 7600, 0)

#define __WindowsNT6_1_SP1 __Version(6, 1, 7601, 0)

// Windows 8 RTM, Windows Server 2012 RTM
#define __WindowsNT6_2 __Version(6, 2, 9200, 0)

// Windows 8.1 RTM, Windows Server 2012 R2 RTM
#define __WindowsNT6_3 __Version(6, 3, 9600, 0)

// Windows 10 1507
#define __WindowsNT10_10240 __Version(10, 0, 10240, 0)

// Windows 10 1607(RS1)
#define __WindowsNT10_14393 __Version(10, 0, 14393, 0)

// Windows 10 1703(RS2)
#define __WindowsNT10_15063 __Version(10, 0, 15063, 0)

// Windows 10 1709(RS3)，注意ARM64从这个版本开始才支持。
#define __WindowsNT10_16299 __Version(10, 0, 16299, 0)

// Windows 10 1803(RS4)
#define __WindowsNT10_17134 __Version(10, 0, 17134, 0)

// Windows 10 2004(VB)
#define __WindowsNT10_19041 __Version(10, 0, 19041, 0)

// Windows Server 2022(FE)
#define __WindowsNT10_20348 __Version(10, 0, 20348, 0)

#if defined(_M_IX86)
#define __WindowsMinTarget __WindowsNT5
#elif defined(_M_AMD64)
#define __WindowsMinTarget __WindowsNT5_2_SP1
#elif defined(_M_ARM)
#define __WindowsMinTarget __WindowsNT6_2
#elif defined(_M_ARM64)
#define __WindowsMinTarget __WindowsNT10_16299
#endif

#ifndef YY_Thunks_Target
#define YY_Thunks_Target __WindowsMinTarget
#endif

// 输错了版本号？
#if YY_Thunks_Target < __WindowsMinTarget
#undef YY_Thunks_Target
#define YY_Thunks_Target __WindowsMinTarget
#endif

#if defined(_M_IX86)
// __ftoul2_legacy v143新增
#pragma comment(linker, "/alternatename:__ftoul2_legacy=__ftol2")
#endif
