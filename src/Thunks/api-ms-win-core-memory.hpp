

namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		//Minimum supported client	Windows 8 [desktop apps only]
		//Minimum supported server	Windows Server 2012 [desktop apps only]
		EXTERN_C
		BOOL
		WINAPI
		GetMemoryErrorHandlingCapabilities(
			_Out_ PULONG Capabilities
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (auto pGetMemoryErrorHandlingCapabilities = try_get_GetMemoryErrorHandlingCapabilities())
			{
				return pGetMemoryErrorHandlingCapabilities(Capabilities);
			}

			//别怪我，我们什么特性都不支持……
			*Capabilities = 0;
			return TRUE;
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, GetMemoryErrorHandlingCapabilities, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		EXTERN_C
		_Success_(return != FALSE)
		BOOL
		WINAPI
		AllocateUserPhysicalPagesNuma(
			_In_ HANDLE hProcess,
			_Inout_ PULONG_PTR NumberOfPages,
			_Out_writes_to_(*NumberOfPages,*NumberOfPages) PULONG_PTR PageArray,
			_In_ DWORD nndPreferred
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pAllocateUserPhysicalPagesNuma = try_get_AllocateUserPhysicalPagesNuma())
			{
				return pAllocateUserPhysicalPagesNuma(hProcess, NumberOfPages, PageArray, nndPreferred);
			}

			return AllocateUserPhysicalPages(hProcess, NumberOfPages, PageArray);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, AllocateUserPhysicalPagesNuma, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		//Minimum supported client	Windows 10 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2016 [desktop apps | UWP apps]
		PVOID
		WINAPI
		VirtualAllocFromApp(
			_In_opt_ PVOID BaseAddress,
			_In_ SIZE_T Size,
			_In_ ULONG AllocationType,
			_In_ ULONG Protection
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pVirtualAllocFromApp = try_get_VirtualAllocFromApp())
			{
				return pVirtualAllocFromApp(BaseAddress, Size, AllocationType, Protection);
			}


			return VirtualAlloc(BaseAddress, Size, AllocationType, Protection);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, VirtualAllocFromApp, 16);

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		EXTERN_C
		LPVOID
		WINAPI
		VirtualAllocExNuma(
			_In_ HANDLE hProcess,
			_In_opt_ LPVOID lpAddress,
			_In_ SIZE_T dwSize,
			_In_ DWORD flAllocationType,
			_In_ DWORD flProtect,
			_In_ DWORD nndPreferred
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pVirtualAllocExNuma = try_get_VirtualAllocExNuma())
			{
				return pVirtualAllocExNuma(hProcess, lpAddress, dwSize, flAllocationType, flProtect, nndPreferred);
			}

			return VirtualAllocEx(hProcess, lpAddress, dwSize, flAllocationType, flProtect);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, VirtualAllocExNuma, 24);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		//Minimum supported client	Windows 10 [desktop apps only]
		//Minimum supported server	Windows Server 2016 [desktop apps only]
		EXTERN_C
		PVOID
		WINAPI
		VirtualAlloc2(
			_In_opt_ HANDLE Process,
			_In_opt_ PVOID BaseAddress,
			_In_ SIZE_T Size,
			_In_ ULONG AllocationType,
			_In_ ULONG PageProtection,
			_Inout_updates_opt_(ParameterCount) MEM_EXTENDED_PARAMETER* ExtendedParameters,
			_In_ ULONG ParameterCount
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pVirtualAlloc2 = try_get_VirtualAlloc2())
			{
				return pVirtualAlloc2(Process, BaseAddress, Size, AllocationType, PageProtection, ExtendedParameters, ParameterCount);
			}

			//尝试搜索 MemExtendedParameterNumaNode
			if (ExtendedParameters && ParameterCount)
			{
				for (ULONG i = 0; i != ParameterCount; ++i)
				{
					if (ExtendedParameters[i].Type == MemExtendedParameterNumaNode)
					{
						return VirtualAllocExNuma(Process, BaseAddress, Size, AllocationType, PageProtection, ExtendedParameters[i].ULong);
					}
				}
			}

			//尽力了，只能调用VirtualAllocEx。
			return VirtualAllocEx(Process, BaseAddress, Size, AllocationType, PageProtection);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, VirtualAlloc2, 28);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		//Minimum supported client	Windows 10 [desktop apps only]
		//Minimum supported server	Windows Server 2016 [desktop apps only]
		EXTERN_C
		PVOID
		WINAPI
		VirtualAlloc2FromApp(
			_In_opt_ HANDLE Process,
			_In_opt_ PVOID BaseAddress,
			_In_ SIZE_T Size,
			_In_ ULONG AllocationType,
			_In_ ULONG PageProtection,
			_Inout_updates_opt_(ParameterCount) MEM_EXTENDED_PARAMETER* ExtendedParameters,
			_In_ ULONG ParameterCount
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pVirtualAlloc2FromApp = try_get_VirtualAlloc2FromApp())
			{
				return pVirtualAlloc2FromApp(Process, BaseAddress, Size, AllocationType, PageProtection, ExtendedParameters, ParameterCount);
			}

			//尝试搜索 MemExtendedParameterNumaNode
			if (ExtendedParameters && ParameterCount)
			{
				for (ULONG i = 0; i != ParameterCount; ++i)
				{
					if (ExtendedParameters[i].Type == MemExtendedParameterNumaNode)
					{
						return VirtualAllocExNuma(Process, BaseAddress, Size, AllocationType, PageProtection, ExtendedParameters[i].ULong);
					}
				}
			}

			return VirtualAllocEx(Process, BaseAddress, Size, AllocationType, PageProtection);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, VirtualAlloc2FromApp, 28);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		EXTERN_C
		HANDLE
		WINAPI
		CreateFileMappingNumaW(
			_In_ HANDLE hFile,
			_In_opt_ LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
			_In_ DWORD flProtect,
			_In_ DWORD dwMaximumSizeHigh,
			_In_ DWORD dwMaximumSizeLow,
			_In_opt_ LPCWSTR lpName,
			_In_ DWORD nndPreferred
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pCreateFileMappingNumaW = try_get_CreateFileMappingNumaW())
			{
				return pCreateFileMappingNumaW(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName, nndPreferred);
			}

			return CreateFileMappingW(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, CreateFileMappingNumaW, 28);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		EXTERN_C
		HANDLE
		WINAPI
		CreateFileMappingNumaA(
			_In_     HANDLE hFile,
			_In_opt_ LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
			_In_     DWORD flProtect,
			_In_     DWORD dwMaximumSizeHigh,
			_In_     DWORD dwMaximumSizeLow,
			_In_opt_ LPCSTR lpName,
			_In_     DWORD nndPreferred
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pCreateFileMappingNumaA = try_get_CreateFileMappingNumaA())
			{
				return pCreateFileMappingNumaA(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName, nndPreferred);
			}

			return CreateFileMappingA(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, CreateFileMappingNumaA, 28);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Minimum supported client	Windows 8 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
		EXTERN_C
		HANDLE
		WINAPI
		CreateFileMappingFromApp(
			_In_ HANDLE hFile,
			_In_opt_ PSECURITY_ATTRIBUTES SecurityAttributes,
			_In_ ULONG PageProtection,
			_In_ ULONG64 MaximumSize,
			_In_opt_ PCWSTR Name
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pVirtualAllocExNuma = try_get_CreateFileMappingFromApp())
			{
				return pVirtualAllocExNuma(hFile, SecurityAttributes, PageProtection, MaximumSize, Name);
			}


			return CreateFileMappingW(hFile, SecurityAttributes, PageProtection, MaximumSize >> 32, MaximumSize & 0xFFFFFFFF, Name);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, CreateFileMappingFromApp, 24);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Minimum supported client	Windows 8 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
		EXTERN_C
		HANDLE
		WINAPI
		CreateFileMapping2(
			_In_ HANDLE File,
			_In_opt_ SECURITY_ATTRIBUTES* SecurityAttributes,
			_In_ ULONG DesiredAccess,
			_In_ ULONG PageProtection,
			_In_ ULONG AllocationAttributes,
			_In_ ULONG64 MaximumSize,
			_In_opt_ PCWSTR Name,
			_Inout_updates_opt_(ParameterCount) MEM_EXTENDED_PARAMETER* ExtendedParameters,
			_In_ ULONG ParameterCount
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pCreateFileMapping2 = try_get_CreateFileMapping2())
			{
				return pCreateFileMapping2(File, SecurityAttributes, DesiredAccess, PageProtection, AllocationAttributes,  MaximumSize, Name, ExtendedParameters, ParameterCount);
			}
			
			//尝试搜索 MemExtendedParameterNumaNode
			if (ExtendedParameters && ParameterCount)
			{
				for (ULONG i = 0; i != ParameterCount; ++i)
				{
					if (ExtendedParameters[i].Type == MemExtendedParameterNumaNode)
					{
						return CreateFileMappingNumaW(File, SecurityAttributes, PageProtection | AllocationAttributes, MaximumSize >> 32, MaximumSize & 0xFFFFFFFF, Name, ExtendedParameters[i].ULong);
					}
				}
			}

			return CreateFileMappingW(File, SecurityAttributes, PageProtection | AllocationAttributes, MaximumSize >> 32, MaximumSize & 0xFFFFFFFF, Name);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, CreateFileMapping2, 40);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Minimum supported client	Windows 8 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
		EXTERN_C
		PVOID
		WINAPI
		MapViewOfFileFromApp(
			_In_ HANDLE hFileMappingObject,
			_In_ ULONG DesiredAccess,
			_In_ ULONG64 FileOffset,
			_In_ SIZE_T NumberOfBytesToMap
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pMapViewOfFileFromApp = try_get_MapViewOfFileFromApp())
			{
				return pMapViewOfFileFromApp(hFileMappingObject, DesiredAccess, FileOffset, NumberOfBytesToMap);
			}

			return MapViewOfFile(hFileMappingObject, DesiredAccess, FileOffset >> 32, FileOffset & 0xFFFFFFFF, NumberOfBytesToMap);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, MapViewOfFileFromApp, 20);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Minimum supported client	Windows 8 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
		EXTERN_C
		BOOL
		WINAPI
		UnmapViewOfFileEx(
			_In_ PVOID BaseAddress,
			_In_ ULONG UnmapFlags
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pUnmapViewOfFileEx = try_get_UnmapViewOfFileEx())
			{
				return pUnmapViewOfFileEx(BaseAddress, UnmapFlags);
			}

			return UnmapViewOfFile(BaseAddress);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, UnmapViewOfFileEx, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		//Minimum supported client	Windows 10 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2016 [desktop apps | UWP apps]
		EXTERN_C
		BOOL
		WINAPI
		VirtualProtectFromApp(
			_In_ PVOID Address,
			_In_ SIZE_T Size,
			_In_ ULONG NewProtection,
			_Out_ PULONG OldProtection
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pVirtualProtectFromApp = try_get_VirtualProtectFromApp())
			{
				return pVirtualProtectFromApp(Address, Size, NewProtection, OldProtection);
			}

			return VirtualProtect(Address, Size, NewProtection, OldProtection);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, VirtualProtectFromApp, 16);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		//Minimum supported client	Windows 10 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2016 [desktop apps | UWP apps]
		EXTERN_C
		HANDLE
		WINAPI
		OpenFileMappingFromApp(
			_In_ ULONG DesiredAccess,
			_In_ BOOL InheritHandle,
			_In_ PCWSTR Name
			)
#ifdef YY_Thunks_Defined
			;
#else
		{
			if (const auto pOpenFileMappingFromApp = try_get_OpenFileMappingFromApp())
			{
				return pOpenFileMappingFromApp(DesiredAccess, InheritHandle, Name);
			}

			return OpenFileMappingW(DesiredAccess, InheritHandle, Name);
		}
#endif

		__YY_Thunks_Expand_Function(kernel32, OpenFileMappingFromApp, 12);

#endif




	}
}