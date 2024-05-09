
namespace YY
{
	namespace Thunks
	{
#ifdef YY_Thunks_Implemented
		namespace internal
		{
			/*LSTATUS __fastcall BasepGetModuleHandleExParameterValidation(
				_In_ DWORD dwFlags,
				_In_opt_ LPCSTR lpModuleName,
				_Out_ HMODULE* phModule
				)*/
#define BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule) \
				for (;;)                                                           \
				{                                                                  \
					if (phModule)                                                  \
					{                                                              \
						*phModule = nullptr;                                       \
						if ((dwFlags & ~(GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)) == 0                                         \
							|| (dwFlags & (GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)) != (GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)   \
							|| (lpModuleName || (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS) == 0))                                                                                                     \
						{                                                         \
							if (lpModuleName == nullptr)                          \
							{                                                     \
								*phModule = (HMODULE)((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->ImageBaseAddress; \
								return TRUE;                                      \
							}                                                     \
							break;                                                \
						}                                                         \
					}                                                             \
					SetLastError(ERROR_INVALID_PARAMETER);                        \
					return FALSE;                                                 \
				}
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WINXP)

		//Windows XP [desktop apps only]
		//Windows Server 2003 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		12,
		BOOL,
		WINAPI,
		GetModuleHandleExA,
			_In_ DWORD dwFlags,
			_In_opt_ LPCSTR lpModuleName,
			_Out_ HMODULE* phModule
			)
		{
			if (const auto pGetModuleHandleExA = try_get_GetModuleHandleExA())
			{
				return pGetModuleHandleExA(dwFlags, lpModuleName, phModule);
			}

			BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule);

			if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
			{
				EnterCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
			}

			LSTATUS lStatus = ERROR_SUCCESS;

			for (;;)
			{
				HMODULE hModule;

				if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
				{
					auto pRtlPcToFileHeader = try_get_RtlPcToFileHeader();
					if (!pRtlPcToFileHeader)
					{
						lStatus = ERROR_NOT_SUPPORTED;
						break;
					}

					hModule = (HMODULE)pRtlPcToFileHeader((PVOID)lpModuleName, (PVOID*)&hModule);
				}
				else
				{
					hModule = GetModuleHandleA(lpModuleName);
				}

				if (hModule == nullptr)
				{
					lStatus = ERROR_DLL_NOT_FOUND;
					break;
				}

				if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)
				{

				}
				else
				{
					const auto pLdrAddRefDll = try_get_LdrAddRefDll();
					if (!pLdrAddRefDll)
					{
						lStatus = ERROR_NOT_SUPPORTED;
						break;
					}

					auto Status = pLdrAddRefDll(dwFlags& GET_MODULE_HANDLE_EX_FLAG_PIN, hModule);
					if (Status < 0)
					{
						lStatus = internal::BaseSetLastNTError(Status);
						break;
					}
				}

				*phModule = hModule;

				break;
			}

			if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
			{
				LeaveCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
			}

			if (lStatus)
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

		//Windows XP [desktop apps only]
		//Windows Server 2003 [desktop apps only]
		__DEFINE_THUNK(
		kernel32,
		12,
		BOOL,
		WINAPI,
		GetModuleHandleExW,
			_In_ DWORD dwFlags,
			_In_opt_ LPCWSTR lpModuleName,
			_Out_ HMODULE* phModule
			)
		{
			if (const auto pGetModuleHandleExW = try_get_GetModuleHandleExW())
			{
				return pGetModuleHandleExW(dwFlags, lpModuleName, phModule);
			}

			BasepGetModuleHandleExParameterValidation(dwFlags, lpModuleName, phModule);

			if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
			{
				EnterCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
			}

			LSTATUS lStatus = ERROR_SUCCESS;

			for (;;)
			{
				HMODULE hModule;

				if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
				{
					const auto pRtlPcToFileHeader = try_get_RtlPcToFileHeader();
					if (!pRtlPcToFileHeader)
					{
						lStatus = ERROR_NOT_SUPPORTED;
						break;
					}

					hModule = (HMODULE)pRtlPcToFileHeader((PVOID)lpModuleName, (PVOID*)&hModule);
				}
				else
				{
					hModule = GetModuleHandleW(lpModuleName);
				}

				if (hModule == nullptr)
				{
					lStatus = ERROR_DLL_NOT_FOUND;
					break;
				}

				if (dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)
				{

				}
				else
				{
					const auto pLdrAddRefDll = try_get_LdrAddRefDll();
					if (!pLdrAddRefDll)
					{
						lStatus = ERROR_NOT_SUPPORTED;
						break;
					}

					auto Status = pLdrAddRefDll(dwFlags & GET_MODULE_HANDLE_EX_FLAG_PIN, hModule);
					if (Status < 0)
					{
						lStatus = internal::BaseSetLastNTError(Status);
						break;
					}
				}

				*phModule = hModule;

				break;
			}

			if ((dwFlags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT) == 0)
			{
				LeaveCriticalSection(((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock->LoaderLock);
			}

			if (lStatus)
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


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//虽然这个早就有了，但是只有Windows 8以及打了KB2533623补丁的系统才支持 LOAD_LIBRARY_SEARCH_SYSTEM32 等特性
		__DEFINE_THUNK(
		kernel32,
		12,
		_Ret_maybenull_
		HMODULE,
		WINAPI,
		LoadLibraryExW,
			_In_ LPCWSTR lpLibFileName,
			_Reserved_ HANDLE hFile,
			_In_ DWORD dwFlags
			)
		{
			const auto pLoadLibraryExW = try_get_LoadLibraryExW();

			if (!pLoadLibraryExW)
			{
				SetLastError(ERROR_FUNCTION_FAILED);
				return nullptr;
			}


			if (try_get_AddDllDirectory() != nullptr)
			{
				//存在AddDllDirectory说明支持 LOAD_LIBRARY_SEARCH_SYSTEM32 等功能，直接调用pLoadLibraryExW即可。

				return pLoadLibraryExW(lpLibFileName, hFile, dwFlags);
			}

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
			//Windows Vista开始才支持 LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE，对于不支持的系统我们只能Fallblack到 LOAD_LIBRARY_AS_DATAFILE
			if (dwFlags & (LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE))
			{
				auto pPeb = ((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock;

				if (pPeb->OSMajorVersion < 6)
				{
					dwFlags &= ~(LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
					dwFlags |= LOAD_LIBRARY_AS_DATAFILE;
				}
			}
#endif
			wchar_t szFilePathBuffer[1024];



			do
			{
				auto dwLoadLibrarySearchFlags = dwFlags & 0xFFFFFF00;

				if (dwLoadLibrarySearchFlags == 0)
				{
					break;
				}

				if (((LOAD_WITH_ALTERED_SEARCH_PATH | 0xFFFFE000 | 0x00000004) & dwFlags) || lpLibFileName == nullptr || hFile)
				{
					//LOAD_WITH_ALTERED_SEARCH_PATH 标记不允许跟其他标记组合使用
					//0xFFFFE000 为 其他不支持的数值
					//LOAD_PACKAGED_LIBRARY: 0x00000004 Windows 8以上平台才支持
					SetLastError(ERROR_INVALID_PARAMETER);
					return nullptr;
				}

				dwFlags &= 0xFF;

				//LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32 等价于 LOAD_LIBRARY_SEARCH_DEFAULT_DIRS标记
				if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)
					dwLoadLibrarySearchFlags = (dwLoadLibrarySearchFlags & ~LOAD_LIBRARY_SEARCH_DEFAULT_DIRS) | (LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32);



				if (dwLoadLibrarySearchFlags == (LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32))
				{
					//如果确定是调用默认体系，则直接调用原始 LoadLibraryExW

					break;
				}

				const auto pRtlDetermineDosPathNameType_U = try_get_RtlDetermineDosPathNameType_U();

				const auto PathType = pRtlDetermineDosPathNameType_U ? pRtlDetermineDosPathNameType_U(lpLibFileName) : RtlPathTypeUnknown;

				if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR)
				{
					//必须是一个完整路径！
					if (PathType == RtlPathTypeUnknown || PathType == RtlPathTypeDriveRelative || PathType == RtlPathTypeRelative)
					{
						SetLastError(ERROR_INVALID_PARAMETER);
						return nullptr;
					}

					if (dwLoadLibrarySearchFlags == (LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32))
					{
						//LOAD_WITH_ALTERED_SEARCH_PATH参数能模拟 LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_USER_DIRS | LOAD_LIBRARY_SEARCH_SYSTEM32 组合效果。
						dwFlags |= LOAD_WITH_ALTERED_SEARCH_PATH;
						break;
					}
				}		


				if (LOAD_LIBRARY_SEARCH_USER_DIRS & dwLoadLibrarySearchFlags)
				{
					//LOAD_LIBRARY_SEARCH_USER_DIRS 无法顺利实现，索性无效参数处理
					SetLastError(ERROR_INVALID_PARAMETER);
					return nullptr;
				}




				if (dwFlags & (LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE))
				{
					//以资源方式加载

					//判断路径是一个绝对路径还是一个相对路径，如果是绝对路径，那么可以直接无视 LOAD_LIBRARY_SEARCH_ 系列参数。
					if ((PathType == RtlPathTypeUnknown || PathType == RtlPathTypeDriveRelative || PathType == RtlPathTypeRelative) == false)
					{
						//是一个绝对路径，我们直接传递给 pLoadLibraryExW 即可

						break;
					}

					if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_APPLICATION_DIR)
					{
						auto nSize = GetModuleFileNameW(NULL, szFilePathBuffer, _countof(szFilePathBuffer));

						if (nSize == 0 || nSize >= _countof(szFilePathBuffer))
						{
							SetLastError(ERROR_FUNCTION_FAILED);
							return nullptr;
						}

						for (;;)
						{
							if (szFilePathBuffer[nSize] == L'\\' || szFilePathBuffer[nSize] == L'/')
							{
								++nSize;
								break;
							}

							if (nSize == 0)
							{
								SetLastError(ERROR_FUNCTION_FAILED);
								return nullptr;
							}

							--nSize;
						}


						for (auto Str = lpLibFileName; *Str; ++Str, ++nSize)
						{
							if (nSize >= _countof(szFilePathBuffer))
							{
								SetLastError(ERROR_FUNCTION_FAILED);
								return nullptr;
							}

							szFilePathBuffer[nSize] = *Str;
						}

						szFilePathBuffer[nSize] = L'\0';


						if (GetFileAttributesW(szFilePathBuffer) != -1)
						{
							lpLibFileName = szFilePathBuffer;
							break;
						}
					}

					if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_SYSTEM32)
					{
						auto nSize = GetSystemDirectoryW(szFilePathBuffer, _countof(szFilePathBuffer));

						if (nSize == 0 || nSize >= _countof(szFilePathBuffer))
						{
							SetLastError(ERROR_FUNCTION_FAILED);
							return nullptr;
						}

						if (szFilePathBuffer[nSize] != L'\\')
						{
							if (nSize >= _countof(szFilePathBuffer))
							{
								SetLastError(ERROR_FUNCTION_FAILED);
								return nullptr;
							}

							szFilePathBuffer[++nSize] = L'\\';
						}

						for (auto Str = lpLibFileName; *Str; ++Str, ++nSize)
						{
							if (nSize >= _countof(szFilePathBuffer))
							{
								SetLastError(ERROR_FUNCTION_FAILED);
								return nullptr;
							}

							szFilePathBuffer[nSize] = *Str;
						}

						szFilePathBuffer[nSize] = L'\0';

						if (GetFileAttributesW(szFilePathBuffer) != -1)
						{
							lpLibFileName = szFilePathBuffer;
							break;
						}
					}

					SetLastError(ERROR_MOD_NOT_FOUND);
					return nullptr;
				}


				//以模块方式加载

				const auto pLdrLoadDll = try_get_LdrLoadDll();
				if (!pLdrLoadDll)
				{
					SetLastError(ERROR_FUNCTION_FAILED);
					return nullptr;
				}

				DWORD nSize = 0;

				if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR)
				{
					for (auto Str = lpLibFileName; *Str; ++Str, ++nSize)
					{
						if (nSize >= _countof(szFilePathBuffer))
						{
							SetLastError(ERROR_FUNCTION_FAILED);
							return nullptr;
						}

						szFilePathBuffer[nSize] = *Str;
					}

					if (nSize == 0)
					{
						SetLastError(ERROR_FUNCTION_FAILED);
						return nullptr;
					}

					--nSize;
					//反向剔除文件名
					for (;;)
					{
						if (szFilePathBuffer[nSize] == L'\\' || szFilePathBuffer[nSize] == L'/')
						{
							break;
						}

						if (nSize == 0)
						{
							SetLastError(ERROR_FUNCTION_FAILED);
							return nullptr;
						}

						--nSize;
					}

					++nSize;
					szFilePathBuffer[nSize] = L';';
					++nSize;
				}

				if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_APPLICATION_DIR)
				{
					const DWORD nBufferMax = _countof(szFilePathBuffer) - nSize;

					auto nBuffer = GetModuleFileNameW(NULL, szFilePathBuffer + nSize, nBufferMax);

					if (nBuffer == 0 || nBuffer >= nBufferMax)
					{
						SetLastError(ERROR_FUNCTION_FAILED);
						return nullptr;
					}

					nSize += nBuffer - 1;

					for (;;)
					{
						if (szFilePathBuffer[nSize] == L'\\' || szFilePathBuffer[nSize] == L'/')
						{
							break;
						}

						if (nSize == 0)
						{
							SetLastError(ERROR_FUNCTION_FAILED);
							return nullptr;
						}

						--nSize;
					}

					++nSize;
					szFilePathBuffer[nSize] = L';';
					++nSize;
				}

				if (dwLoadLibrarySearchFlags & LOAD_LIBRARY_SEARCH_SYSTEM32)
				{
					const DWORD nBufferMax = _countof(szFilePathBuffer) - nSize;

					auto nBuffer = GetSystemDirectoryW(szFilePathBuffer + nSize, nBufferMax);

					if (nBuffer == 0 || nBuffer >= nBufferMax)
					{
						SetLastError(ERROR_FUNCTION_FAILED);
						return nullptr;
					}

					nSize += nBuffer;
				}

				szFilePathBuffer[nSize] = L'\0';

				UNICODE_STRING ModuleFileName;
				ModuleFileName.Buffer = (PWSTR)lpLibFileName;

				for (; *lpLibFileName; ++lpLibFileName);
                const auto _uNewLength = (lpLibFileName - ModuleFileName.Buffer) * sizeof(lpLibFileName[0]);
                if (_uNewLength + sizeof(lpLibFileName[0]) > MAXUINT16)
                {
                    SetLastError(ERROR_INVALID_PARAMETER);
                    return nullptr;
                }

				ModuleFileName.Length = static_cast<USHORT>(_uNewLength);
				ModuleFileName.MaximumLength = ModuleFileName.Length + sizeof(lpLibFileName[0]);

				HMODULE hModule = NULL;

				ULONG dwLdrLoadDllFlags = 0;

				if (dwFlags & DONT_RESOLVE_DLL_REFERENCES)
				{
					dwLdrLoadDllFlags |= 0x2;
				}

				if (dwFlags & LOAD_IGNORE_CODE_AUTHZ_LEVEL)
				{
					dwLdrLoadDllFlags |= 0x1000;
				}

				if (dwFlags & LOAD_LIBRARY_REQUIRE_SIGNED_TARGET)
				{
					dwLdrLoadDllFlags |= 0x800000;
				}

#if defined(_X86_) || defined(_M_IX86)
				//我们先关闭重定向，再加载DLL，Windows 7 SP1以前的系统不会关闭重定向，而导致某些线程关闭重定向后DLL加载问题。
				PVOID OldFsRedirectionLevel;

				auto pRtlWow64EnableFsRedirectionEx = try_get_RtlWow64EnableFsRedirectionEx();
				auto StatusFsRedir = pRtlWow64EnableFsRedirectionEx ? pRtlWow64EnableFsRedirectionEx(nullptr, &OldFsRedirectionLevel) : 0;
#endif

				auto Status = pLdrLoadDll(szFilePathBuffer, &dwLdrLoadDllFlags, &ModuleFileName, &hModule);

#if defined(_X86_) || defined(_M_IX86)
				if (StatusFsRedir >= 0 && pRtlWow64EnableFsRedirectionEx)
					pRtlWow64EnableFsRedirectionEx(OldFsRedirectionLevel, &OldFsRedirectionLevel);
#endif
				if (Status < 0)
				{
					YY::Thunks::internal::BaseSetLastNTError(Status);
				}

				return hModule;
			} while (false);

#if defined(_X86_) || defined(_M_IX86)
			//我们先关闭重定向，再加载DLL，Windows 7 SP1以前的系统不会关闭重定向，而导致某些线程关闭重定向后DLL加载问题。
			PVOID OldFsRedirectionLevel;

			auto pRtlWow64EnableFsRedirectionEx = try_get_RtlWow64EnableFsRedirectionEx();
			auto StatusFsRedir = pRtlWow64EnableFsRedirectionEx ? pRtlWow64EnableFsRedirectionEx(nullptr, &OldFsRedirectionLevel) : 0;
#endif

			auto hModule = pLoadLibraryExW(lpLibFileName, hFile, dwFlags);

#if defined(_X86_) || defined(_M_IX86)
			if (StatusFsRedir >= 0 && pRtlWow64EnableFsRedirectionEx)
			{
				LSTATUS lStatus = GetLastError();
				pRtlWow64EnableFsRedirectionEx(OldFsRedirectionLevel, &OldFsRedirectionLevel);
				SetLastError(lStatus);
			}
#endif
			return hModule;
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

		//虽然这个早就有了，但是只有Windows 8以及打了KB2533623补丁的系统才支持 LOAD_LIBRARY_SEARCH_SYSTEM32 等特性
		__DEFINE_THUNK(
		kernel32,
		12,
		_Ret_maybenull_
		HMODULE,
		WINAPI,
		LoadLibraryExA,
			_In_ LPCSTR lpLibFileName,
			_Reserved_ HANDLE hFile,
			_In_ DWORD dwFlags
			)
		{
			const auto pLoadLibraryExA = try_get_LoadLibraryExA();

			if (!pLoadLibraryExA)
			{
				SetLastError(ERROR_FUNCTION_FAILED);
				return nullptr;
			}


			if (try_get_AddDllDirectory() != nullptr)
			{
				//存在AddDllDirectory说明支持 LOAD_LIBRARY_SEARCH_SYSTEM32 等功能，直接调用pLoadLibraryExW即可。

				return pLoadLibraryExA(lpLibFileName, hFile, dwFlags);
			}

			wchar_t szLibFileNameUnicode[512];

			UNICODE_STRING usLibFileName = { 0, sizeof(szLibFileNameUnicode), szLibFileNameUnicode };

			auto lStatus = internal::Basep8BitStringToStaticUnicodeString(&usLibFileName, lpLibFileName);
			if (lStatus != ERROR_SUCCESS)
			{
				SetLastError(lStatus);
				return nullptr;
			}

			return LoadLibraryExW(szLibFileNameUnicode, hFile, dwFlags);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// Windows Vista [仅限桌面应用], Windows Server 2008 [仅限桌面应用]
		__DEFINE_THUNK(
		kernel32,
		28,
		BOOL,
		APIENTRY,
		EnumResourceLanguagesExW,
			_In_opt_ HMODULE _hModule,
			_In_ LPCWSTR _lpType,
			_In_ LPCWSTR _lpName,
			_In_ ENUMRESLANGPROCW _lpEnumFunc,
			_In_opt_ LONG_PTR _lParam,
			DWORD _dwFlags,
			LANGID _LangId
			)
		{
			const auto _pfnEnumResourceLanguagesExW = try_get_EnumResourceLanguagesExW();

			if (_pfnEnumResourceLanguagesExW)
			{
				return _pfnEnumResourceLanguagesExW(_hModule, _lpType, _lpName, _lpEnumFunc, _lParam, _dwFlags, _LangId);
			}

			// WinXP不支持MUI，故而忽略 _dwFlags、_LangId
			return EnumResourceLanguagesW(_hModule, _lpType, _lpName, _lpEnumFunc, _lParam);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)

		// Windows Vista [仅限桌面应用], Windows Server 2008 [仅限桌面应用]
		__DEFINE_THUNK(
		kernel32,
		28,
		BOOL,
		APIENTRY,
		EnumResourceLanguagesExA,
			_In_opt_ HMODULE _hModule,
			_In_ LPCSTR _lpType,
			_In_ LPCSTR _lpName,
			_In_ ENUMRESLANGPROCA _lpEnumFunc,
			_In_opt_ LONG_PTR _lParam,
			DWORD _dwFlags,
			LANGID _LangId
			)
		{
			const auto _pfnEnumResourceLanguagesExA = try_get_EnumResourceLanguagesExA();

			if (_pfnEnumResourceLanguagesExA)
			{
				return _pfnEnumResourceLanguagesExA(_hModule, _lpType, _lpName, _lpEnumFunc, _lParam, _dwFlags, _LangId);
			}

			// WinXP不支持MUI，故而忽略 _dwFlags、_LangId
			return EnumResourceLanguagesA(_hModule, _lpType, _lpName, _lpEnumFunc, _lParam);
		}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN7)

		// 最低受支持的客户端	Windows 7 [桌面应用 |UWP 应用]
        // 最低受支持的服务器	Windows Server 2008 R2[桌面应用 | UWP 应用]
		__DEFINE_THUNK(
		kernel32,
		24,
        int,
        WINAPI,
        FindStringOrdinal,
            _In_ DWORD _uFindStringOrdinalFlags,
            _In_reads_(_cchSource) LPCWSTR _pStringSource,
            _In_ int _cchSource,
            _In_reads_(_cchValue) LPCWSTR _pStringValue,
            _In_ int _cchValue,
            _In_ BOOL _bIgnoreCase
			)
		{
			if (const auto _pfnFindStringOrdinal = try_get_FindStringOrdinal())
			{
				return _pfnFindStringOrdinal(_uFindStringOrdinalFlags, _pStringSource, _cchSource, _pStringValue, _cchValue, _bIgnoreCase);
			}

            SetLastError(ERROR_SUCCESS);
            if (_pStringSource == nullptr || _cchSource < -1 || _pStringValue == nullptr || _cchValue < -1)
            {
                SetLastError(ERROR_INVALID_PARAMETER);
                return -1;
            }

            if (_cchSource == -1)
            {
                _cchSource = (int)wcslen(_pStringSource);
            }

            if (_cchSource == 0)
            {
                return -1;
            }

            if (_cchValue == -1)
            {
                _cchValue = (int)wcslen(_pStringValue);
            }

            if (_cchValue == 0 || _cchValue > _cchSource)
            {
                return -1;
            }

            switch (_uFindStringOrdinalFlags)
            {
            case 0:
            case FIND_FROMSTART:
                for (auto _pStart = _pStringSource; _cchValue <= _cchSource;++_pStart, --_cchSource)
                {
                    if (CompareStringOrdinal(_pStart, _cchValue, _pStringValue, _cchValue, _bIgnoreCase) == CSTR_EQUAL)
                    {
                        return static_cast<int>(_pStart - _pStringSource);
                    }
                }
                return -1;
                break;
            case FIND_FROMEND:
                for (auto _pStart = _pStringSource + _cchSource - _cchValue; _cchValue <= _cchSource; --_pStart, --_cchSource)
                {
                    if (CompareStringOrdinal(_pStart, _cchValue, _pStringValue, _cchValue, _bIgnoreCase) == CSTR_EQUAL)
                    {
                        return static_cast<int>(_pStart - _pStringSource);
                    }
                }
                return -1;
                break;
            case FIND_STARTSWITH:
                if (CompareStringOrdinal(_pStringSource, _cchValue, _pStringValue, _cchValue, _bIgnoreCase) == CSTR_EQUAL)
                {
                    return 0;
                }
                return -1;
                break;
            case FIND_ENDSWITH:
                _cchSource -= _cchValue;
                if (CompareStringOrdinal(_pStringSource + _cchSource, _cchValue, _pStringValue, _cchValue, _bIgnoreCase) == CSTR_EQUAL)
                {
                    return _cchSource;
                }
                return -1;
                break;
            default:
                SetLastError(ERROR_INVALID_FLAGS);
                return -1;
                break;
            }
		}
#endif
	}
}
