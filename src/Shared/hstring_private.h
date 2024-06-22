/*
 * PROJECT:   YY-Thunks
 * FILE:      hstring_private.h
 * PURPOSE:   Extra definitions for the Windows Runtime String (HSTRING)
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef YY_THUNKS_HSTRING_PRIVATE
#define YY_THUNKS_HSTRING_PRIVATE

#include <hstring.h>

/*
 * @remark The following definitions are dumped from the debug symbol from the
 *         Windows 10 Build 14347's combase.dll.
 */

/*
 * @brief The flags of the Windows Runtime String.
 * @remark Originally it's a C/C++ enum in the debug symbols. But we changed the
 *         definition to macro for reducing the C++ implicit cast operations to
 *         make both compiler and maintainer happily.
 */
#ifndef WINDOWS_RUNTIME_HSTRING_FLAGS
#define WINDOWS_RUNTIME_HSTRING_FLAGS

#define WRHF_NONE 0x00000000
#define WRHF_STRING_REFERENCE 0x00000001
#define WRHF_VALID_UNICODE_FORMAT_INFO 0x00000002
#define WRHF_WELL_FORMED_UNICODE 0x00000004
#define WRHF_HAS_EMBEDDED_NULLS 0x00000008
#define WRHF_EMBEDDED_NULLS_COMPUTED 0x00000010
#define WRHF_RESERVED_FOR_PREALLOCATED_STRING_BUFFER 0x80000000

#endif // !WINDOWS_RUNTIME_HSTRING_FLAGS

/*
 * @brief The internal structure of Windows Runtime String header.
 */
typedef struct _HSTRING_HEADER_INTERNAL
{
    UINT32 Flags;
    UINT32 Length;
    UINT32 Padding1;
    UINT32 Padding2;
    PCWSTR StringRef;
} HSTRING_HEADER_INTERNAL, *PHSTRING_HEADER_INTERNAL;

/*
 * @brief The internal structure of heap allocated Windows Runtime String.
 */
typedef struct _STRING_OPAQUE
{
    HSTRING_HEADER_INTERNAL Header;
    volatile INT RefCount;
    WCHAR String[ANYSIZE_ARRAY];
} STRING_OPAQUE, *PSTRING_OPAQUE;

#endif // !YY_THUNKS_HSTRING_PRIVATE
