

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
//Windows Vista, Windows Server 2008

EXTERN_C
ULONGLONG
WINAPI
GetTickCount64(
    VOID
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pGetTickCount64 = try_get_GetTickCount64())
	{
		return pGetTickCount64();
	}

	return GetTickCount();
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetTickCount64, 0);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)
//Windows 8, Windows Server 2012

EXTERN_C
VOID
WINAPI
GetSystemTimePreciseAsFileTime(
	_Out_ LPFILETIME lpSystemTimeAsFileTime
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pGetSystemTimePreciseAsFileTime = try_get_GetSystemTimePreciseAsFileTime())
	{
		return pGetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
	}

	return GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetSystemTimePreciseAsFileTime, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WINXPSP3)
//Windows Vista, Windows XP Professional x64 Edition, Windows XP with SP3 [desktop apps | UWP apps]
//Windows Server 2003 [desktop apps | UWP apps]

EXTERN_C
BOOL
WINAPI
GetLogicalProcessorInformation(
	_Out_writes_bytes_to_opt_(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer,
	_Inout_ PDWORD ReturnedLength
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetLogicalProcessorInformation = try_get_GetLogicalProcessorInformation())
	{
		return pGetLogicalProcessorInformation(Buffer, ReturnedLength);
	}
	
	SetLastError(ERROR_INVALID_FUNCTION);
	return FALSE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetLogicalProcessorInformation, 8);

#else
using ::GetLogicalProcessorInformation;
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7, Windows Server 2008 R2

EXTERN_C
BOOL
WINAPI
GetLogicalProcessorInformationEx(
	_In_ LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType,
	_Out_writes_bytes_to_opt_(*ReturnedLength, *ReturnedLength) PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Buffer,
	_Inout_ PDWORD ReturnedLength
	)
#ifdef YY_Thunks_Defined
	;
#else
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

	
	for (; YY::Thunks::GetLogicalProcessorInformation(pProcessorInfo, &cbLogicalProcessorInformation) == FALSE;)
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


				constexpr const size_t BitCountTable[256] =
				{
					0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
					1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
					1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
					1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
					2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
					3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
					3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
					4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
				};

				pInfo->Group.GroupInfo->ActiveProcessorMask = SystemInfo.dwActiveProcessorMask;

				auto& ActiveProcessorMask = SystemInfo.dwActiveProcessorMask;

#if defined(_M_IX86) || defined(_M_ARM)
				size_t ActiveProcessorCount = BitCountTable[((byte*)&ActiveProcessorMask)[0]] + BitCountTable[((byte*)&ActiveProcessorMask)[1]] + BitCountTable[((byte*)&ActiveProcessorMask)[2]] + BitCountTable[((byte*)&ActiveProcessorMask)[3]];
#elif defined(_M_AMD64) || defined(_M_IA64) || defined(_M_ARM64)
				size_t ActiveProcessorCount = BitCountTable[((byte*)&ActiveProcessorMask)[0]] + BitCountTable[((byte*)&ActiveProcessorMask)[1]] + BitCountTable[((byte*)&ActiveProcessorMask)[2]] + BitCountTable[((byte*)&ActiveProcessorMask)[3]]
					+ BitCountTable[((byte*)&ActiveProcessorMask)[4]] + BitCountTable[((byte*)&ActiveProcessorMask)[5]] + BitCountTable[((byte*)&ActiveProcessorMask)[6]] + BitCountTable[((byte*)&ActiveProcessorMask)[7]];
#else
				size_t ActiveProcessorCount = 0;
				for (int i = 0; i != sizeof(ActiveProcessorMask); ++i)
				{
					ActiveProcessorCount += BitCountTable[((byte*)&ActiveProcessorMask)[i]];
				}
#endif
				pInfo->Group.GroupInfo->ActiveProcessorCount = ActiveProcessorCount;
				pInfo->Group.GroupInfo->MaximumProcessorCount = SystemInfo.dwNumberOfProcessors;
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

__YY_Thunks_Expand_Function(kernel32, GetLogicalProcessorInformationEx, 12);

#endif

	}//namespace Thunks

} //namespace YY