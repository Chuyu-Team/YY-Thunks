
namespace YY
{
	namespace Thunks
	{
#ifdef YY_Thunks_Implemented
		namespace Downlevel
		{
			//我们Windows 7以后才引入了组的概念，因此我们可以统一的假定 只有一组，并且不支持CPU热插拔。
			static
			DWORD
			WINAPI GetProcessorCount(
				_In_ WORD GroupNumber
				)
			{
				if (GroupNumber == 0 || GroupNumber == ALL_PROCESSOR_GROUPS)
				{
					SYSTEM_INFO SystemInfo;
					GetSystemInfo(&SystemInfo);

					return SystemInfo.dwNumberOfProcessors;
				}

				SetLastError(ERROR_INVALID_PARAMETER);

				return 0;
			}
		}
#endif

#if (YY_Thunks_Target < __WindowsNT6_1)

		//Minimum supported client	Windows 7 [desktop apps only]
		//Minimum supported server	Windows Server 2008 R2 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		4,
		DWORD,
		WINAPI,
		GetMaximumProcessorCount,
			_In_ WORD GroupNumber
			)
		{
			if (auto pGetMaximumProcessorCount = try_get_GetMaximumProcessorCount())
			{
				return pGetMaximumProcessorCount(GroupNumber);
			}

			return Downlevel::GetProcessorCount(GroupNumber);
		}
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

		//Minimum supported client	Windows 7 [desktop apps only]
		//Minimum supported server	Windows Server 2008 R2 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		4,
		DWORD,
		WINAPI,
		GetActiveProcessorCount,
			_In_ WORD GroupNumber
			)
		{
			if (auto pGetActiveProcessorCount = try_get_GetActiveProcessorCount())
			{
				return pGetActiveProcessorCount(GroupNumber);
			}

			return Downlevel::GetProcessorCount(GroupNumber);
		}
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

		//Minimum supported client	Windows 7 [desktop apps only]
		//Minimum supported server	Windows Server 2008 R2 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		0,
		WORD,
		WINAPI,
		GetActiveProcessorGroupCount,
			VOID
			)
		{
			if (auto pGetActiveProcessorGroupCount = try_get_GetActiveProcessorGroupCount())
			{
				return pGetActiveProcessorGroupCount();
			}

			//我们统一假定只有一组，事实也如此。
			return 1;
		}
#endif


#if (YY_Thunks_Target < __WindowsNT6_1)

		//Minimum supported client	Windows 7 [desktop apps only]
		//Minimum supported server	Windows Server 2008 R2 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		0,
		WORD,
		WINAPI,
		GetMaximumProcessorGroupCount,
			VOID
			)
		{
			if (auto pGetMaximumProcessorGroupCount = try_get_GetMaximumProcessorGroupCount())
			{
				return pGetMaximumProcessorGroupCount();
			}

			//我们统一假定只有一组，事实也如此。
			return 1;
		}
#endif
	}
}