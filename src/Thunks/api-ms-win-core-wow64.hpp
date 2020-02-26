

namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WS03)
//Windows XP Professional x64 Edition, Windows Server 2003

EXTERN_C
BOOL
WINAPI
Wow64DisableWow64FsRedirection(
	_Out_ PVOID* OldValue
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pWow64DisableWow64FsRedirection = try_get_Wow64DisableWow64FsRedirection())
	{
		return pWow64DisableWow64FsRedirection(OldValue);
	}


	SetLastError(ERROR_INVALID_FUNCTION);

	return FALSE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, Wow64DisableWow64FsRedirection, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)
//Windows XP Professional x64 Edition, Windows Server 2003

EXTERN_C
BOOL
WINAPI
Wow64RevertWow64FsRedirection(
	_In_ PVOID OlValue
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pWow64RevertWow64FsRedirection = try_get_Wow64RevertWow64FsRedirection())
	{
		return pWow64RevertWow64FsRedirection(OlValue);
	}


	SetLastError(ERROR_INVALID_FUNCTION);

	return FALSE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, Wow64RevertWow64FsRedirection, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)
//Windows XP Professional x64 Edition, Windows Server 2003

EXTERN_C
BOOLEAN
WINAPI
Wow64EnableWow64FsRedirection(
	_In_ BOOLEAN Wow64FsEnableRedirection
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pWow64EnableWow64FsRedirection = try_get_Wow64EnableWow64FsRedirection())
	{
		return pWow64EnableWow64FsRedirection(Wow64FsEnableRedirection);
	}


	SetLastError(ERROR_INVALID_FUNCTION);

	return FALSE;

}
#endif

__YY_Thunks_Expand_Function(kernel32, Wow64EnableWow64FsRedirection, 4);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03SP1)
//Windows XP with SP2, Windows Server 2003 with SP1

EXTERN_C
BOOL
WINAPI
IsWow64Process(
	_In_ HANDLE hProcess,
	_Out_ PBOOL Wow64Process
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pIsWow64Process = try_get_IsWow64Process())
	{
		return pIsWow64Process(hProcess, Wow64Process);
	}


	*Wow64Process = FALSE;

	return TRUE;
}
#endif

__YY_Thunks_Expand_Function(kernel32, IsWow64Process, 8);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS3) && (defined _X86_ || defined _AMD64_)
//Windows 10, Version 1511
//΢���ĵ��е����⣬ʵ��x86�Լ�amd64ϵͳ�У�16299��RS3���ſ�ʼ�д�API��

EXTERN_C
BOOL
WINAPI
IsWow64Process2(
	_In_      HANDLE  hProcess,
	_Out_     USHORT* pProcessMachine,
	_Out_opt_ USHORT* pNativeMachine
	)
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pIsWow64Process2 = try_get_IsWow64Process2())
	{
		return pIsWow64Process2(hProcess, pProcessMachine, pNativeMachine);
	}


	//�ж��Ƿ�������Wow6432�����
	BOOL bWow64Process;
	auto bRet = IsWow64Process(hProcess, &bWow64Process);

	if (bRet)
	{
		if (bWow64Process)
		{
			*pProcessMachine = IMAGE_FILE_MACHINE_I386;

			//IA64�Ѿ������ڲ���
			if (pNativeMachine)
				*pNativeMachine = IMAGE_FILE_MACHINE_AMD64;
		}
		else
		{
			*pProcessMachine = IMAGE_FILE_MACHINE_UNKNOWN;

#if defined _X86_
			if (pNativeMachine)
				*pNativeMachine = IMAGE_FILE_MACHINE_I386;
#elif defined _AMD64_
			if (pNativeMachine)
				*pNativeMachine = IMAGE_FILE_MACHINE_AMD64;
#else
#error ��֧�ִ���ϵ
#endif
		}
	}

	return bRet;
}
#endif

__YY_Thunks_Expand_Function(kernel32, IsWow64Process2, 12);

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS3) && (defined _X86_ || defined _AMD64_)
//Windows 10, version 1709

EXTERN_C
_Must_inspect_result_
HRESULT
WINAPI
IsWow64GuestMachineSupported(
	_In_ USHORT WowGuestMachine,
	_Out_ BOOL* MachineIsSupported
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pIsWow64GuestMachineSupported = try_get_IsWow64GuestMachineSupported())
	{
		return pIsWow64GuestMachineSupported(WowGuestMachine, MachineIsSupported);
	}

	if (IMAGE_FILE_MACHINE_I386 == WowGuestMachine)
	{
#ifdef _AMD64_
		*MachineIsSupported = TRUE;
#else
		SYSTEM_INFO SystemInfo;
		YY::Thunks::GetNativeSystemInfo(&SystemInfo);

		*MachineIsSupported = SystemInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL;
#endif

	}
	else
	{
		*MachineIsSupported = FALSE;
	}

	return S_OK;
}
#endif

__YY_Thunks_Expand_Function(kernel32, IsWow64GuestMachineSupported, 8);

#endif
	}//namespace Thunks

} //namespace YY