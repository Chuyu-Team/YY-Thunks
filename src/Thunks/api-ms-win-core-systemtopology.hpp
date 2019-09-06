

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WINXPSP2)
//Windows Vista, Windows XP Professional x64 Edition, Windows XP with SP2, Windows Server 2003

EXTERN_C
BOOL
WINAPI
GetNumaNodeProcessorMask(
	_In_  UCHAR Node,
	_Out_ PULONGLONG ProcessorMask
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetNumaNodeProcessorMask = try_get_GetNumaNodeProcessorMask())
	{
		return pGetNumaNodeProcessorMask(Node, ProcessorMask);
	}
	else
	{
		//不支持此接口
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetNumaNodeProcessorMask, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7, Windows Server 2008 R2

EXTERN_C
BOOL
WINAPI
GetNumaNodeProcessorMaskEx(
	_In_ USHORT Node,
	_Out_ PGROUP_AFFINITY ProcessorMask
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetNumaNodeProcessorMaskEx = try_get_GetNumaNodeProcessorMaskEx())
	{
		return pGetNumaNodeProcessorMaskEx(Node, ProcessorMask);
	}
	else
	{
		ULONGLONG ullProcessorMask;
		auto bRet = GetNumaNodeProcessorMask(Node, &ullProcessorMask);

		if (bRet)
		{
			ProcessorMask->Mask = ullProcessorMask;
			//假定只有一组CPU
			ProcessorMask->Group = 0;
			ProcessorMask->Reserved[0] = 0;
			ProcessorMask->Reserved[1] = 0;
			ProcessorMask->Reserved[2] = 0;
		}

		return bRet;
	}

}
#endif

__YY_Thunks_Expand_Function(kernel32, GetNumaNodeProcessorMaskEx, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)
//Windows 7, Windows Server 2008 R2

EXTERN_C
BOOL
WINAPI
SetThreadGroupAffinity(
	_In_ HANDLE hThread,
	_In_ CONST GROUP_AFFINITY* GroupAffinity,
	_Out_opt_ PGROUP_AFFINITY PreviousGroupAffinity
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pSetThreadGroupAffinity = try_get_SetThreadGroupAffinity())
	{
		return pSetThreadGroupAffinity(hThread, GroupAffinity, PreviousGroupAffinity);
	}

	auto NewMask = SetThreadAffinityMask(hThread, GroupAffinity->Mask);

	if (NewMask==0)
	{
		return FALSE;
	}

	if (PreviousGroupAffinity)
	{
		PreviousGroupAffinity->Mask = NewMask;
		PreviousGroupAffinity->Group = 0;
		PreviousGroupAffinity->Reserved[0] = 0;
		PreviousGroupAffinity->Reserved[1] = 0;
		PreviousGroupAffinity->Reserved[2] = 0;
	}

	return TRUE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, SetThreadGroupAffinity, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WINXPSP2)
//Windows Vista, Windows XP Professional x64 Edition, Windows XP with SP2 [desktop apps only]
//Windows Server 2003 [desktop apps only]

EXTERN_C
BOOL
WINAPI
GetNumaHighestNodeNumber(
    _Out_ PULONG HighestNodeNumber
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto pGetNumaHighestNodeNumber = try_get_GetNumaHighestNodeNumber())
	{
		return pGetNumaHighestNodeNumber(HighestNodeNumber);
	}

	//不支持时始终假定只有一个NUMA节点
	*HighestNodeNumber = 0;

	return TRUE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, GetNumaHighestNodeNumber, 4);

#endif


	}//namespace Thunks

} //namespace YY