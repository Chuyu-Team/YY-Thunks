#include <winhttp.h>

#if (YY_Thunks_Support_Version < NTDDI_WIN8)
#pragma comment(lib, "winhttp.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Support_Version < NTDDI_WIN8) && defined(YY_Thunks_Implemented)
    namespace Fallback
    {
        namespace 
        {
            struct WinHttpProxyResolver
            {
                struct Request
                {
                    ULONG uRef;
                    WINHTTP_PROXY_INFO ProxyInfo;

                    WinHttpProxyResolver* pProxyResolver;
                    DWORD_PTR pContext;
                    WINHTTP_AUTOPROXY_OPTIONS AutoProxyOptions;
                    wchar_t szUrl[0];

                    void AddRef()
                    {
                        InterlockedIncrement(&uRef);
                    }

                    void Release()
                    {
                        if (InterlockedDecrement(&uRef) == 0)
                        {
                            if (ProxyInfo.lpszProxy)
                                GlobalFree(ProxyInfo.lpszProxy);
                            if (ProxyInfo.lpszProxyBypass)
                                GlobalFree(ProxyInfo.lpszProxyBypass);
                            internal::Free(this);
                        }
                    }
                };

                static constexpr DWORD kMagic = 696;
                DWORD uMagic = kMagic;
                DWORD uRef = 1;
                SRWLOCK Lock = {};
                HINTERNET hSession = NULL;
                HINTERNET hConnect = NULL;
                WINHTTP_STATUS_CALLBACK pfnGetProxyForUrlCallback = nullptr;
                DWORD fNotificationFlags = 0;
                Request* pResult = nullptr;

                void AddRef()
                {
                    InterlockedIncrement(&uRef);
                }

                void Release()
                {
                    if (InterlockedDecrement(&uRef) == 0)
                    {
                        auto _pResult = (WinHttpProxyResolver::Request*)InterlockedExchange((volatile uintptr_t*)&pResult, (uintptr_t)nullptr);
                        if (_pResult)
                        {
                            _pResult->Release();
                        }

                        ::WinHttpCloseHandle(hConnect);
                        internal::Delete(this);
                    }
                }

                Request* GetResult()
                {
                    ::AcquireSRWLockShared(&Lock);
                    auto _p = pResult;
                    _p->AddRef();
                    ::ReleaseSRWLockShared(&Lock);
                    return _p;
                }

                void SetResult(Request* _pResult)
                {
                    if (_pResult)
                        _pResult->AddRef();
                    ::AcquireSRWLockExclusive(&Lock);
                    auto _p = (WinHttpProxyResolver::Request*)InterlockedExchange((volatile uintptr_t*)&pResult, (uintptr_t)_pResult);
                    ::ReleaseSRWLockExclusive(&Lock);
                    if (_p)
                    {
                        _p->Release();
                    }
                }
            };

            template<typename Type>
            bool Is(HINTERNET _hSession) noexcept;

            template<>
            bool __fastcall Is<WinHttpProxyResolver>(HINTERNET _hSession) noexcept
            {
                return _hSession && reinterpret_cast<WinHttpProxyResolver*>(_hSession)->uMagic == WinHttpProxyResolver::kMagic;
            }

            // 代理格式： [<scheme>=][<scheme>"://"]<server>[":"<port>]
            struct ProxyResultEntry
            {
                bool            bProxy = false;
                bool            bBypass = false;
                INTERNET_PORT   ProxyPort = 0;
                INTERNET_SCHEME uProxyScheme = 0;
                LPCWSTR         szProxy = nullptr;
                size_t          cchProxy = 0;

            public:
                bool __fastcall Parse(LPCWSTR* _pszProxy)
                {
                    if (!ParseProxyScheme(_pszProxy))
                        return false;

                    if (!ParseProxyServerAndPort(_pszProxy))
                        return false;

                    return true;
                }

                static size_t __fastcall GetMaxBufferCount(_In_opt_z_ LPCWSTR _szProxy)
                {
                    size_t _cbMaxBuffer = sizeof(WINHTTP_PROXY_RESULT_ENTRY) + sizeof(wchar_t);
                    if (_szProxy)
                    {
                        for (; *_szProxy; ++_szProxy, _cbMaxBuffer += sizeof(wchar_t))
                        {
                            if (*_szProxy == L';')
                            {
                                _cbMaxBuffer += sizeof(WINHTTP_PROXY_RESULT_ENTRY) + sizeof(wchar_t);
                            }

                        }
                    }

                    return _cbMaxBuffer;
                }

                void To(_Out_ WINHTTP_PROXY_RESULT_ENTRY* _pEntry, WCHAR** _pszBuffer)
                {
                    auto _szBuffer = *_pszBuffer;

                    _pEntry->fProxy = bProxy;
                    _pEntry->fBypass = bBypass;
                    _pEntry->ProxyScheme = uProxyScheme;
                    _pEntry->ProxyPort = ProxyPort;
                    if (cchProxy)
                    {
                        --_szBuffer;
                        *_szBuffer = L'\0';
                        _szBuffer -= cchProxy;
                        _pEntry->pwszProxy = _szBuffer;
                        memcpy(_szBuffer, szProxy, cchProxy * sizeof(_szBuffer[0]));
                    }
                    else
                    {
                        _pEntry->pwszProxy = nullptr;
                    }
                    *_pszBuffer = _szBuffer;
                }

            private:
                bool __fastcall ParseProxyScheme(LPCWSTR* _pszProxy) noexcept
                {
                    auto _szProxy = *_pszProxy;
                    for (; *_szProxy == L' '; ++_szProxy);

                    // scheme: http HTTPS FTP SOCKS
                    switch (__ascii_towlower(*_szProxy))
                    {
                    case 'h':
                        ++_szProxy;
                        if (__ascii_towlower(*_szProxy) != L't')
                        {
                            return false;
                        }
                        ++_szProxy;
                        if (__ascii_towlower(*_szProxy) != L't')
                        {
                            return false;
                        }
                        ++_szProxy;
                        if (__ascii_towlower(*_szProxy) != L'p')
                        {
                            return false;
                        }
                        ++_szProxy;
                        if (__ascii_towlower(*_szProxy) == L's')
                        {
                            ++_szProxy;
                            uProxyScheme = INTERNET_SCHEME_HTTPS;
                        }
                        else
                        {
                            uProxyScheme = INTERNET_SCHEME_HTTP;
                        }
                        break;
                    case 'f':
                        // Windows的PAC好像本身就不支持FTP，代码先留着吧。
                        ++_szProxy;
                        if (__ascii_towlower(*_szProxy) != L't')
                        {
                            return false;
                        }
                        ++_szProxy;
                        if (__ascii_towlower(*_szProxy) != L'p')
                        {
                            return false;
                        }
                        ++_szProxy;
                        uProxyScheme = INTERNET_SCHEME_FTP;
                        break;
                    case 's':
                        ++_szProxy;
                        if (__ascii_towlower(*_szProxy) != L'o')
                        {
                            return false;
                        }
                        ++_szProxy;
                        if (__ascii_towlower(*_szProxy) != L'c')
                        {
                            return false;
                        }
                        ++_szProxy;
                        if (__ascii_towlower(*_szProxy) != L'k')
                        {
                            return false;
                        }
                        ++_szProxy;
                        if (__ascii_towlower(*_szProxy) != L's')
                        {
                            return false;
                        }
                        ++_szProxy;
                        uProxyScheme = INTERNET_SCHEME_SOCKS;
                        break;
                    default:
                        // 默认认为是http
                        uProxyScheme = INTERNET_SCHEME_HTTP;
                        goto __Exit;
                        break;
                    }

                    if (*_szProxy == L'=')
                    {
                        ++_szProxy;
                    }
                    else if (*_szProxy == L':')
                    {
                        ++_szProxy;
                        if (*_szProxy != L'/')
                        {
                            return false;
                        }
                        ++_szProxy;
                        if (*_szProxy != L'/')
                        {
                            return false;
                        }
                        ++_szProxy;
                    }

                __Exit:
                    *_pszProxy = _szProxy;
                    return true;
                }

                bool __fastcall ParseProxyServerAndPort(LPCWSTR* _pszProxy) noexcept
                {
                    auto _szProxy = *_pszProxy;
                    for (; *_szProxy == L' '; ++_szProxy);

                    szProxy = _szProxy;

                    for (; ; ++_szProxy)
                    {
                        if (*_szProxy == L'\0' || *_szProxy == ';')
                        {
                            cchProxy = _szProxy - szProxy;
                            if (cchProxy == 0)
                                return false;

                            switch (uProxyScheme)
                            {
                            case INTERNET_SCHEME_HTTP:
                                ProxyPort = INTERNET_DEFAULT_HTTP_PORT;
                                break;
                            case INTERNET_SCHEME_HTTPS:
                                // 实际微软的API测试函数就算是HTTPS默认端口也是80
                                // 具体原因不明，我们就先跟微软保持一致吧。
                                ProxyPort = /*INTERNET_DEFAULT_HTTPS_PORT*/INTERNET_DEFAULT_HTTP_PORT;
                                break;
                            case INTERNET_SCHEME_FTP:
                                ProxyPort = 21;
                                break;
                            case INTERNET_SCHEME_SOCKS:
                                ProxyPort = 1080;
                                break;
                            default:
                                return false;
                                break;
                            }

                            if (*_szProxy == ';')
                                ++_szProxy;

                            break;
                        }
                        else if (*_szProxy == ':')
                        {
                            cchProxy = _szProxy - szProxy;
                            if (cchProxy == 0)
                                return false;
                            ++_szProxy;
                            if (!ParseProxyPort(&_szProxy))
                                return false;
                            break;
                        }
                    }

                    *_pszProxy = _szProxy;
                    return true;
                }

                bool __fastcall ParseProxyPort(LPCWSTR* _pszProxy) noexcept
                {
                    auto _szProxy = *_pszProxy;
                    for (; *_szProxy == L' '; ++_szProxy);

                    DWORD _uPort = 0;
                    if (!internal::StringToUint32(_szProxy, &_uPort, &_szProxy))
                        return false;

                    if (_uPort > MAXUINT16)
                        return false;

                    for (; *_szProxy == L' '; ++_szProxy);
                    if (*_szProxy == ';')
                    {
                        ++_szProxy;
                    }
                    else if (*_szProxy != '\0')
                    {
                        return false;
                    }

                    ProxyPort = static_cast<UINT16>(_uPort);
                    *_pszProxy = _szProxy;
                    return true;
                }
            };
        }
    }
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN8)

	// 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
	__DEFINE_THUNK(
	winhttp,
	8,
	DWORD,
    WINAPI,
    WinHttpCreateProxyResolver,
        _In_ HINTERNET _hSession,
        _Out_ HINTERNET* _phResolver
        )
	{
		if (const auto _pfnWinHttpCreateProxyResolver = try_get_WinHttpCreateProxyResolver())
		{
			return _pfnWinHttpCreateProxyResolver(_hSession, _phResolver);
		}
            
        // WinHttpConnect接口不会实际访问网络，这里调用以下主要是2个目的
        // 1. 维持 _hSession 的引用计数，避免hResolver存在期间调用者就主动关闭了hSession。
        // 2. 顺道还可以检测以下这个hSession是否合法。
        auto _hConnect = ::WinHttpConnect(_hSession, L"127.0.0.1", INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (NULL == _hConnect)
        {
            return GetLastError();
        }

        auto _pResolver = internal::New<Fallback::WinHttpProxyResolver>();
        if (!_pResolver)
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }
        _pResolver->hSession = _hSession;
        _pResolver->hConnect = _hConnect;
        *_phResolver = reinterpret_cast<HINTERNET>(_pResolver);
		return ERROR_SUCCESS;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

	// 最低受支持的客户端	Windows XP、Windows 2000 Professional SP3 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2003、Windows 2000 Server SP3[仅限桌面应用]
    // 1. 额外修正了Windows 8新增的ProxyResolver特性。
	__DEFINE_THUNK(
	winhttp,
	4,
	BOOL,
    WINAPI,
    WinHttpCloseHandle,
        IN HINTERNET _hInternet
        )
	{
        if (!Fallback::Is<Fallback::WinHttpProxyResolver>(_hInternet))
        {
            if (const auto _pfnWinHttpCloseHandle = try_get_WinHttpCloseHandle())
            {
                return _pfnWinHttpCloseHandle(_hInternet);
            }
            else
            {
                SetLastError(ERROR_FUNCTION_FAILED);
                return FALSE;
            }
        }
			
        reinterpret_cast<Fallback::WinHttpProxyResolver*>(_hInternet)->Release();
        return TRUE;            
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

	// 最低受支持的客户端	Windows XP、Windows 2000 Professional 和 SP3 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2003、Windows 2000 Server SP3[仅限桌面应用]
    // 1. 额外修正了Windows 8新增的ProxyResolver特性。
	__DEFINE_THUNK(
	winhttp,
	16,
	WINHTTP_STATUS_CALLBACK,
    WINAPI,
    WinHttpSetStatusCallback,
        IN HINTERNET _hInternet,
        IN WINHTTP_STATUS_CALLBACK _pfnInternetCallback,
        IN DWORD _fNotificationFlags,
        IN DWORD_PTR _uReserved
        )
	{
        if (!Fallback::Is<Fallback::WinHttpProxyResolver>(_hInternet))
        {
            if (const auto _pfnWinHttpSetStatusCallback = try_get_WinHttpSetStatusCallback())
            {
                return _pfnWinHttpSetStatusCallback(_hInternet, _pfnInternetCallback, _fNotificationFlags, _uReserved);
            }
            else
            {
                SetLastError(ERROR_FUNCTION_FAILED);
                return WINHTTP_INVALID_STATUS_CALLBACK;
            }
        }

        if (((WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE | WINHTTP_CALLBACK_FLAG_REQUEST_ERROR) & _fNotificationFlags) == 0)
        {
            SetLastError(ERROR_WINHTTP_INTERNAL_ERROR);
            return WINHTTP_INVALID_STATUS_CALLBACK;
        }

        // ProxyResolver修正代码
        auto _pResolver = reinterpret_cast<Fallback::WinHttpProxyResolver*>(_hInternet);
        _pResolver->fNotificationFlags = _fNotificationFlags;
        return (WINHTTP_STATUS_CALLBACK)InterlockedExchange((uintptr_t*)&_pResolver->pfnGetProxyForUrlCallback, (uintptr_t)_pfnInternetCallback);
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

	// 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
	__DEFINE_THUNK(
	winhttp,
	16,
	DWORD,
    WINAPI,
    WinHttpGetProxyForUrlEx,
        _In_ HINTERNET _hResolver,
        _In_ PCWSTR _szUrl,
        _In_ WINHTTP_AUTOPROXY_OPTIONS* _pAutoProxyOptions,
        _In_opt_ DWORD_PTR _pContext
        )
	{
		if (const auto _pfnWinHttpGetProxyForUrlEx = try_get_WinHttpGetProxyForUrlEx())
		{
			return _pfnWinHttpGetProxyForUrlEx(_hResolver, _szUrl, _pAutoProxyOptions, _pContext);
		}

        if (_szUrl == NULL)
            return ERROR_WINHTTP_INVALID_URL;

        if (!Fallback::Is<Fallback::WinHttpProxyResolver>(_hResolver))
            return ERROR_WINHTTP_INCORRECT_HANDLE_TYPE;

        auto _pResolver = reinterpret_cast<Fallback::WinHttpProxyResolver*>(_hResolver);

        const auto _cbUrl = (wcslen(_szUrl) + 1) * sizeof(WCHAR);

        auto _pRequest = (Fallback::WinHttpProxyResolver::Request*)internal::Alloc(sizeof(Fallback::WinHttpProxyResolver::Request) + _cbUrl, HEAP_ZERO_MEMORY);
        if (!_pRequest)
        {
            return ERROR_NOT_ENOUGH_MEMORY;
        }

        _pResolver->AddRef();
        _pRequest->uRef = 1;
        _pRequest->pProxyResolver = _pResolver;
        _pRequest->pContext = _pContext;
        _pRequest->AutoProxyOptions = *_pAutoProxyOptions;
        memcpy(_pRequest->szUrl, _szUrl, _cbUrl);

        auto _bRet = ::TrySubmitThreadpoolCallback(
            [](_Inout_ PTP_CALLBACK_INSTANCE Instance,
                _Inout_ PVOID Context)
            {
                auto _pRequest = reinterpret_cast<Fallback::WinHttpProxyResolver::Request*>(Context);
                if (WinHttpGetProxyForUrl(_pRequest->pProxyResolver->hSession, _pRequest->szUrl, &_pRequest->AutoProxyOptions, &_pRequest->ProxyInfo))
                {
                    if (_pRequest->pProxyResolver->fNotificationFlags & WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE)
                    {
                        _pRequest->pProxyResolver->SetResult(_pRequest);
                        _pRequest->pProxyResolver->pfnGetProxyForUrlCallback((HINTERNET)_pRequest->pProxyResolver, _pRequest->pContext, WINHTTP_CALLBACK_FLAG_GETPROXYFORURL_COMPLETE, nullptr, 0);
                    }
                }
                else
                {
                    if (_pRequest->pProxyResolver->fNotificationFlags & WINHTTP_CALLBACK_FLAG_REQUEST_ERROR)
                    {
                        WINHTTP_ASYNC_RESULT _AsyncResult = { API_GET_PROXY_FOR_URL, GetLastError()};
                        _pRequest->pProxyResolver->pfnGetProxyForUrlCallback((HINTERNET)_pRequest->pProxyResolver, _pRequest->pContext, WINHTTP_CALLBACK_FLAG_REQUEST_ERROR, &_AsyncResult, sizeof(_AsyncResult));
                    }
                }
                _pRequest->pProxyResolver->Release();
                _pRequest->Release();
            }, _pRequest, nullptr);

        if (!_bRet)
        {
            _pRequest->pProxyResolver->Release();
            _pRequest->Release();
            return ERROR_NOT_ENOUGH_MEMORY;
        }
            
		return ERROR_IO_PENDING;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

	// 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
	__DEFINE_THUNK(
	winhttp,
	8,
	DWORD,
    WINAPI,
    WinHttpGetProxyResult,
        _In_ HINTERNET _hResolver,
        _Out_ WINHTTP_PROXY_RESULT* _pProxyResult
        )
	{
        if (const auto _pfnWinHttpGetProxyResult = try_get_WinHttpGetProxyResult())
        {
            return _pfnWinHttpGetProxyResult(_hResolver, _pProxyResult);
        }

        if (!Fallback::Is<Fallback::WinHttpProxyResolver>(_hResolver))
        {
            return ERROR_WINHTTP_INCORRECT_HANDLE_TYPE;
        }

        auto _pResult = reinterpret_cast<Fallback::WinHttpProxyResolver*>(_hResolver)->GetResult();
        if (!_pResult)
        {
            return ERROR_WINHTTP_INCORRECT_HANDLE_STATE;
        }

        LSTATUS _lStatus = ERROR_SUCCESS;

        do
        {
            const auto _cbMaxBuffer = Fallback::ProxyResultEntry::GetMaxBufferCount(_pResult->ProxyInfo.lpszProxy) + Fallback::ProxyResultEntry::GetMaxBufferCount(_pResult->ProxyInfo.lpszProxyBypass);

            auto _pEntries = (WINHTTP_PROXY_RESULT_ENTRY*)internal::Alloc(_cbMaxBuffer);
            if (!_pEntries)
            {
                _lStatus = ERROR_NOT_ENOUGH_MEMORY;
                break;
            }

            auto _pBuffer = reinterpret_cast<wchar_t*>((uintptr_t)_pEntries + _cbMaxBuffer);

            DWORD _uCount = 0;
            Fallback::ProxyResultEntry _TmpEntrie;

            if (_pResult->ProxyInfo.dwAccessType == WINHTTP_ACCESS_TYPE_NO_PROXY)
            {
                auto& _pEntry = _pEntries[_uCount++];
                _pEntry.fProxy = FALSE;
                _pEntry.fBypass = TRUE;
                _pEntry.ProxyScheme = 0;
                _pEntry.pwszProxy = nullptr;
                _pEntry.ProxyPort = 0;
            }
            else if (_pResult->ProxyInfo.dwAccessType == WINHTTP_ACCESS_TYPE_NAMED_PROXY)
            {
                if (LPCWSTR _szProxy = _pResult->ProxyInfo.lpszProxy)
                {
                    for (; *_szProxy;)
                    {
                        _TmpEntrie.bProxy = true;
                        _TmpEntrie.bBypass = false;

                        if (_TmpEntrie.Parse(&_szProxy))
                        {
                            _TmpEntrie.To(&_pEntries[_uCount++], &_pBuffer);
                            continue;
                        }

                        // 解析失败，直接跳过到下一行
                        for (; *_szProxy; ++_szProxy)
                        {
                            if (*_szProxy == ';')
                            {
                                ++_szProxy;
                                break;
                            }
                        }
                    }
                }
            }

            _pProxyResult->cEntries = _uCount;
            _pProxyResult->pEntries = _pEntries;
        } while (false);
        _pResult->Release();

        return _lStatus;
	}
#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN8)

	// 最低受支持的客户端	Windows 8 [仅限桌面应用]
    // 最低受支持的服务器	Windows Server 2012[仅限桌面应用]
	__DEFINE_THUNK(
	winhttp,
	4,
	VOID,
    WINAPI,
    WinHttpFreeProxyResult,
        _Inout_ WINHTTP_PROXY_RESULT* _pProxyResult
        )
	{
		if (const auto _pfnWinHttpFreeProxyResult = try_get_WinHttpFreeProxyResult())
		{
			return _pfnWinHttpFreeProxyResult(_pProxyResult);
		}
            
        if(_pProxyResult)
            internal::Free(_pProxyResult->pEntries);
		return;
	}
#endif
}
