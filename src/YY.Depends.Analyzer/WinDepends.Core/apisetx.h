/*
*  File: apisetx.h
*
*  Created on: Aug 27, 2024
*
*  Modified on: Dec 06, 2024
*
*      Project: WinDepends.Core
*
*      Author: WinDepends dev team
* https://github.com/hfiref0x/WinDepends/blob/master/src/WinDepends.Core/apisetx.h
*/

#pragma once

#include <vector>
#include <atlstr.h>

#include "..\..\Shared\km.h"
using namespace YY;

#ifndef _APISETX_H_
#define _APISETX_H_

#ifndef RtlOffsetToPointer
#define RtlOffsetToPointer(Base, Offset)  ((PCHAR)( ((PCHAR)(Base)) + ((ULONG_PTR)(Offset))  ))
#endif

//
// Copy-pasted from MS headers from the pre nerfed state.
//

#ifndef API_SET_SECTION_NAME
#define API_SET_SECTION_NAME            ".apiset"
#endif

#ifndef API_SET_SCHEMA_SUFFIX
#define API_SET_SCHEMA_SUFFIX           L".sys"
#endif

#define API_SET_PREFIX_API              (ULONGLONG)0x002D004900500041 /* L"API-" */

#define API_SET_PREFIX_EXT              (ULONGLONG)0x002D005400580045 /* L"EXT-" */

#define API_SET_PREFIX_NAME_A           "API-"
#define API_SET_PREFIX_NAME_A_SIZE      (sizeof(API_SET_PREFIX_NAME_A) - sizeof(CHAR))
#define API_SET_PREFIX_NAME_A_LENGTH    (RTL_NUMBER_OF(API_SET_PREFIX_NAME_A) - 1)
#define API_SET_PREFIX_NAME_U           TEXT(API_SET_PREFIX_NAME_A)
#define API_SET_PREFIX_NAME_U_SIZE      (sizeof(API_SET_PREFIX_NAME_U) - sizeof(WCHAR))
#define API_SET_PREFIX_NAME_U_LENGTH    (RTL_NUMBER_OF(API_SET_PREFIX_NAME_U) - 1)

#define API_SET_EXTENSION_NAME_A        "EXT-"
#define API_SET_EXTENSION_NAME_A_SIZE   (sizeof(API_SET_EXTENSION_NAME_A) - sizeof(CHAR))
#define API_SET_EXTENSION_NAME_A_LENGTH (RTL_NUMBER_OF(API_SET_EXTENSION_NAME_A) - 1)
#define API_SET_EXTENSION_NAME_U        TEXT(API_SET_EXTENSION_NAME_A)
#define API_SET_EXTENSION_NAME_U_SIZE   (sizeof(API_SET_EXTENSION_NAME_U) - sizeof(WCHAR))
#define API_SET_EXTENSION_NAME_U_LENGTH (RTL_NUMBER_OF(API_SET_EXTENSION_NAME_U) - 1)

#define API_SET_DLL_EXT_A               ".DLL"
#define API_SET_DLL_EXT_A_SIZE          (sizeof(API_SET_DLL_EXT_A) - sizeof(CHAR))
#define API_SET_DLL_EXT_A_NAME_LENGTH   (RTL_NUMBER_OF(API_SET_DLL_EXT_A) - 1)
#define API_SET_DLL_EXT_U               TEXT(API_SET_DLL_EXT_A)
#define API_SET_DLL_EXT_U_SIZE          (sizeof(API_SET_DLL_EXT_U) - sizeof(WCHAR))
#define API_SET_DLL_EXT_U_NAME_LENGTH   (RTL_NUMBER_OF(API_SET_DLL_EXT_U) - 1)


#define API_SET_SCHEMA_FLAGS_SEALED              0x00000001UL
#define API_SET_SCHEMA_FLAGS_HOST_EXTENSION      0x00000002UL

#define API_SET_SCHEMA_ENTRY_FLAGS_SEALED        0x00000001UL
#define API_SET_SCHEMA_ENTRY_FLAGS_EXTENSION     0x00000002UL

#ifndef API_SET_SCHEMA_VERSION_V2
#define API_SET_SCHEMA_VERSION_V2    2
#endif

#ifndef API_SET_SCHEMA_VERSION_V3
#define API_SET_SCHEMA_VERSION_V3    3  //private
#endif

#ifndef API_SET_SCHEMA_VERSION_V4
#define API_SET_SCHEMA_VERSION_V4    4
#endif

#ifndef API_SET_SCHEMA_VERSION_V6
#define API_SET_SCHEMA_VERSION_V6    6
#endif

#define API_SET_TO_UPPER_PREFIX(x) ((x) & 0xFFFFFFDFFFDFFFDFULL)

//
// API Set Schema Version 2
//

typedef struct _API_SET_VALUE_ENTRY_V2 {
    ULONG NameOffset;
    ULONG NameLength;
    ULONG ValueOffset;
    ULONG ValueLength;
} API_SET_VALUE_ENTRY_V2, * PAPI_SET_VALUE_ENTRY_V2;

typedef struct _API_SET_VALUE_ARRAY_V2 {
    ULONG Count;
    API_SET_VALUE_ENTRY_V2 Array[ANYSIZE_ARRAY];
} API_SET_VALUE_ARRAY_V2, * PAPI_SET_VALUE_ARRAY_V2;

typedef struct _API_SET_NAMESPACE_ENTRY_V2 {
    ULONG NameOffset;
    ULONG NameLength;
    ULONG DataOffset;   // API_SET_VALUE_ARRAY
} API_SET_NAMESPACE_ENTRY_V2, * PAPI_SET_NAMESPACE_ENTRY_V2;

typedef struct _API_SET_NAMESPACE_ARRAY_V2 {
    ULONG Version;
    ULONG Count;
    _Field_size_full_(Count) API_SET_NAMESPACE_ENTRY_V2 Array[ANYSIZE_ARRAY];
} API_SET_NAMESPACE_ARRAY_V2, * PAPI_SET_NAMESPACE_ARRAY_V2;

#define API_SET_NAMESPACE_ENTRY_V2(ApiSetNamespace, Index) \
  (((PAPI_SET_NAMESPACE_ARRAY_V2)(ApiSetNamespace))->Array + Index)

//
// API Set Schema Version 4
//

typedef struct _API_SET_VALUE_ENTRY_V4 {
    ULONG Flags;
    ULONG NameOffset;
    _Field_range_(0, UNICODE_STRING_MAX_BYTES) ULONG NameLength;
    ULONG ValueOffset;
    _Field_range_(0, UNICODE_STRING_MAX_BYTES) ULONG ValueLength;
} API_SET_VALUE_ENTRY_V4, * PAPI_SET_VALUE_ENTRY_V4;

_Struct_size_bytes_(FIELD_OFFSET(API_SET_VALUE_ARRAY_V4, Array) + (sizeof(API_SET_VALUE_ENTRY_V4) * Count))
typedef struct _API_SET_VALUE_ARRAY_V4 {
    ULONG Flags;
    ULONG Count;
    _Field_size_full_(Count) API_SET_VALUE_ENTRY_V4 Array[ANYSIZE_ARRAY];
} API_SET_VALUE_ARRAY_V4, * PAPI_SET_VALUE_ARRAY_V4;

typedef struct _API_SET_NAMESPACE_ENTRY_V4 {
    ULONG Flags;
    ULONG NameOffset;
    _Field_range_(0, UNICODE_STRING_MAX_BYTES) ULONG NameLength;
    ULONG AliasOffset;
    _Field_range_(0, UNICODE_STRING_MAX_BYTES) ULONG AliasLength;
    ULONG DataOffset;   // API_SET_VALUE_ARRAY_V4
} API_SET_NAMESPACE_ENTRY_V4, * PAPI_SET_NAMESPACE_ENTRY_V4;

_Struct_size_bytes_(Size)
typedef struct _API_SET_NAMESPACE_ARRAY_V4 {
    ULONG Version;
    ULONG Size;
    ULONG Flags;
    ULONG Count;
    _Field_size_full_(Count) API_SET_NAMESPACE_ENTRY_V4 Array[ANYSIZE_ARRAY];
} API_SET_NAMESPACE_ARRAY_V4, * PAPI_SET_NAMESPACE_ARRAY_V4;

#define IS_API_SET_EMPTY_VALUE_ENTRY_V4(ValueEntry) \
    ((ValueEntry->ValueOffset == 0) && (ValueEntry->ValueLength == 0) && \
    (ValueEntry->NameOffset == 0) && (ValueEntry->NameLength == 0))

#define API_SET_NAMESPACE_ENTRY_V4(ApiSetNamespace, Index) \
    ((PAPI_SET_NAMESPACE_ENTRY_V4)(((PAPI_SET_NAMESPACE_ARRAY_V4)(ApiSetNamespace))->Array + Index))

#define API_SET_NAMESPACE_ENTRY_NAME_V4(ApiSetNamespace, NamespaceEntry) \
    ((PWCHAR)((ULONG_PTR)(ApiSetNamespace) + ((PAPI_SET_NAMESPACE_ENTRY_V4)(NamespaceEntry))->NameOffset))

#define API_SET_NAMESPACE_ENTRY_DATA_V4(ApiSetNamespace, NamespaceEntry) \
    ((PAPI_SET_VALUE_ARRAY_V4)((ULONG_PTR)(ApiSetNamespace) + ((PAPI_SET_NAMESPACE_ENTRY_V4)(NamespaceEntry))->DataOffset))

#define API_SET_VALUE_ENTRY_V4(ApiSetNamespace, ResolvedValueArray, Index) \
    ((PAPI_SET_VALUE_ENTRY_V4)(((PAPI_SET_VALUE_ARRAY_V4)(ResolvedValueArray))->Array + Index))

#define API_SET_VALUE_ENTRY_NAME_V4(ApiSetNamespace, ApiSetValueEntry) \
    ((WCHAR*)((ULONG_PTR)(ApiSetNamespace) + ((PAPI_SET_VALUE_ENTRY_V4)(ApiSetValueEntry))->NameOffset))

#define API_SET_VALUE_ENTRY_VALUE_V4(ApiSetNamespace, ApiSetValueEntry) \
    ((WCHAR*)((ULONG_PTR)(ApiSetNamespace) + ((PAPI_SET_VALUE_ENTRY_V4)(ApiSetValueEntry))->ValueOffset))

//
// API Set Schema Version 6
//

typedef struct _API_SET_HASH_ENTRY_V6 {
    ULONG Hash;
    ULONG NamespaceIndex;
} API_SET_HASH_ENTRY_V6, * PAPI_SET_HASH_ENTRY_V6;

typedef struct _API_SET_NAMESPACE_ENTRY_V6 {
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG HashNameLength; //size of name up to the last hyphen
    ULONG DataOffset;     //API_SET_VALUE_ENTRY_V6
    ULONG Count;          //number of API_SET_VALUE_ENTRY_V6 at DataOffset
} API_SET_NAMESPACE_ENTRY_V6, * PAPI_SET_NAMESPACE_ENTRY_V6;

typedef struct _API_SET_VALUE_ENTRY_V6 {
    ULONG Flags;
    ULONG NameOffset;
    ULONG NameLength;
    ULONG ValueOffset;
    ULONG ValueLength;
} API_SET_VALUE_ENTRY_V6, * PAPI_SET_VALUE_ENTRY_V6;

_Struct_size_bytes_(Size)
typedef struct _API_SET_NAMESPACE_V6 {
    ULONG Version;
    ULONG Size;
    ULONG Flags;
    ULONG Count;
    ULONG NamespaceEntryOffset;  //API_SET_NAMESPACE_ENTRY_V6
    ULONG NamespaceHashesOffset; //_API_SET_HASH_ENTRY_V6
    ULONG HashMultiplier;
} API_SET_NAMESPACE_V6, * PAPI_SET_NAMESPACE_V6;

#define IS_API_SET_EMPTY_VALUE_ENTRY_V6(ValueEntry) \
    ((ValueEntry->ValueOffset == 0) && (ValueEntry->ValueLength == 0) && \
    (ValueEntry->NameOffset == 0) && (ValueEntry->NameLength == 0))

#define API_SET_VALUE_ENTRY_V6(Namespace, Entry, Index) \
    ((API_SET_VALUE_ENTRY_V6 *)RtlOffsetToPointer(Namespace, (Index) * sizeof(API_SET_VALUE_ENTRY_V6) + Entry->DataOffset))

#define API_SET_VALUE_NAME_V6(Namespace, Entry) \
    ((PWCHAR)RtlOffsetToPointer(Namespace, Entry->NameOffset))

#define API_SET_VALUE_ENTRY_VALUE_V6(ApiSetNamespace, Entry) \
    ((PWCHAR)((ULONG_PTR)(ApiSetNamespace) + ((PAPI_SET_VALUE_ENTRY_V6)(Entry))->ValueOffset))

#define API_SET_HASH_ENTRY_V6(Namespace, HashIndex) \
   ((API_SET_HASH_ENTRY_V6*)RtlOffsetToPointer(Namespace, Namespace->NamespaceHashesOffset + sizeof(ULONG64) * (HashIndex)))

#define API_SET_NAMESPACE_ENTRY_V6(Namespace, LookupHashEntry) \
   ((PAPI_SET_NAMESPACE_ENTRY_V6)RtlOffsetToPointer(Namespace, LookupHashEntry->NamespaceIndex * sizeof(API_SET_NAMESPACE_ENTRY_V6) + \
        Namespace->NamespaceEntryOffset))

#define API_SET_NAMESPACE_ENTRY_NAME_V6(Namespace, NamespaceEntry) \
   ((PWCHAR)RtlOffsetToPointer(Namespace, NamespaceEntry->NameOffset))

typedef struct _API_SET_NAMESPACE {
    ULONG Version;
} API_SET_NAMESPACE, * PAPI_SET_NAMESPACE;

NTSTATUS
NTAPI
ApiSetResolveToHostV6(
    _In_ PAPI_SET_NAMESPACE ApiSetNamespace,
    _In_ PCUNICODE_STRING ApiSetNameToResolve,
    _In_opt_ PCUNICODE_STRING ParentName,
    _Out_ PUNICODE_STRING Output);

NTSTATUS
NTAPI
ApiSetResolveToHostV4(
    _In_ PAPI_SET_NAMESPACE ApiSetNamespace,
    _In_ PCUNICODE_STRING ResolveName,
    _In_opt_ PCUNICODE_STRING ParentName,
    _Out_ PUNICODE_STRING Output);

NTSTATUS 
NTAPI
ApiSetResolveToHostV2(
    _In_ PAPI_SET_NAMESPACE ApiSetNamespace,
    _In_ PCUNICODE_STRING ApiSetNameToResolve,
    _In_opt_ PCUNICODE_STRING ParentName,
    _Out_ PUNICODE_STRING Output);

PAPI_SET_NAMESPACE __fastcall LoadApisetNamespace(
    _In_ LPCWSTR apiset_schema_dll
    );

UNICODE_STRING __fastcall ResolveApisetName(
    _In_ PAPI_SET_NAMESPACE ApiSetNamespace,
    _In_ LPCWSTR apiset_name,
    _In_opt_ LPCWSTR parent_name
    );

struct ApisetNameSet
{
    CStringA ApisetName;
    UNICODE_STRING ResolveName;
};

std::vector<ApisetNameSet> __fastcall GetAllApisetNames(_In_ PAPI_SET_NAMESPACE ApiSetNamespace);


#endif /* _APISETX_H_ */
