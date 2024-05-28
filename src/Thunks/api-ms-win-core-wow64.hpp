

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WS03)

	//Windows XP Professional x64 Edition, Windows Server 2003
	__DEFINE_THUNK(
	kernel32,
	4,
	BOOL,
	WINAPI,
	Wow64DisableWow64FsRedirection,
		_Out_ PVOID* OldValue
		)
	{
		if (auto const pWow64DisableWow64FsRedirection = try_get_Wow64DisableWow64FsRedirection())
		{
			return pWow64DisableWow64FsRedirection(OldValue);
		}


		SetLastError(ERROR_INVALID_FUNCTION);

		return FALSE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)

	//Windows XP Professional x64 Edition, Windows Server 2003
	__DEFINE_THUNK(
	kernel32,
	4,
	BOOL,
	WINAPI,
	Wow64RevertWow64FsRedirection,
		_In_ PVOID OlValue
		)
	{
		if (auto const pWow64RevertWow64FsRedirection = try_get_Wow64RevertWow64FsRedirection())
		{
			return pWow64RevertWow64FsRedirection(OlValue);
		}


		SetLastError(ERROR_INVALID_FUNCTION);

		return FALSE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03)

	//Windows XP Professional x64 Edition, Windows Server 2003
	__DEFINE_THUNK(
	kernel32,
	4,
	BOOLEAN,
	WINAPI,
	Wow64EnableWow64FsRedirection,
		_In_ BOOLEAN Wow64FsEnableRedirection
		)
	{
		if (auto const pWow64EnableWow64FsRedirection = try_get_Wow64EnableWow64FsRedirection())
		{
			return pWow64EnableWow64FsRedirection(Wow64FsEnableRedirection);
		}


		SetLastError(ERROR_INVALID_FUNCTION);

		return FALSE;

	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WS03SP1)

	//Windows XP with SP2, Windows Server 2003 with SP1
	__DEFINE_THUNK(
	kernel32,
	8,
	BOOL,
	WINAPI,
	IsWow64Process,
		_In_ HANDLE hProcess,
		_Out_ PBOOL Wow64Process
		)
	{
		if (auto const pIsWow64Process = try_get_IsWow64Process())
		{
			return pIsWow64Process(hProcess, Wow64Process);
		}


		*Wow64Process = FALSE;

		return TRUE;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS3) && (defined _X86_ || defined _AMD64_)

	//Windows 10, Version 1511
	//微软文档有点问题，实际x86以及amd64系统中，16299（RS3）才开始有此API。
	__DEFINE_THUNK(
	kernel32,
	12,
	_Success_(return)
	BOOL,
	WINAPI,
	IsWow64Process2,
		_In_      HANDLE  hProcess,
		_Out_     USHORT* pProcessMachine,
		_Out_opt_ USHORT* pNativeMachine
		)
	{
		if (auto const pIsWow64Process2 = try_get_IsWow64Process2())
		{
			return pIsWow64Process2(hProcess, pProcessMachine, pNativeMachine);
		}


		//判断是否运行在Wow6432虚拟机
		BOOL bWow64Process;
		auto bRet = IsWow64Process(hProcess, &bWow64Process);

		if (bRet)
		{
			if (bWow64Process)
			{
				*pProcessMachine = IMAGE_FILE_MACHINE_I386;

				//IA64已经哭晕在厕所
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
	#error 不支持此体系
#endif
			}
		}

		return bRet;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN10_RS3) && (defined _X86_ || defined _AMD64_)

	//Windows 10, version 1709
	__DEFINE_THUNK(
	kernel32,
	8,
	_Must_inspect_result_
	HRESULT,
	WINAPI,
	IsWow64GuestMachineSupported,
		_In_ USHORT WowGuestMachine,
		_Out_ BOOL* MachineIsSupported
		)
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
			GetNativeSystemInfo(&SystemInfo);

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


#if (YY_Thunks_Support_Version < NTDDI_WS03)

    // 实际看到Vista就有，推测这个接口是x64系统时代添加的。
    __DEFINE_THUNK(
    kernel32,
    8,
    BOOL,
    WINAPI,
    Wow64GetThreadContext,
        _In_ HANDLE         _hThread,
        _Out_ PWOW64_CONTEXT _pContext
        )
    {
        if (const auto _pfnWow64GetThreadContext = try_get_Wow64GetThreadContext())
        {
            return _pfnWow64GetThreadContext(_hThread, _pContext);
        }

        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
#endif
} //namespace YY::Thunks
