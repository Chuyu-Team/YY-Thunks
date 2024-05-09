

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows Vista, Windows Server 2008
		__DEFINE_THUNK(
		kernel32,
		0,
		ULONGLONG,
		WINAPI,
		GetTickCount64,
			VOID
			)
		{
			if (auto const pGetTickCount64 = try_get_GetTickCount64())
			{
				return pGetTickCount64();
			}

			__pragma(warning(suppress:28159))
			return GetTickCount();
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//Windows 8, Windows Server 2012
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		GetSystemTimePreciseAsFileTime,
			_Out_ LPFILETIME lpSystemTimeAsFileTime
			)
		{
			if (auto const pGetSystemTimePreciseAsFileTime = try_get_GetSystemTimePreciseAsFileTime())
			{
				return pGetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
			}

			return GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WINXPSP3)

		//Windows Vista, Windows XP Professional x64 Edition, Windows XP with SP3 [desktop apps | UWP apps]
		//Windows Server 2003 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		8,
		BOOL,
		WINAPI,
		GetLogicalProcessorInformation,
			_Out_writes_bytes_to_opt_(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
			_Inout_ PDWORD ReturnedLength
			)
		{
			if (auto pGetLogicalProcessorInformation = try_get_GetLogicalProcessorInformation())
			{
				return pGetLogicalProcessorInformation(Buffer, ReturnedLength);
			}
	
			SetLastError(ERROR_INVALID_FUNCTION);
			return FALSE;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		//Windows 7, Windows Server 2008 R2
		__DEFINE_THUNK(
		kernel32,
		12,
		_Success_(return)
		BOOL,
		WINAPI,
		GetLogicalProcessorInformationEx,
			_In_ LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
			_Out_writes_bytes_to_opt_(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Buffer,
			_Inout_ PDWORD ReturnedLength
			)
		{
			if (auto pGetLogicalProcessorInformationEx = try_get_GetLogicalProcessorInformationEx())
			{
				return pGetLogicalProcessorInformationEx(RelationshipType, Buffer, ReturnedLength);
			}


			if (!ReturnedLength)
			{
				SetLastError(ERROR_INVALID_PARAMETER);
				return FALSE;
			}
	
			const auto cbBuffer = *ReturnedLength;

			if (cbBuffer != 0 && Buffer == nullptr)
			{
				SetLastError(ERROR_NOACCESS);
				return FALSE;
			}
	
			const auto ProcessHeap = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ProcessHeap;
			LSTATUS lStatus = ERROR_SUCCESS;

			SYSTEM_LOGICAL_PROCESSOR_INFORMATION* pProcessorInfo = nullptr;
			DWORD cbLogicalProcessorInformation = 0;

	
			for (; GetLogicalProcessorInformation(pProcessorInfo, &cbLogicalProcessorInformation) == FALSE;)
			{
				lStatus = GetLastError();

				if (ERROR_INSUFFICIENT_BUFFER == lStatus)
				{
					if (pProcessorInfo)
					{
						auto pNewBuffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)HeapReAlloc(ProcessHeap, 0, pProcessorInfo, cbLogicalProcessorInformation);
						if (pNewBuffer)
						{
							pProcessorInfo = pNewBuffer;
							continue;
						}
					}
					else
					{
						pProcessorInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)HeapAlloc(ProcessHeap, 0, cbLogicalProcessorInformation);

						if (pProcessorInfo)
							continue;
					}

					lStatus = ERROR_NOT_ENOUGH_MEMORY;
				}

				goto __End;
			}


			{
				const auto pProcessorInfoLastItem = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)((byte*)pProcessorInfo + cbLogicalProcessorInformation - sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

				DWORD cbBufferUsed = 0;



				for (auto pProcessorInfoStart = pProcessorInfo; pProcessorInfoStart <= pProcessorInfoLastItem; ++pProcessorInfoStart)
				{
					if (RelationshipType == LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll
						|| pProcessorInfoStart->Relationship == RelationshipType)
					{
						DWORD cbInfoNeed;

						switch (pProcessorInfoStart->Relationship)
						{
						case RelationProcessorCore:
						case RelationProcessorPackage:
							cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Processor);
							break;
						case RelationNumaNode:
							cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, NumaNode);
							break;
						case RelationCache:
							cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Cache);
							break;
						default:
							cbInfoNeed = 0;
							break;
						}

						//不支持传输此类型
						if (0 == cbInfoNeed)
							continue;

						cbBufferUsed += cbInfoNeed;

						if (cbBuffer >= cbBufferUsed)
						{
							auto pInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)((byte*)Buffer + cbBufferUsed);

							memset(pInfo, 0, cbInfoNeed);

							pInfo->Size = cbInfoNeed;
							pInfo->Relationship = pProcessorInfoStart->Relationship;

							switch (pProcessorInfoStart->Relationship)
							{
							case RelationProcessorCore:
							case RelationProcessorPackage:
								pInfo->Processor.Flags = pProcessorInfoStart->ProcessorCore.Flags;
								pInfo->Processor.GroupCount = 1;
								pInfo->Processor.GroupMask->Mask = pProcessorInfoStart->ProcessorMask;
								break;
							case RelationNumaNode:
								pInfo->NumaNode.NodeNumber = pProcessorInfoStart->NumaNode.NodeNumber;
								pInfo->NumaNode.GroupMask.Mask = pProcessorInfoStart->ProcessorMask;
								break;
							case RelationCache:
								pInfo->Cache.Level = pProcessorInfoStart->Cache.Level;
								pInfo->Cache.Associativity = pProcessorInfoStart->Cache.Associativity;
								pInfo->Cache.LineSize = pProcessorInfoStart->Cache.LineSize;
								pInfo->Cache.CacheSize = pProcessorInfoStart->Cache.Size;
								pInfo->Cache.Type = pProcessorInfoStart->Cache.Type;
								pInfo->Cache.GroupMask.Mask= pProcessorInfoStart->ProcessorMask;
								break;
							}
						}
					}
				}


				//传输 RelationGroup 信息，这里只能假设只有一组CPU
				if (RelationshipType == LOGICAL_PROCESSOR_RELATIONSHIP::RelationAll
					|| RelationGroup == RelationshipType)
				{
					const auto cbInfoNeed = RTL_SIZEOF_THROUGH_FIELD(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Group);

					cbBufferUsed += cbInfoNeed;

					if (cbBuffer >= cbBufferUsed)
					{
						auto pInfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)((byte*)Buffer + cbBufferUsed);

						memset(pInfo, 0, cbInfoNeed);

						pInfo->Size = cbInfoNeed;
						pInfo->Relationship = RelationGroup;

						pInfo->Group.ActiveGroupCount = 1;
						pInfo->Group.MaximumGroupCount = 1;

						SYSTEM_INFO SystemInfo;

						GetSystemInfo(&SystemInfo);

						pInfo->Group.GroupInfo->ActiveProcessorMask = SystemInfo.dwActiveProcessorMask;
						pInfo->Group.GroupInfo->ActiveProcessorCount = internal::BitsCount(static_cast<size_t>(SystemInfo.dwActiveProcessorMask));
						pInfo->Group.GroupInfo->MaximumProcessorCount = static_cast<BYTE>(SystemInfo.dwNumberOfProcessors);
					}
				}

				*ReturnedLength = cbBufferUsed;

				if (cbBufferUsed > cbBuffer)
				{
					//缓冲区不足
					lStatus = ERROR_INSUFFICIENT_BUFFER;
				}
			}

		__End:
			if (pProcessorInfo)
				HeapFree(ProcessHeap, 0, pProcessorInfo);

			if (lStatus != ERROR_SUCCESS)
			{
				SetLastError(lStatus);

				return FALSE;
			}
			else
			{
				return TRUE;
			}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WINXP)

		//Windows XP [desktop apps | UWP apps]
		//Windows Server 2003 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		4,
		VOID,
		WINAPI,
		GetNativeSystemInfo,
			_Out_ LPSYSTEM_INFO lpSystemInfo
			)
		{
			if (const auto pGetNativeSystemInfo = try_get_GetNativeSystemInfo())
			{
				return pGetNativeSystemInfo(lpSystemInfo);
			}


			GetSystemInfo(lpSystemInfo);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		//Windows XP [desktop apps | UWP apps]
		//Windows Server 2003 [desktop apps | UWP apps]
		__DEFINE_THUNK(
		kernel32,
		20,
		BOOL,
		WINAPI,
		GetProductInfo,
			_In_ DWORD dwOSMajorVersion,
			_In_ DWORD dwOSMinorVersion,
			_In_ DWORD dwSpMajorVersion,
			_In_ DWORD dwSpMinorVersion,
			_Out_ PDWORD pdwReturnedProductType
			)
		{
			if (const auto pGetProductInfo = try_get_GetProductInfo())
			{
				return pGetProductInfo(dwOSMajorVersion, dwOSMinorVersion, dwSpMajorVersion, dwSpMinorVersion, pdwReturnedProductType);
			}

			if (!pdwReturnedProductType)
				return FALSE;

			//故意不检测版本号，能凑合就尽可能的凑合吧……

			DWORD dwReturnedProductType = PRODUCT_UNLICENSED;

			//由于不支持这个接口，所以系统版本必然是XP或者以下
			OSVERSIONINFOEXW VersionInfo = { sizeof(VersionInfo) };
            #pragma warning(suppress:4996 28159)
			if (GetVersionExW((LPOSVERSIONINFOW)&VersionInfo))
			{
				if (VersionInfo.wSuiteMask & VER_SUITE_ENTERPRISE)
				{
					dwReturnedProductType = PRODUCT_ENTERPRISE_SERVER;
				}
				else if (VersionInfo.wSuiteMask & VER_SUITE_SMALLBUSINESS_RESTRICTED) // VER_SUITE_SMALLBUSINESS 与 VER_SUITE_SMALLBUSINESS_RESTRICTED
				{
					dwReturnedProductType = PRODUCT_SMALLBUSINESS_SERVER;
				}
				else if (VersionInfo.wSuiteMask & VER_SUITE_EMBEDDED_RESTRICTED) // VER_SUITE_EMBEDDEDNT 与 VER_SUITE_EMBEDDED_RESTRICTED
				{
					dwReturnedProductType = PRODUCT_EMBEDDED;
				}
				else if (VersionInfo.wSuiteMask & VER_SUITE_DATACENTER)
				{
					dwReturnedProductType = PRODUCT_DATACENTER_SERVER;
				}
				else if (VersionInfo.wSuiteMask & VER_SUITE_PERSONAL)
				{
					dwReturnedProductType = PRODUCT_HOME_BASIC;
				}
				else if (VersionInfo.wSuiteMask & VER_SUITE_BLADE)
				{
					dwReturnedProductType = PRODUCT_WEB_SERVER;
				}
				else if (VersionInfo.wSuiteMask & VER_SUITE_STORAGE_SERVER)
				{
					dwReturnedProductType = PRODUCT_STORAGE_STANDARD_SERVER;
				}
				else if (VersionInfo.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
				{
					dwReturnedProductType = PRODUCT_CLUSTER_SERVER;
				}
				else if (VersionInfo.wSuiteMask & VER_SUITE_WH_SERVER)
				{
					dwReturnedProductType = PRODUCT_HOME_SERVER;
				}
				else
				{
					if (VersionInfo.wProductType == VER_NT_WORKSTATION)
					{
						//如果目标系统是5.1，因为SuiteMask中检测了Home，所以这里只可能是专业版
						// 
						//如果目标系统是5.0、5.2，只可能是专业版。

						dwReturnedProductType = PRODUCT_PROFESSIONAL;
					}
					else if (VersionInfo.wProductType == VER_NT_DOMAIN_CONTROLLER || VersionInfo.wProductType == VER_NT_SERVER)
					{
						//如果目标系统是5.1、5.2，因为SuiteMask中检测了Small Business Server、Web Edition、Enterprise Edition、
						//Datacenter Edition、Compute Cluster Server、Storage Server，所以这里只可能是Standard Edition
						//
						//如果目标系统是5.0，因为SuiteMask中检测了Datacenter Server，这里只能是 Advanced Server或者 Server，但是我没有找到 Advanced，姑且用 PRODUCT_STANDARD_SERVER 凑合吧。
						dwReturnedProductType = PRODUCT_STANDARD_SERVER;
					}
					else
					{
						//这是什么鬼？
						dwReturnedProductType = PRODUCT_UNDEFINED;
					}
				}
			}

			*pdwReturnedProductType = dwReturnedProductType;

			return dwReturnedProductType != PRODUCT_UNDEFINED;
		}
#endif
	}//namespace Thunks

} //namespace YY
