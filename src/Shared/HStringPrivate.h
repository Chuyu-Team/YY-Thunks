/*
 * PROJECT:   YY-Thunks
 * FILE:      hstring_private.h
 * PURPOSE:   Extra definitions for the Windows Runtime String (HSTRING)
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */
#pragma once
#include <hstring.h>

#include "SharedDefs.h"

/*
 * @remark The following definitions are dumped from the debug symbol from the
 *         Windows 10 Build 14347's combase.dll.
 */
namespace YY::Thunks::internal
{
    namespace
    {
        /*
         * @brief The flags of the Windows Runtime String.
         * @remark Originally it's a C/C++ enum in the debug symbols.
         */
        enum class RuntimeStringFlags : UINT32
        {
            WRHF_NONE = 0x00000000,
            WRHF_STRING_REFERENCE = 0x00000001,
            WRHF_VALID_UNICODE_FORMAT_INFO = 0x00000002,
            WRHF_WELL_FORMED_UNICODE = 0x00000004,
            WRHF_HAS_EMBEDDED_NULLS = 0x00000008,
            WRHF_EMBEDDED_NULLS_COMPUTED = 0x00000010,
            WRHF_RESERVED_FOR_PREALLOCATED_STRING_BUFFER = 0x80000000,
        };
        YY_APPLY_ENUM_CALSS_BIT_OPERATOR(RuntimeStringFlags)

        /*
         * @brief The internal structure of Windows Runtime String header.
         */
        typedef struct _HSTRING_HEADER_INTERNAL
        {
            RuntimeStringFlags Flags;
            UINT32 Length;
            UINT32 Padding1;
            UINT32 Padding2;
            PCWSTR StringRef;
        } HSTRING_HEADER_INTERNAL, * PHSTRING_HEADER_INTERNAL;
        static_assert(sizeof(HSTRING_HEADER) == sizeof(HSTRING_HEADER_INTERNAL));

        /*
         * @brief The internal structure of heap allocated Windows Runtime String.
         */
        typedef struct _STRING_OPAQUE
        {
            HSTRING_HEADER_INTERNAL Header;
            volatile LONG RefCount;
            WCHAR String[ANYSIZE_ARRAY];
        } STRING_OPAQUE, * PSTRING_OPAQUE;
    }
}
