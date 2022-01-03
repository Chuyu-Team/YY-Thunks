

namespace YY
{
	namespace Thunks
	{
#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		//Minimum supported client	Windows 8 [desktop apps only]
		//Minimum supported server	Windows Server 2012 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		4,
		BOOL,
		WINAPI,
		GetMemoryErrorHandlingCapabilities,
			_Out_ PULONG Capabilities
			)
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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		16,
		_Success_(return != FALSE)
		BOOL,
		WINAPI,
		AllocateUserPhysicalPagesNuma,
			_In_ HANDLE hProcess,
			_Inout_ PULONG_PTR NumberOfPages,
			_Out_writes_to_(*NumberOfPages,*NumberOfPages) PULONG_PTR PageArray,
			_In_ DWORD nndPreferred
			)
		{
			if (const auto pAllocateUserPhysicalPagesNuma = try_get_AllocateUserPhysicalPagesNuma())
			{
				return pAllocateUserPhysicalPagesNuma(hProcess, NumberOfPages, PageArray, nndPreferred);
			}

			return AllocateUserPhysicalPages(hProcess, NumberOfPages, PageArray);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		//Minimum supported client	Windows 10 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2016 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		PVOID,
		WINAPI,
		VirtualAllocFromApp,
			_In_opt_ PVOID BaseAddress,
			_In_ SIZE_T Size,
			_In_ ULONG AllocationType,
			_In_ ULONG Protection
			)
		{
			if (const auto pVirtualAllocFromApp = try_get_VirtualAllocFromApp())
			{
				return pVirtualAllocFromApp(BaseAddress, Size, AllocationType, Protection);
			}


			return VirtualAlloc(BaseAddress, Size, AllocationType, Protection);
		}
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		24,
		LPVOID,
		WINAPI,
		VirtualAllocExNuma,
			_In_ HANDLE hProcess,
			_In_opt_ LPVOID lpAddress,
			_In_ SIZE_T dwSize,
			_In_ DWORD flAllocationType,
			_In_ DWORD flProtect,
			_In_ DWORD nndPreferred
			)
		{
			if (const auto pVirtualAllocExNuma = try_get_VirtualAllocExNuma())
			{
				return pVirtualAllocExNuma(hProcess, lpAddress, dwSize, flAllocationType, flProtect, nndPreferred);
			}

			return VirtualAllocEx(hProcess, lpAddress, dwSize, flAllocationType, flProtect);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		//Minimum supported client	Windows 10 [desktop apps only]
		//Minimum supported server	Windows Server 2016 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		28,
		PVOID,
		WINAPI,
		VirtualAlloc2,
			_In_opt_ HANDLE Process,
			_In_opt_ PVOID BaseAddress,
			_In_ SIZE_T Size,
			_In_ ULONG AllocationType,
			_In_ ULONG PageProtection,
			_Inout_updates_opt_(ParameterCount) MEM_EXTENDED_PARAMETER* ExtendedParameters,
			_In_ ULONG ParameterCount
			)
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


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		//Minimum supported client	Windows 10 [desktop apps only]
		//Minimum supported server	Windows Server 2016 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		28,
		PVOID,
		WINAPI,
		VirtualAlloc2FromApp,
			_In_opt_ HANDLE Process,
			_In_opt_ PVOID BaseAddress,
			_In_ SIZE_T Size,
			_In_ ULONG AllocationType,
			_In_ ULONG PageProtection,
			_Inout_updates_opt_(ParameterCount) MEM_EXTENDED_PARAMETER* ExtendedParameters,
			_In_ ULONG ParameterCount
			)
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


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		28,
		HANDLE,
		WINAPI,
		CreateFileMappingNumaW,
			_In_ HANDLE hFile,
			_In_opt_ LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
			_In_ DWORD flProtect,
			_In_ DWORD dwMaximumSizeHigh,
			_In_ DWORD dwMaximumSizeLow,
			_In_opt_ LPCWSTR lpName,
			_In_ DWORD nndPreferred
			)
		{
			if (const auto pCreateFileMappingNumaW = try_get_CreateFileMappingNumaW())
			{
				return pCreateFileMappingNumaW(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName, nndPreferred);
			}

			return CreateFileMappingW(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Minimum supported client	Windows Vista [desktop apps only]
		//Minimum supported server	Windows Server 2008 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		28,
		HANDLE,
		WINAPI,
		CreateFileMappingNumaA,
			_In_     HANDLE hFile,
			_In_opt_ LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
			_In_     DWORD flProtect,
			_In_     DWORD dwMaximumSizeHigh,
			_In_     DWORD dwMaximumSizeLow,
			_In_opt_ LPCSTR lpName,
			_In_     DWORD nndPreferred
			)
		{
			if (const auto pCreateFileMappingNumaA = try_get_CreateFileMappingNumaA())
			{
				return pCreateFileMappingNumaA(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName, nndPreferred);
			}

			return CreateFileMappingA(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Minimum supported client	Windows 8 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		24,
		HANDLE,
		WINAPI,
		CreateFileMappingFromApp,
			_In_ HANDLE hFile,
			_In_opt_ PSECURITY_ATTRIBUTES SecurityAttributes,
			_In_ ULONG PageProtection,
			_In_ ULONG64 MaximumSize,
			_In_opt_ PCWSTR Name
			)
		{
			if (const auto pVirtualAllocExNuma = try_get_CreateFileMappingFromApp())
			{
				return pVirtualAllocExNuma(hFile, SecurityAttributes, PageProtection, MaximumSize, Name);
			}


			return CreateFileMappingW(hFile, SecurityAttributes, PageProtection, MaximumSize >> 32, MaximumSize & 0xFFFFFFFF, Name);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Minimum supported client	Windows 8 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		40,
		HANDLE,
		WINAPI,
		CreateFileMapping2,
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


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Minimum supported client	Windows 8 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		20,
		PVOID,
		WINAPI,
		MapViewOfFileFromApp,
			_In_ HANDLE hFileMappingObject,
			_In_ ULONG DesiredAccess,
			_In_ ULONG64 FileOffset,
			_In_ SIZE_T NumberOfBytesToMap
			)
		{
			if (const auto pMapViewOfFileFromApp = try_get_MapViewOfFileFromApp())
			{
				return pMapViewOfFileFromApp(hFileMappingObject, DesiredAccess, FileOffset, NumberOfBytesToMap);
			}

			return MapViewOfFile(hFileMappingObject, DesiredAccess, FileOffset >> 32, FileOffset & 0xFFFFFFFF, NumberOfBytesToMap);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Minimum supported client	Windows 8 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		BOOL,
		WINAPI,
		UnmapViewOfFileEx,
			_In_ PVOID BaseAddress,
			_In_ ULONG UnmapFlags
			)
		{
			if (const auto pUnmapViewOfFileEx = try_get_UnmapViewOfFileEx())
			{
				return pUnmapViewOfFileEx(BaseAddress, UnmapFlags);
			}

			return UnmapViewOfFile(BaseAddress);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		//Minimum supported client	Windows 10 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2016 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		16,
		BOOL,
		WINAPI,
		VirtualProtectFromApp,
			_In_ PVOID Address,
			_In_ SIZE_T Size,
			_In_ ULONG NewProtection,
			_Out_ PULONG OldProtection
			)
		{
			if (const auto pVirtualProtectFromApp = try_get_VirtualProtectFromApp())
			{
				return pVirtualProtectFromApp(Address, Size, NewProtection, OldProtection);
			}

			return VirtualProtect(Address, Size, NewProtection, OldProtection);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10)

		//Minimum supported client	Windows 10 [desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2016 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		12,
		HANDLE,
		WINAPI,
		OpenFileMappingFromApp,
			_In_ ULONG DesiredAccess,
			_In_ BOOL InheritHandle,
			_In_ PCWSTR Name
			)
		{
			if (const auto pOpenFileMappingFromApp = try_get_OpenFileMappingFromApp())
			{
				return pOpenFileMappingFromApp(DesiredAccess, InheritHandle, Name);
			}

			return OpenFileMappingW(DesiredAccess, InheritHandle, Name);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)

		//Minimum supported client	Windows Vista[desktop apps | UWP apps]
		//Minimum supported server	Windows Server 2003[desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		0,
		SIZE_T,
		WINAPI,
		GetLargePageMinimum,
			VOID
			)
		{
			if (const auto pGetLargePageMinimum = try_get_GetLargePageMinimum())
			{
				return pGetLargePageMinimum();
			}

			return 0;
		}
#endif
	}
}