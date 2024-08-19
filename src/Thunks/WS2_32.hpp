#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef FreeAddrInfoEx
#undef FreeAddrInfoEx
#endif // ! FreeAddrInfoEx

namespace YY::Thunks
{
    namespace
    {
#if defined(YY_Thunks_Implemented) && YY_Thunks_Target < __WindowsNT6_2
        struct GetAddrInfoExParameter
        {
            // Parameter 的引用计数，如果引用归 0 则释放内存。
            volatile ULONG nRef;
            volatile LONG fFlags;
            enum
            {
                // 访问 pHandle 成员时必须持有 HandleLock。
                HandleLock = 0,
                // 通知已经完成，这时无法在访问 pHandle。
                HasSignalCompleted,
            };

            DWORD uNameSpace;
            LPGUID pNspId;
            timeval* pTimeout;
            LPOVERLAPPED pOverlapped;
            LPLOOKUPSERVICE_COMPLETION_ROUTINE pfnCompletionRoutine;
            uintptr_t* pHandle;

            GUID NspIdBuffer;
            timeval TimeoutBuffer;

            void __fastcall SignalCompleted(DWORD _lStatus)
            {
                if (!InterlockedBitTestAndSet(&fFlags, HasSignalCompleted))
                {
                    if (pHandle)
                    {
                        for (;;)
                        {
                            if (!InterlockedBitTestAndSet(&fFlags, HandleLock))
                            {
                                InterlockedExchange(pHandle, _lStatus);
                                InterlockedBitTestAndReset(&fFlags, HandleLock);
                                break;
                            }
                            YieldProcessor();
                        }
                    }

                    if (pOverlapped)
                    {
                        pOverlapped->Internal = _lStatus;
                    }

                    if (pfnCompletionRoutine)
                    {
                        pfnCompletionRoutine(_lStatus, 0, pOverlapped);
                    }

                    if (pOverlapped && pOverlapped->hEvent)
                        SetEvent(pOverlapped->hEvent);
                }

            }

            void __fastcall AddRef()
            {
                InterlockedIncrement(&nRef);
            }

            void __fastcall Release()
            {
                if (InterlockedDecrement(&nRef) == 0)
                {
                    internal::Free(this);
                }
            }

            LSTATUS __fastcall UpdateHandleStatus(LSTATUS _lStatus)
            {
                // 访问pHandle时必须持有 HandleLock 状态。
                for (auto _uLast = fFlags;;)
                {
                    if (_uLast & (1 << HasSignalCompleted))
                    {
                        return WSA_E_CANCELLED;
                    }

                    if (_uLast & (1 << HandleLock))
                    {
                        YieldProcessor();
                        _uLast = fFlags;
                        continue;
                    }

                    const auto _uResult = InterlockedCompareExchange(&fFlags, _uLast | (1 << HandleLock), _uLast);
                    if (_uResult == _uLast)
                        break;

                    _uLast = _uResult;
                }
                uintptr_t _uResult = 0;
                if (pHandle)
                {
                    _uResult = InterlockedCompareExchange(pHandle, _lStatus, uintptr_t(this));
                }
                InterlockedBitTestAndReset(&fFlags, HandleLock);

                if (uintptr_t(this) == _uResult)
                {
                    Release();
                    return ERROR_SUCCESS;
                }
                else
                {
                    if (_uResult <= MAXUINT16)
                    {
                        // 错误代码？看起来任务正在进行或者已经取消
                        return static_cast<LSTATUS>(_uResult);
                    }

                    return ERROR_FUNCTION_FAILED;
                }
            }
        };

        struct GetAddrInfoExWParameter : public GetAddrInfoExParameter
        {
            PCWSTR szName;
            PCWSTR szServiceName;

            const ADDRINFOEXW* pHints;
            PADDRINFOEXW* ppResult;

            ADDRINFOEXW HintsBuffer;

            //
            BYTE Others[0];
        };

        struct GetAddrInfoExAParameter : public GetAddrInfoExParameter
        {
            PCSTR szName;
            PCSTR szServiceName;

            const ADDRINFOEXA* pHints;
            PADDRINFOEXA* ppResult;

            ADDRINFOEXA HintsBuffer;

            BYTE Others[0];
        };

        static LSTATUS __fastcall Convert(_In_opt_ const ADDRINFOA* _pItem, _Outptr_ ADDRINFOW** _ppResult)
        {
            *_ppResult = nullptr;

            ADDRINFOW* _pResult = nullptr;
            for (ADDRINFOW* _pLast = nullptr; ; _pItem = _pItem->ai_next)
            {
                if (!_pItem)
                {
                    *_ppResult = _pResult;
                    return ERROR_SUCCESS;
                }
                const auto _cchCanonName = _pItem->ai_canonname ? strlen(_pItem->ai_canonname) : 0;
                auto _pTmp = (ADDRINFOW*)internal::Alloc(sizeof(ADDRINFOW) + (_cchCanonName + 1) * sizeof(wchar_t) + _pItem->ai_addrlen);
                if (!_pTmp)
                {
                    break;
                }
                BYTE* _pBuffer = (BYTE*)&_pTmp[1];

                if (_pLast)
                {
                    _pLast->ai_next = _pTmp;
                }
                else
                {
                    _pResult = _pTmp;
                }
                _pLast = _pTmp;

                _pTmp->ai_flags = _pItem->ai_flags;
                _pTmp->ai_family = _pItem->ai_family;
                _pTmp->ai_socktype = _pItem->ai_socktype;
                _pTmp->ai_protocol = _pItem->ai_protocol;
                _pTmp->ai_addrlen = _pItem->ai_addrlen;
                _pTmp->ai_canonname = nullptr;
                if (_pItem->ai_canonname)
                {
                    _pTmp->ai_canonname = (wchar_t*)_pBuffer;
                    const auto _cchCanonNameUTF16 = MultiByteToWideChar(CP_ACP, 0, _pItem->ai_canonname, _cchCanonName, _pTmp->ai_canonname, _cchCanonName);
                    _pTmp->ai_canonname[_cchCanonNameUTF16] = '\0';
                    _pBuffer += (_cchCanonNameUTF16 + 1) * sizeof(wchar_t);
                }
                _pTmp->ai_addr = nullptr;
                if (_pItem->ai_addr)
                {
                    _pTmp->ai_addr = (sockaddr*)_pBuffer;
                    memcpy(_pBuffer, _pItem->ai_addr, _pItem->ai_addrlen);
                    _pBuffer += _pItem->ai_addrlen;
                }
                _pTmp->ai_next = nullptr;
            }

            ::FreeAddrInfoW(_pResult);
            return WSA_NOT_ENOUGH_MEMORY;
        }

        static LSTATUS __fastcall Convert(_In_opt_ const ADDRINFOA* _pItem, _Outptr_ ADDRINFOEXW** _ppResult)
        {
            *_ppResult = nullptr;

            ADDRINFOEXW* _pResult = nullptr;
            for (ADDRINFOEXW* _pLast = nullptr; ; _pItem = _pItem->ai_next)
            {
                if (!_pItem)
                {
                    *_ppResult = _pResult;
                    return ERROR_SUCCESS;
                }
                const auto _cchCanonName = _pItem->ai_canonname ? strlen(_pItem->ai_canonname) : 0;
                auto _pTmp = (ADDRINFOEXW*)internal::Alloc(sizeof(ADDRINFOEXW) + (_cchCanonName + 1) * sizeof(wchar_t) + _pItem->ai_addrlen);
                if (!_pTmp)
                {
                    break;
                }
                BYTE* _pBuffer = (BYTE*)&_pTmp[1];

                if (_pLast)
                {
                    _pLast->ai_next = _pTmp;
                }
                else
                {
                    _pResult = _pTmp;
                }
                _pLast = _pTmp;

                _pTmp->ai_flags = _pItem->ai_flags;
                _pTmp->ai_family = _pItem->ai_family;
                _pTmp->ai_socktype = _pItem->ai_socktype;
                _pTmp->ai_protocol = _pItem->ai_protocol;
                _pTmp->ai_addrlen = _pItem->ai_addrlen;
                _pTmp->ai_canonname = nullptr;
                if (_pItem->ai_canonname)
                {
                    _pTmp->ai_canonname = (wchar_t*)_pBuffer;
                    const auto _cchCanonNameUTF16 = MultiByteToWideChar(CP_ACP, 0, _pItem->ai_canonname, _cchCanonName, _pTmp->ai_canonname, _cchCanonName);
                    _pTmp->ai_canonname[_cchCanonNameUTF16] = '\0';
                    _pBuffer += (_cchCanonNameUTF16 + 1) * sizeof(wchar_t);
                }
                _pTmp->ai_addr = nullptr;
                if (_pItem->ai_addr)
                {
                    _pTmp->ai_addr = (sockaddr*)_pBuffer;
                    memcpy(_pBuffer, _pItem->ai_addr, _pItem->ai_addrlen);
                    _pBuffer += _pItem->ai_addrlen;
                }
                _pTmp->ai_blob = nullptr;
                _pTmp->ai_bloblen = 0;
                _pTmp->ai_provider = nullptr;
                _pTmp->ai_next = nullptr;
            }

            ::FreeAddrInfoExW(_pResult);
            return WSA_NOT_ENOUGH_MEMORY;
        }

        static LSTATUS __fastcall Convert(_In_opt_ const ADDRINFOW* _pItem, _Outptr_ ADDRINFOEXW** _ppResult)
        {
            *_ppResult = nullptr;
            ADDRINFOEXW* _pResult = nullptr;

            for (ADDRINFOEXW* _pLast = nullptr; ; _pItem = _pItem->ai_next)
            {
                if (!_pItem)
                {
                    *_ppResult = _pResult;
                    return ERROR_SUCCESS;
                }

                const auto _cbCanonName = _pItem->ai_canonname ? (wcslen(_pItem->ai_canonname) + 1) * sizeof(_pItem->ai_canonname[0]) : 0;
                auto _pTmpEx = (ADDRINFOEXW*)internal::Alloc(sizeof(ADDRINFOEXW) + _cbCanonName + _pItem->ai_addrlen);
                if (!_pTmpEx)
                {
                    break;
                }
                auto _pBufffer = (BYTE*)&_pTmpEx[1];
                _pTmpEx->ai_flags = _pItem->ai_flags;
                _pTmpEx->ai_family = _pItem->ai_family;
                _pTmpEx->ai_socktype = _pItem->ai_socktype;
                _pTmpEx->ai_protocol = _pItem->ai_protocol;
                _pTmpEx->ai_addrlen = _pItem->ai_addrlen;
                if (_pLast)
                {
                    _pLast->ai_next = _pTmpEx;
                }
                else
                {
                    _pResult = _pTmpEx;
                }
                _pLast = _pTmpEx;

                _pTmpEx->ai_canonname = nullptr;
                if (_pItem->ai_canonname)
                {
                    _pTmpEx->ai_canonname = (PWSTR)_pBufffer;
                    memcpy(_pBufffer, _pItem->ai_canonname, _cbCanonName);
                    _pBufffer += _cbCanonName;
                }

                _pTmpEx->ai_addr = nullptr;
                if (_pItem->ai_addr)
                {
                    _pTmpEx->ai_addr = (sockaddr*)_pBufffer;
                    memcpy(_pBufffer, _pItem->ai_addr, _pItem->ai_addrlen);
                    _pBufffer += _pItem->ai_addrlen;
                }
                _pTmpEx->ai_blob = nullptr;
                _pTmpEx->ai_bloblen = 0;
                _pTmpEx->ai_provider = nullptr;
                _pTmpEx->ai_next = nullptr;
            }

            ::FreeAddrInfoExW(_pResult);
            return WSA_NOT_ENOUGH_MEMORY;
        }

        static LSTATUS __fastcall Convert(_In_opt_ const ADDRINFOA* _pItem, _Outptr_ ADDRINFOEXA** _ppResult)
        {
            *_ppResult = nullptr;

            ADDRINFOEXA* _pResult = nullptr;

            for (ADDRINFOEXA* _pLast = nullptr; ; _pItem = _pItem->ai_next)
            {
                if (!_pItem)
                {
                    *_ppResult = _pResult;
                    return ERROR_SUCCESS;
                }
                const auto _cbCanonName = _pItem->ai_canonname ? strlen(_pItem->ai_canonname) + 1 : 0;
                auto _pTmp = (ADDRINFOEXA*)internal::Alloc(sizeof(ADDRINFOEXA) + _cbCanonName + _pItem->ai_addrlen);
                if (!_pTmp)
                {
                    break;
                }
                BYTE* _pBuffer = (BYTE*)&_pTmp[1];

                if (_pLast)
                {
                    _pLast->ai_next = _pTmp;
                }
                else
                {
                    _pResult = _pTmp;
                }
                _pLast = _pTmp;

                _pTmp->ai_flags = _pItem->ai_flags;
                _pTmp->ai_family = _pItem->ai_family;
                _pTmp->ai_socktype = _pItem->ai_socktype;
                _pTmp->ai_protocol = _pItem->ai_protocol;
                _pTmp->ai_addrlen = _pItem->ai_addrlen;
                _pTmp->ai_canonname = nullptr;
                if (_pItem->ai_canonname)
                {
                    _pTmp->ai_canonname = (char*)_pBuffer;
                    memcpy(_pBuffer, _pItem->ai_canonname, _cbCanonName);
                    _pBuffer += _cbCanonName;
                }
                _pTmp->ai_addr = nullptr;
                if (_pItem->ai_addr)
                {
                    _pTmp->ai_addr = (sockaddr*)_pBuffer;
                    memcpy(_pBuffer, _pItem->ai_addr, _pItem->ai_addrlen);
                    _pBuffer += _pItem->ai_addrlen;
                }
                _pTmp->ai_blob = nullptr;
                _pTmp->ai_bloblen = 0;
                _pTmp->ai_provider = nullptr;
                _pTmp->ai_next = nullptr;
            }

            ::FreeAddrInfoEx(_pResult);
            return WSA_NOT_ENOUGH_MEMORY;
        }

#if (YY_Thunks_Target < __WindowsNT6)
        static INT WSAAPI GetAddrInfoExWDownlevel(
            _In_opt_    PCWSTR          _szName,
            _In_opt_    PCWSTR          _szServiceName,
            _In_        DWORD           _uNameSpace,
            _In_opt_    LPGUID          _pNspId,
            _In_opt_    const ADDRINFOEXW* _pHintsEx,
            _Outptr_    PADDRINFOEXW* _ppResultEx,
            _In_opt_    timeval* _pTimeout,
            _In_opt_    LPOVERLAPPED    _pOverlapped,
            _In_opt_    LPLOOKUPSERVICE_COMPLETION_ROUTINE  _pfnCompletionRoutine,
            _Out_opt_   LPHANDLE        _pHandle
        )
        {
            if (_pHandle)
                *_pHandle = nullptr;

            *_ppResultEx = nullptr;

            if (_pHintsEx && (_pHintsEx->ai_addrlen || _pHintsEx->ai_canonname || _pHintsEx->ai_addr || _pHintsEx->ai_next))
            {
                return WSANO_RECOVERY;
            }

            ADDRINFOW _Hints;
            if (_pHintsEx)
            {
                _Hints.ai_flags = _pHintsEx->ai_flags;
                _Hints.ai_family = _pHintsEx->ai_family;
                _Hints.ai_socktype = _pHintsEx->ai_socktype;
                _Hints.ai_protocol = _pHintsEx->ai_protocol;
                _Hints.ai_addrlen = 0;
                _Hints.ai_canonname = nullptr;
                _Hints.ai_addr = nullptr;
                _Hints.ai_next = nullptr;
            }

            PADDRINFOEXW _pResultEx = nullptr;

#if (YY_Thunks_Target < __WindowsNT5_1_SP2)
            auto const GetAddrInfoW = try_get_GetAddrInfoW();
            if (!GetAddrInfoW)
            {
                internal::StringBuffer<char> _szNodeNameANSI;
                internal::StringBuffer<char> _szServiceNameANSI;
                if (internal::Convert(_szName, -1, &_szNodeNameANSI) != ERROR_SUCCESS || internal::Convert(_szServiceName, -1, &_szServiceNameANSI) != ERROR_SUCCESS)
                {
                    return WSA_NOT_ENOUGH_MEMORY;
                }
                // 因为 ai_canonname == null 所以这里 ADDRINFOA == ADDRINFOW
                auto _pHintsA = _pHintsEx ? reinterpret_cast<const ADDRINFOA*>(&_Hints) : nullptr;
                PADDRINFOA _pResultA = nullptr;
                auto _lStatus = getaddrinfo(
                    _szName ? _szNodeNameANSI.GetC_String() : nullptr,
                    _szServiceName ? _szServiceNameANSI.GetC_String() : nullptr,
                    _pHintsA,
                    &_pResultA);
                if (_lStatus || _pResultA == nullptr)
                    return _lStatus;

                _lStatus = Convert(_pResultA, _ppResultEx);
                ::freeaddrinfo(_pResultA);
                return _lStatus;
            }
#endif // ! (YY_Thunks_Target < __WindowsNT5_1_SP2)

            PADDRINFOW _pResult = nullptr;
            auto _lStatus = GetAddrInfoW(_szName, _szServiceName, _pHintsEx ? &_Hints : nullptr, &_pResult);
            if (_lStatus != ERROR_SUCCESS || _pResult == nullptr)
                return _lStatus;

            _lStatus = Convert(_pResult, _ppResultEx);
            ::FreeAddrInfoW(_pResult);
            return _lStatus;
        }
#endif // ! (YY_Thunks_Target < __WindowsNT6)


#if (YY_Thunks_Target < __WindowsNT6)
        static INT WSAAPI GetAddrInfoExADownlevel(
            _In_opt_    PCSTR           _szName,
            _In_opt_    PCSTR           _szServiceName,
            _In_        DWORD           _uNameSpace,
            _In_opt_    LPGUID          _pNspId,
            _In_opt_    const ADDRINFOEXA* _pHintsEx,
            _Outptr_    PADDRINFOEXA* _ppResultEx,
            _In_opt_    struct timeval* _pTimeout,
            _In_opt_    LPOVERLAPPED    _pOverlapped,
            _In_opt_    LPLOOKUPSERVICE_COMPLETION_ROUTINE  _pfnCompletionRoutine,
            _Out_opt_   LPHANDLE        _pHandle
        )
        {
            if (_pHandle)
                *_pHandle = nullptr;

            *_ppResultEx = nullptr;

            if (_pHintsEx && (_pHintsEx->ai_addrlen || _pHintsEx->ai_canonname || _pHintsEx->ai_addr || _pHintsEx->ai_next))
            {
                return WSANO_RECOVERY;
            }

            ADDRINFOA _Hints;
            if (_pHintsEx)
            {
                _Hints.ai_flags = _pHintsEx->ai_flags;
                _Hints.ai_family = _pHintsEx->ai_family;
                _Hints.ai_socktype = _pHintsEx->ai_socktype;
                _Hints.ai_protocol = _pHintsEx->ai_protocol;
                _Hints.ai_addrlen = 0;
                _Hints.ai_canonname = nullptr;
                _Hints.ai_addr = nullptr;
                _Hints.ai_next = nullptr;
            }

            PADDRINFOA _pResultA = nullptr;
            auto _lStatus = getaddrinfo(
                _szName,
                _szServiceName,
                _pHintsEx ? &_Hints : nullptr,
                &_pResultA);
            if (_lStatus || _pResultA == nullptr)
                return _lStatus;

            _lStatus = Convert(_pResultA, _ppResultEx);
            ::freeaddrinfo(_pResultA);
            return _lStatus;
        }
#endif // ! (YY_Thunks_Target < __WindowsNT6)


#endif // ! defined(YY_Thunks_Implemented) && YY_Thunks_Target < __WindowsNT6_2
    }
}


namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    //Windows 8.1, Windows Vista [desktop apps | UWP apps]
    //Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    ws2_32,
    12,
    INT,
    WINAPI,
    inet_pton,
        _In_                                      INT             Family,
        _In_                                      PCSTR           pszAddrString,
        _When_(Family == AF_INET, _Out_writes_bytes_(sizeof(IN_ADDR)))
        _When_(Family == AF_INET6, _Out_writes_bytes_(sizeof(IN6_ADDR)))
                                                    PVOID           pAddrBuf
        )
    {
        if (auto pinet_pton = try_get_inet_pton())
        {
            return pinet_pton(Family, pszAddrString, pAddrBuf);
        }

        typedef char Char;

        if (nullptr == pszAddrString || nullptr == pAddrBuf)
        {
            WSASetLastError(WSAEFAULT);
            return -1;
        }

        if (AF_INET == Family)
        {
            //IPv4
            UCHAR IPAddress[4] = {};

            unsigned i = 0;
            for (; i != _countof(IPAddress); )
            {
                auto& IPNum = IPAddress[i++];

                auto Start = pszAddrString;
                for (; *Start && *Start != Char('.'); ++Start)
                {
                    auto& Ch = *Start;

                    //必须是0~9数字
                    if (Ch >= Char('0') && Ch <= Char('9'))
                    {
                        auto NewNum = IPNum * 10ul + (Ch & 0x0Ful);
                        if (NewNum > 0xFF)
                        {
                            //不能大于255
                            return 0;
                        }

                        IPNum = (UCHAR)NewNum;
                    }
                    else
                    {
                        return 0;
                    }
                }

                //
                if (Start == pszAddrString)
                    return 0;

                pszAddrString = Start;

                if (*pszAddrString == Char('\0'))
                    break;
                else
                    ++pszAddrString;
            }

            //未正常截断
            if (i != _countof(IPAddress) || *pszAddrString != Char('\0'))
                return 0;


            ((IN_ADDR *)pAddrBuf)->S_un.S_addr = *(ULONG*)IPAddress;

            return 1;
        }
        else if (AF_INET6 == Family)
        {
            //IPv6

            IN6_ADDR IPAddress;

            int i = 0;
            int InsertIndex = -1;

            for (; i != _countof(IPAddress.u.Word) && *pszAddrString; )
            {
                if (pszAddrString[0] == Char(':') && pszAddrString[1] == Char(':'))
                {
                    //缩进只能有一次
                    if (InsertIndex != -1)
                        return 0;


                    InsertIndex = i;

                    pszAddrString += 2;
                    continue;
                }

                auto& IPNum = IPAddress.u.Word[i++];
                IPNum = 0;

                auto Start = pszAddrString;
                for (; *Start && *Start != Char(':'); ++Start)
                {
                    auto& Ch = *Start;


                    unsigned int NewNum;

                    //0~9数字
                    if (Ch >= Char('0') && Ch <= Char('9'))
                    {
                        NewNum = Ch & 0x0Ful;
                    }
                    else if (Ch >= Char('a') && Ch <= Char('f'))
                    {
                        NewNum = Ch - Char('a') + 10;
                    }
                    else if (Ch >= Char('A') && Ch <= Char('F'))
                    {
                        NewNum = Ch - Char('A') + 10;
                    }
                    else
                    {
                        //无法识别
                        return 0;
                    }

                    NewNum = ((unsigned int)IPNum << 4) | NewNum;

                    if (NewNum > 0xFFFF)
                    {
                        //不能大于255
                        return 0;
                    }

                    IPNum = NewNum;
                }


                //截断错误
                if (Start == pszAddrString)
                {
                    return 0;
                }


                IPNum = _byteswap_ushort(IPNum);

                pszAddrString = Start;

                if (*pszAddrString == Char('\0'))
                {
                    break;
                }
                else if (pszAddrString[1] == Char(':'))
                {
                }
                else
                {
                    ++pszAddrString;
                }
            }


            //未正常截断
            if (*pszAddrString != Char('\0'))
            {
                return 0;
            }
            else if (i != _countof(IPAddress.u.Word) && InsertIndex == -1)
            {
                return 0;
            }
            else
            {
                if (InsertIndex == -1)
                {
                    *(IN6_ADDR*)pAddrBuf = IPAddress;
                }
                else
                {
                    //先复制头

                    auto j = 0u;
                    for (; j != InsertIndex; ++j)
                    {
                        ((IN6_ADDR*)pAddrBuf)->u.Word[j] = IPAddress.u.Word[j];
                    }

                    //补充中间 0
                    for (const auto Count = _countof(IPAddress.u.Word) - i + j; j != Count; ++j)
                    {
                        ((IN6_ADDR*)pAddrBuf)->u.Word[j] = 0;
                    }

                    //复制小尾巴
                    for (; j != _countof(IPAddress.u.Word); ++j, ++InsertIndex)
                    {
                        ((IN6_ADDR*)pAddrBuf)->u.Word[j] = IPAddress.u.Word[InsertIndex];
                    }
                }
                return 1;
            }
        }
        else
        {
            WSASetLastError(WSAEAFNOSUPPORT);
            return -1;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows 8.1, Windows Vista [desktop apps | UWP apps]
    //Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    ws2_32,
    12,
    INT,
    WINAPI,
    InetPtonW,
        _In_                                      INT             Family,
        _In_                                      PCWSTR          pszAddrString,
        _When_(Family == AF_INET, _Out_writes_bytes_(sizeof(IN_ADDR)))
        _When_(Family == AF_INET6, _Out_writes_bytes_(sizeof(IN6_ADDR)))
                                                    PVOID           pAddrBuf
        )
    {
        if (auto pInetPtonW = try_get_InetPtonW())
        {
            return pInetPtonW(Family, pszAddrString, pAddrBuf);
        }

        typedef wchar_t Char;

        if (nullptr == pszAddrString || nullptr == pAddrBuf)
        {
            WSASetLastError(WSAEFAULT);
            return -1;
        }

        if (AF_INET == Family)
        {
            //IPv4
            UCHAR IPAddress[4] = {};

            unsigned i = 0;
            for (; i != _countof(IPAddress); )
            {
                auto& IPNum = IPAddress[i++];

                auto Start = pszAddrString;
                for (; *Start && *Start != Char('.'); ++Start)
                {
                    auto& Ch = *Start;

                    //必须是0~9数字
                    if (Ch >= Char('0') && Ch <= Char('9'))
                    {
                        auto NewNum = IPNum * 10ul + (Ch & 0x0Ful);
                        if (NewNum > 0xFF)
                        {
                            //不能大于255
                            return 0;
                        }

                        IPNum = (UCHAR)NewNum;
                    }
                    else
                    {
                        return 0;
                    }
                }

                //
                if (Start == pszAddrString)
                    return 0;

                pszAddrString = Start;

                if (*pszAddrString == Char('\0'))
                    break;
                else
                    ++pszAddrString;
            }

            //未正常截断
            if (i != _countof(IPAddress) || *pszAddrString != Char('\0'))
                return 0;


            ((IN_ADDR *)pAddrBuf)->S_un.S_addr = *(ULONG*)IPAddress;

            return 1;
        }
        else if (AF_INET6 == Family)
        {
            //IPv6

            IN6_ADDR IPAddress;

            int i = 0;
            int InsertIndex = -1;

            for (; i != _countof(IPAddress.u.Word) && *pszAddrString; )
            {
                if (pszAddrString[0] == Char(':') && pszAddrString[1] == Char(':'))
                {
                    //缩进只能有一次
                    if (InsertIndex != -1)
                        return 0;


                    InsertIndex = i;

                    pszAddrString += 2;
                    continue;
                }

                auto& IPNum = IPAddress.u.Word[i++];
                IPNum = 0;

                auto Start = pszAddrString;
                for (; *Start && *Start != Char(':'); ++Start)
                {
                    auto& Ch = *Start;


                    unsigned int NewNum;

                    //0~9数字
                    if (Ch >= Char('0') && Ch <= Char('9'))
                    {
                        NewNum = Ch & 0x0Ful;
                    }
                    else if (Ch >= Char('a') && Ch <= Char('f'))
                    {
                        NewNum = Ch - Char('a') + 10;
                    }
                    else if (Ch >= Char('A') && Ch <= Char('F'))
                    {
                        NewNum = Ch - Char('A') + 10;
                    }
                    else
                    {
                        //无法识别
                        return 0;
                    }

                    NewNum = ((unsigned int)IPNum << 4) | NewNum;

                    if (NewNum > 0xFFFF)
                    {
                        //不能大于255
                        return 0;
                    }

                    IPNum = NewNum;
                }


                //截断错误
                if (Start == pszAddrString)
                {
                    return 0;
                }


                IPNum = _byteswap_ushort(IPNum);

                pszAddrString = Start;

                if (*pszAddrString == Char('\0'))
                {
                    break;
                }
                else if (pszAddrString[1] == Char(':'))
                {
                }
                else
                {
                    ++pszAddrString;
                }
            }


            //未正常截断
            if (*pszAddrString != Char('\0'))
            {
                return 0;
            }
            else if (i != _countof(IPAddress.u.Word) && InsertIndex == -1)
            {
                return 0;
            }
            else
            {
                if (InsertIndex == -1)
                {
                    *(IN6_ADDR*)pAddrBuf = IPAddress;
                }
                else
                {
                    //先复制头

                    auto j = 0u;
                    for (; j != InsertIndex; ++j)
                    {
                        ((IN6_ADDR*)pAddrBuf)->u.Word[j] = IPAddress.u.Word[j];
                    }

                    //补充中间 0
                    for (const auto Count = _countof(IPAddress.u.Word) - i + j; j != Count; ++j)
                    {
                        ((IN6_ADDR*)pAddrBuf)->u.Word[j] = 0;
                    }

                    //复制小尾巴
                    for (; j != _countof(IPAddress.u.Word); ++j, ++InsertIndex)
                    {
                        ((IN6_ADDR*)pAddrBuf)->u.Word[j] = IPAddress.u.Word[InsertIndex];
                    }
                }
                return 1;
            }
        }
        else
        {
            WSASetLastError(WSAEAFNOSUPPORT);
            return -1;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows 8.1, Windows Vista [desktop apps | UWP apps]
    //Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    ws2_32,
    16,
    PCSTR,
    WINAPI,
    inet_ntop,
        _In_                                INT             Family,
        _In_                                const VOID *    pAddr,
        _Out_writes_(StringBufSize)         PSTR            pStringBuf,
        _In_                                size_t          StringBufSize
        )
    {
        if (auto pinet_ntop = try_get_inet_ntop())
        {
            return pinet_ntop(Family, pAddr, pStringBuf, StringBufSize);
        }

        NTSTATUS WSAError = ERROR_SUCCESS;

        if (pAddr == nullptr || pStringBuf == nullptr)
        {
            WSAError = ERROR_INVALID_PARAMETER;
        }
        else
        {
            char BufferTemp[64];
            auto szString = pStringBuf;
            size_t cchString = 0;

            constexpr const char Hex[] = "0123456789abcdef";


            if (AF_INET == Family)
            {
                //IPv4
                if (StringBufSize < 16)
                {
                    szString = BufferTemp;
                }

                auto& IPv4 = *((unsigned char(*)[4])pAddr);

                for (int i = 0; i != _countof(IPv4); ++i)
                {
                    auto Num = IPv4[i];
                    if (Num < 10)
                    {
                        //1 位
                        szString[cchString++] = Hex[Num];
                    }
                    else if (Num < 100)
                    {
                        //2 位
                        szString[cchString++] = Hex[Num / 10];
                        szString[cchString++] = Hex[Num % 10];
                    }
                    else
                    {
                        //3 位
                        szString[cchString++] = Hex[Num / 100];
                        szString[cchString++] = Hex[(Num / 10) % 10];
                        szString[cchString++] = Hex[Num % 10];
                    }

                    szString[cchString++] = '.';
                }

                --cchString;
            }
            else if (AF_INET6 == Family)
            {
                //IPv6
                if (StringBufSize < 46)
                {
                    szString = BufferTemp;
                }

                auto& IPv6 = ((const IN6_ADDR*)pAddr)->u.Word;

                int ZeroIndex = -1;
                int ZeroCount = 1;

                //统计 连续 0个数最多的情况
                for (int i = 0; i != _countof(IPv6);)
                {
                    auto j = i;

                    for (; j != _countof(IPv6) && IPv6[j] == 0; ++j);

                    auto Count = j - i;

                    if (Count)
                    {
                        if (Count > ZeroCount)
                        {
                            ZeroCount = Count;
                            ZeroIndex = i;
                        }

                        i = j;
                    }
                    else
                    {
                        ++i;
                    }
                }


                for (int i = 0; i != _countof(IPv6);)
                {
                    if (ZeroIndex == i)
                    {
                        if (i == 0)
                        {
                            szString[cchString++] = ':';
                        }

                        szString[cchString++] = ':';
                        i += ZeroCount;
                    }
                    else
                    {
                        auto Num = _byteswap_ushort(IPv6[i++]);

                        if (Num <= 0xF)
                        {
                            //1 位
                            szString[cchString++] = Hex[Num];
                        }
                        else if (Num <= 0xFF)
                        {
                            //2 位
                            szString[cchString++] = Hex[Num >> 4];
                            szString[cchString++] = Hex[Num & 0x0F];
                        }
                        else if (Num <= 0xFFF)
                        {
                            //3 位
                            szString[cchString++] = Hex[(Num >> 8)];

                            szString[cchString++] = Hex[(Num >> 4) & 0x0F];
                            szString[cchString++] = Hex[(Num >> 0) & 0x0F];
                        }
                        else
                        {
                            //4位
                            szString[cchString++] = Hex[(Num >> 12)];
                            szString[cchString++] = Hex[(Num >> 8) & 0x0F];

                            szString[cchString++] = Hex[(Num >> 4) & 0x0F];
                            szString[cchString++] = Hex[(Num >> 0) & 0x0F];
                        }

                        if (i != _countof(IPv6))
                            szString[cchString++] = ':';
                    }
                }
            }
            else
            {
                WSAError = WSAEAFNOSUPPORT;
            }

            if (WSAError == ERROR_SUCCESS)
            {
                if (cchString >= StringBufSize)
                {
                    WSAError = ERROR_INVALID_PARAMETER;
                }
                else
                {
                    if (szString != BufferTemp)
                    {
                        //缓冲区不足

                        memcpy(pStringBuf, szString, sizeof(szString[0]) * cchString);
                    }

                    pStringBuf[cchString] = '\0';

                    return pStringBuf;
                }
            }
        }


        WSASetLastError(WSAError);
        return nullptr;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows 8.1, Windows Vista [desktop apps | UWP apps]
    //Windows Server 2008 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    ws2_32,
    16,
    PCWSTR,
    WINAPI,
    InetNtopW,
        _In_                                INT             Family,
        _In_                                const VOID *    pAddr,
        _Out_writes_(StringBufSize)         PWSTR           pStringBuf,
        _In_                                size_t          StringBufSize
        )
    {
        if (auto pInetNtopW = try_get_InetNtopW())
        {
            return pInetNtopW(Family, pAddr, pStringBuf, StringBufSize);
        }

        NTSTATUS WSAError = ERROR_SUCCESS;

        if (pAddr == nullptr || pStringBuf == nullptr)
        {
            WSAError = ERROR_INVALID_PARAMETER;
        }
        else
        {
            wchar_t BufferTemp[64];
            auto szString = pStringBuf;
            size_t cchString = 0;

            constexpr const char Hex[] = "0123456789abcdef";


            if (AF_INET == Family)
            {
                //IPv4
                if (StringBufSize < 16)
                {
                    szString = BufferTemp;
                }

                auto& IPv4 = *((unsigned char(*)[4])pAddr);

                for (int i = 0; i != _countof(IPv4); ++i)
                {
                    auto Num = IPv4[i];
                    if (Num < 10)
                    {
                        //1 位
                        szString[cchString++] = Hex[Num];
                    }
                    else if (Num < 100)
                    {
                        //2 位
                        szString[cchString++] = Hex[Num / 10];
                        szString[cchString++] = Hex[Num % 10];
                    }
                    else
                    {
                        //3 位
                        szString[cchString++] = Hex[Num / 100];
                        szString[cchString++] = Hex[(Num / 10) % 10];
                        szString[cchString++] = Hex[Num % 10];
                    }

                    szString[cchString++] = '.';
                }

                --cchString;
            }
            else if (AF_INET6 == Family)
            {
                //IPv6
                if (StringBufSize < 46)
                {
                    szString = BufferTemp;
                }

                auto& IPv6 = ((const IN6_ADDR*)pAddr)->u.Word;

                int ZeroIndex = -1;
                int ZeroCount = 1;

                //统计 连续 0个数最多的情况
                for (int i = 0; i != _countof(IPv6);)
                {
                    auto j = i;

                    for (; j != _countof(IPv6) && IPv6[j] == 0; ++j);

                    auto Count = j - i;

                    if (Count)
                    {
                        if (Count > ZeroCount)
                        {
                            ZeroCount = Count;
                            ZeroIndex = i;
                        }

                        i = j;
                    }
                    else
                    {
                        ++i;
                    }
                }


                for (int i = 0; i != _countof(IPv6);)
                {
                    if (ZeroIndex == i)
                    {
                        if (i == 0)
                        {
                            szString[cchString++] = ':';
                        }

                        szString[cchString++] = ':';
                        i += ZeroCount;
                    }
                    else
                    {
                        auto Num = _byteswap_ushort(IPv6[i++]);

                        if (Num <= 0xF)
                        {
                            //1 位
                            szString[cchString++] = Hex[Num];
                        }
                        else if (Num <= 0xFF)
                        {
                            //2 位
                            szString[cchString++] = Hex[Num >> 4];
                            szString[cchString++] = Hex[Num & 0x0F];
                        }
                        else if (Num <= 0xFFF)
                        {
                            //3 位
                            szString[cchString++] = Hex[(Num >> 8)];

                            szString[cchString++] = Hex[(Num >> 4) & 0x0F];
                            szString[cchString++] = Hex[(Num >> 0) & 0x0F];
                        }
                        else
                        {
                            //4位
                            szString[cchString++] = Hex[(Num >> 12)];
                            szString[cchString++] = Hex[(Num >> 8) & 0x0F];

                            szString[cchString++] = Hex[(Num >> 4) & 0x0F];
                            szString[cchString++] = Hex[(Num >> 0) & 0x0F];
                        }

                        if (i != _countof(IPv6))
                            szString[cchString++] = ':';
                    }
                }
            }
            else
            {
                WSAError = WSAEAFNOSUPPORT;
            }

            if (WSAError == ERROR_SUCCESS)
            {
                if (cchString >= StringBufSize)
                {
                    WSAError = ERROR_INVALID_PARAMETER;
                }
                else
                {
                    if (szString != BufferTemp)
                    {
                        //缓冲区不足

                        memcpy(pStringBuf, szString, sizeof(szString[0]) * cchString);
                    }

                    pStringBuf[cchString] = '\0';

                    return pStringBuf;
                }
            }
        }


        WSASetLastError(WSAError);
        return nullptr;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Windows 8.1, Windows Vista [desktop apps | UWP apps]
    //Windows Server 2008 [desktop apps | UWP apps]
    //参考 https://blog.csdn.net/liangzhao_jay/article/details/53261684 实现
    __DEFINE_THUNK(
    ws2_32,
    12,
    int,
    WSAAPI,
    WSAPoll,
        _Inout_ LPWSAPOLLFD fdArray,
        _In_ ULONG fds,
        _In_ INT timeout
        )
    {
        if (auto const pWSAPoll = try_get_WSAPoll())
        {
            return pWSAPoll(fdArray, fds, timeout);
        }

        fd_set        readfds;
        fd_set        writefds;
        fd_set        exceptfds;

        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);

        for (ULONG i = 0; i < fds; i++)
        {
            auto& fd = fdArray[i];

            //Read (in) socket
            if (fd.events & (POLLRDNORM | POLLRDBAND | POLLPRI))
            {
                FD_SET(fd.fd, &readfds);
            }

            //Write (out) socket
            if (fd.events & (POLLWRNORM | POLLWRBAND))
            {
                FD_SET(fd.fd, &writefds);
            }

            //异常
            if (fd.events & (POLLERR | POLLHUP | POLLNVAL))
            {
                FD_SET(fd.fd, &exceptfds);
            }
        }


        /*
        timeout  < 0 ，无限等待
        timeout == 0 ，马上回来
        timeout  >0  ，最长等这个时间
        */
        timeval* __ptimeout = nullptr;

        timeval time_out;

        if (timeout >= 0)
        {
            time_out.tv_sec = timeout / 1000;
            time_out.tv_usec = (timeout % 1000) * 1000;
            __ptimeout = &time_out;
        }

        const auto result = select(1, &readfds, &writefds, &exceptfds, __ptimeout);

        if (result > 0)
        {
            for (ULONG i = 0; i < fds; i++)
            {
                auto& fd = fdArray[i];

                fd.revents = 0;
                if (FD_ISSET(fd.fd, &readfds))
                    fd.revents |= fd.events & (POLLRDNORM | POLLRDBAND | POLLPRI);

                if (FD_ISSET(fd.fd, &writefds))
                    fd.revents |= fd.events & (POLLWRNORM | POLLWRBAND);

                if (FD_ISSET(fd.fd, &exceptfds))
                    fd.revents |= fd.events & (POLLERR | POLLHUP | POLLNVAL);
            }
        }
    
        return result;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8.1、Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    ws2_32,
    4,
    INT,
    WSAAPI,
    GetAddrInfoExCancel,
        _In_ LPHANDLE _phHandle
        )
    {
        if (auto const _pfnGetAddrInfoExCancel = try_get_GetAddrInfoExCancel())
        {
            return _pfnGetAddrInfoExCancel(_phHandle);
        }

        if (!_phHandle)
            return WSA_INVALID_HANDLE;
            
        for (auto _uResult = *(volatile uintptr_t*)_phHandle;;)
        {
            if (_uResult <= MAXUINT16)
            {
                // 错误代码？看起来任务正在进行或者已经取消
                return WSA_INVALID_HANDLE;
            }

            const auto _uLast = InterlockedCompareExchange((volatile uintptr_t*)_phHandle, WSA_E_CANCELLED, _uResult);
            if (_uLast != _uResult)
            {
                _uResult = _uLast;
                continue;
            }
            auto _pParameter = reinterpret_cast<GetAddrInfoExParameter*>(_uResult);
            _pParameter->SignalCompleted(WSA_E_CANCELLED);
            _pParameter->Release();
            return ERROR_SUCCESS;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    // Vista虽然有这个函数，但是不支持异步，所以也要修正。

    __DEFINE_THUNK(
    ws2_32,
    40,
    INT,
    WSAAPI,
    GetAddrInfoExW,
        _In_opt_    PCWSTR          _szName,
        _In_opt_    PCWSTR          _szServiceName,
        _In_        DWORD           _uNameSpace,
        _In_opt_    LPGUID          _pNspId,
        _In_opt_    const ADDRINFOEXW *_pHints,
        _Outptr_    PADDRINFOEXW *  _ppResult,
        _In_opt_    timeval *_pTimeout,
        _In_opt_    LPOVERLAPPED    _pOverlapped,
        _In_opt_    LPLOOKUPSERVICE_COMPLETION_ROUTINE  _pfnCompletionRoutine,
        _Out_opt_   LPHANDLE        _pHandle
        )
    {
        auto _pfnGetAddrInfoExW = try_get_GetAddrInfoExW();
#if (YY_Thunks_Target < __WindowsNT6)
        // 正常情况下，Vista必定存在这个函数
        if (!_pfnGetAddrInfoExW)
            _pfnGetAddrInfoExW = &GetAddrInfoExWDownlevel;
#endif // ! (YY_Thunks_Target < __WindowsNT6)
        const bool _bAsync = _pTimeout || _pOverlapped || _pfnCompletionRoutine || _pHandle;
        if (_pfnGetAddrInfoExW && (_bAsync == false || try_get_GetAddrInfoExCancel()))
        {
            // 如果GetAddrInfoExCancel函数存在说明是Windows 8以上操作系统，这时已经原生支持异步。
            // 直接调用即可。
            return _pfnGetAddrInfoExW(_szName, _szServiceName, _uNameSpace, _pNspId, _pHints, _ppResult, _pTimeout, _pOverlapped, _pfnCompletionRoutine, _pHandle);
        }

        if (_pHandle)
            *_pHandle = nullptr;
            
        *_ppResult = nullptr;

        if (_pHints && (_pHints->ai_addrlen || _pHints->ai_canonname || _pHints->ai_addr || _pHints->ai_next))
        {
            return WSANO_RECOVERY;
        }

        if (!_pfnGetAddrInfoExW)
            return ERROR_NOT_SUPPORTED;

        const auto _cbName = _szName ? (wcslen(_szName) + 1) * sizeof(WCHAR) : 0;
        const auto _cbServiceName = _szServiceName ? (wcslen(_szServiceName) + 1) * sizeof(WCHAR) : 0;
        auto _pParameter = (GetAddrInfoExWParameter*)internal::Alloc(sizeof(GetAddrInfoExWParameter) + _cbName + _cbServiceName);
        if (!_pParameter)
        {
            return WSA_NOT_ENOUGH_MEMORY;
        }

        memset(_pParameter, 0, sizeof(GetAddrInfoExWParameter));
        _pParameter->nRef = _pHandle ? 2 : 1;
        auto _pOthersBuffer = _pParameter->Others;
        if (_szName)
        {
            _pParameter->szName = reinterpret_cast<PCWSTR>(_pOthersBuffer);
            memcpy(_pOthersBuffer, _szName, _cbName);
            _pOthersBuffer += _cbName;
        }

        if (_szServiceName)
        {
            _pParameter->szName = reinterpret_cast<PCWSTR>(_pOthersBuffer);
            memcpy(_pOthersBuffer, _szServiceName, _cbServiceName);
            _pOthersBuffer += _cbServiceName;
        }

        _pParameter->uNameSpace = _uNameSpace;

        if (_pNspId)
        {
            _pParameter->pNspId = &_pParameter->NspIdBuffer;
            _pParameter->NspIdBuffer = *_pNspId;
        }

        if (_pHints)
        {
            _pParameter->pHints = &_pParameter->HintsBuffer;
            _pParameter->HintsBuffer = *_pHints;
        }
        _pParameter->ppResult = _ppResult;

        if (_pTimeout)
        {
            _pParameter->pTimeout = &_pParameter->TimeoutBuffer;
            _pParameter->TimeoutBuffer = *_pTimeout;
        }
        if (_pOverlapped)
        {
            _pParameter->pOverlapped = _pOverlapped;
            _pOverlapped->Internal = WSA_IO_PENDING;
            _pOverlapped->Pointer = _ppResult;
            if (_pOverlapped->hEvent)
                ResetEvent(_pOverlapped->hEvent);
        }

        _pParameter->pfnCompletionRoutine = _pfnCompletionRoutine;
        _pParameter->pHandle = (uintptr_t*)_pHandle;

        const auto _bResult = ::TrySubmitThreadpoolCallback([](
            _Inout_ PTP_CALLBACK_INSTANCE Instance,
            _Inout_ PVOID                 Context)
            {
                auto _pParameter = static_cast<GetAddrInfoExWParameter*>(Context);
                DWORD _lStatus = ERROR_SUCCESS;

                do
                {
                    auto _pfnGetAddrInfoExW = try_get_GetAddrInfoExW();
#if (YY_Thunks_Target < __WindowsNT6)
                    if (!_pfnGetAddrInfoExW)
                    {
                        _pfnGetAddrInfoExW = &GetAddrInfoExWDownlevel;
                    }
#endif
                    _lStatus = _pParameter->UpdateHandleStatus(WSA_IO_PENDING);
                    if (_lStatus)
                    {
                        break;
                    }

                    _lStatus = _pfnGetAddrInfoExW(
                        _pParameter->szName,
                        _pParameter->szServiceName,
                        _pParameter->uNameSpace,
                        _pParameter->pNspId,
                        _pParameter->pHints,
                        _pParameter->ppResult,
                        nullptr, nullptr, nullptr, nullptr);

                } while (false);

                _pParameter->SignalCompleted(_lStatus);
                _pParameter->Release();
            }, _pParameter, nullptr);

        if (!_bResult)
        {
            internal::Free(_pParameter);
            return WSA_NOT_ENOUGH_MEMORY;
        }

        if (_pHandle)
            *_pHandle = _pParameter;

        return WSA_IO_PENDING;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows Vista [桌面应用 | UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    // Vista虽然有这个函数，但是不支持异步，所以也要修正。
    __DEFINE_THUNK(
    ws2_32,
    40,
    INT,
    WSAAPI,
    GetAddrInfoExA,
        _In_opt_    PCSTR           _szName,
        _In_opt_    PCSTR           _szServiceName,
        _In_        DWORD           _uNameSpace,
        _In_opt_    LPGUID          _pNspId,
        _In_opt_    const ADDRINFOEXA * _pHints,
        _Outptr_    PADDRINFOEXA *  _ppResult,
        _In_opt_    struct timeval * _pTimeout,
        _In_opt_    LPOVERLAPPED    _pOverlapped,
        _In_opt_    LPLOOKUPSERVICE_COMPLETION_ROUTINE  _pfnCompletionRoutine,
        _Out_opt_   LPHANDLE        _pHandle
        )
    {
        auto _pfnGetAddrInfoExA = try_get_GetAddrInfoExA();

#if (YY_Thunks_Target < __WindowsNT6)
        if (!_pfnGetAddrInfoExA)
            _pfnGetAddrInfoExA = &GetAddrInfoExADownlevel;
#endif // ! (YY_Thunks_Target < __WindowsNT6)

        const bool _bAsync = _pTimeout || _pOverlapped || _pfnCompletionRoutine || _pHandle;
        if (_pfnGetAddrInfoExA && (_bAsync == false || try_get_GetAddrInfoExCancel()))
        {
            // 如果GetAddrInfoExCancel函数存在说明是Windows 8以上操作系统，这时已经原生支持异步。
            // 直接调用即可。
            return _pfnGetAddrInfoExA(_szName, _szServiceName, _uNameSpace, _pNspId, _pHints, _ppResult, _pTimeout, _pOverlapped, _pfnCompletionRoutine, _pHandle);
        }

        if (_pHandle)
            *_pHandle = nullptr;
            
        *_ppResult = nullptr;

        if (_pHints && (_pHints->ai_addrlen || _pHints->ai_canonname || _pHints->ai_addr || _pHints->ai_next))
        {
            return WSANO_RECOVERY;
        }

        if (!_pfnGetAddrInfoExA)
        {
            return ERROR_NOT_SUPPORTED;
        }

        const auto _cbName = _szName ? (strlen(_szName) + 1) * sizeof(_szName[0]) : 0;
        const auto _cbServiceName = _szServiceName ? (strlen(_szServiceName) + 1) * sizeof(_szServiceName[0]) : 0;
        auto _pParameter = (GetAddrInfoExAParameter*)internal::Alloc(sizeof(GetAddrInfoExAParameter) + _cbName + _cbServiceName);
        if (!_pParameter)
        {
            return WSA_NOT_ENOUGH_MEMORY;
        }

        memset(_pParameter, 0, sizeof(*_pParameter));
        _pParameter->nRef = _pHandle ? 2 : 1;

        auto _pOthersBuffer = _pParameter->Others;
        if (_szName)
        {
            _pParameter->szName = reinterpret_cast<PCSTR>(_pOthersBuffer);
            memcpy(_pOthersBuffer, _szName, _cbName);
            _pOthersBuffer += _cbName;
        }

        if (_szServiceName)
        {
            _pParameter->szName = reinterpret_cast<PCSTR>(_pOthersBuffer);
            memcpy(_pOthersBuffer, _szServiceName, _cbServiceName);
            _pOthersBuffer += _cbServiceName;
        }

        _pParameter->uNameSpace = _uNameSpace;

        if (_pNspId)
        {
            _pParameter->pNspId = &_pParameter->NspIdBuffer;
            _pParameter->NspIdBuffer = *_pNspId;
        }

        if (_pHints)
        {
            _pParameter->pHints = &_pParameter->HintsBuffer;
            _pParameter->HintsBuffer = *_pHints;
        }
        _pParameter->ppResult = _ppResult;

        if (_pTimeout)
        {
            _pParameter->pTimeout = &_pParameter->TimeoutBuffer;
            _pParameter->TimeoutBuffer = *_pTimeout;
        }
        if (_pOverlapped)
        {
            _pParameter->pOverlapped = _pOverlapped;
            _pOverlapped->Internal = WSA_IO_PENDING;
            _pOverlapped->Pointer = _ppResult;
            if (_pOverlapped->hEvent)
                ResetEvent(_pOverlapped->hEvent);
        }

        _pParameter->pfnCompletionRoutine = _pfnCompletionRoutine;
        _pParameter->pHandle = (uintptr_t*)_pHandle;

        const auto _bResult = ::TrySubmitThreadpoolCallback([](
            _Inout_ PTP_CALLBACK_INSTANCE Instance,
            _Inout_ PVOID                 Context)
            {
                auto _pParameter = static_cast<GetAddrInfoExAParameter*>(Context);
                DWORD _lStatus = ERROR_SUCCESS;

                do
                {
                    auto _pfnGetAddrInfoExA = try_get_GetAddrInfoExA();
#if (YY_Thunks_Target < __WindowsNT6)
                    if (!_pfnGetAddrInfoExA)
                    {
                        _pfnGetAddrInfoExA = &GetAddrInfoExADownlevel;
                    }
#endif
                    _lStatus = _pParameter->UpdateHandleStatus(WSA_IO_PENDING);
                    if (_lStatus)
                    {
                        break;
                    }

                    _lStatus = _pfnGetAddrInfoExA(
                        _pParameter->szName,
                        _pParameter->szServiceName,
                        _pParameter->uNameSpace,
                        _pParameter->pNspId,
                        _pParameter->pHints,
                        _pParameter->ppResult,
                        nullptr, nullptr, nullptr, nullptr);

                } while (false);

                _pParameter->SignalCompleted(_lStatus);
                _pParameter->Release();
            }, _pParameter, nullptr);

        if (!_bResult)
        {
            internal::Free(_pParameter);
            return WSA_NOT_ENOUGH_MEMORY;
        }

        if (_pHandle)
            *_pHandle = _pParameter;

        return WSA_IO_PENDING;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    // 最低受支持的客户端	Windows 8.1、Windows 8 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2012[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    ws2_32,
    4,
    INT,
    WSAAPI,
    GetAddrInfoExOverlappedResult,
        _In_ LPOVERLAPPED _pOverlapped
        )
    {
        if (auto const _pfnGetAddrInfoExOverlappedResult = try_get_GetAddrInfoExOverlappedResult())
        {
            return _pfnGetAddrInfoExOverlappedResult(_pOverlapped);
        }

        return _pOverlapped ? static_cast<INT>(_pOverlapped->Internal) : WSAEINVAL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows 8.1，Windows Vista [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    ws2_32,
    4,
    void,
    WSAAPI,
    FreeAddrInfoExW,
        _In_opt_ PADDRINFOEXW _pAddrInfoEx
        )
    {
        if (auto const _pfnFreeAddrInfoExW = try_get_FreeAddrInfoExW())
        {
            return _pfnFreeAddrInfoExW(_pAddrInfoEx);
        }
            
        // 跟微软实现存在略微差异，微软的实现每一个成员都需要申请内存。
        // 而YY-Thunks为了优化性能，一块 _pAddrInfoEx的内存是一次内存申请。
        while (_pAddrInfoEx)
        {
            auto _pNext = _pAddrInfoEx->ai_next;
            internal::Free(_pAddrInfoEx);
            _pAddrInfoEx = _pNext;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // 最低受支持的客户端	Windows 8.1，Windows Vista [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    ws2_32,
    4,
    void,
    WSAAPI,
    FreeAddrInfoEx,
        _In_opt_ PADDRINFOEXA _pAddrInfoEx
        )
    {
        if (auto const _pfnFreeAddrInfoEx = try_get_FreeAddrInfoEx())
        {
            return _pfnFreeAddrInfoEx(_pAddrInfoEx);
        }

        // 跟微软实现存在略微差异，微软的实现每一个成员都需要申请内存。
        // 而YY-Thunks为了优化性能，一块 _pAddrInfoEx的内存是一次内存申请。
        while (_pAddrInfoEx)
        {
            auto _pNext = _pAddrInfoEx->ai_next;
            internal::Free(_pAddrInfoEx);
            _pAddrInfoEx = _pNext;
        }
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1_SP2)

    // 最低受支持的客户端	Windows XP SP2 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    ws2_32,
    16,
    INT,
    WSAAPI,
    GetAddrInfoW,
        _In_opt_ PCWSTR _szNodeName,
        _In_opt_ PCWSTR _szServiceName,
        _In_opt_ const ADDRINFOW * _pHints,
        _Outptr_ PADDRINFOW * _ppResult
        )
    {
        if (auto const _pfnGetAddrInfoW = try_get_GetAddrInfoW())
        {
            return _pfnGetAddrInfoW(_szNodeName, _szServiceName, _pHints, _ppResult);
        }
            
        *_ppResult = nullptr;
        if (_pHints && (_pHints->ai_addrlen || _pHints->ai_canonname || _pHints->ai_addr || _pHints->ai_next))
        {
            return WSANO_RECOVERY;
        }

        internal::StringBuffer<char> _szNodeNameANSI;
        internal::StringBuffer<char> _szServiceNameANSI;
        if (internal::Convert(_szNodeName, -1, &_szNodeNameANSI) != ERROR_SUCCESS || internal::Convert(_szServiceName, -1, &_szServiceNameANSI) != ERROR_SUCCESS)
        {
            return WSA_NOT_ENOUGH_MEMORY;
        }

        PADDRINFOA _pResultA = nullptr;
        auto _lStatus = getaddrinfo(
            _szNodeName ? _szNodeNameANSI.GetC_String() : nullptr,
            _szServiceName ? _szServiceNameANSI.GetC_String() : nullptr,
            reinterpret_cast<const ADDRINFOA*>(_pHints), // 因为 ai_canonname == null 所以这里 ADDRINFOA == ADDRINFOW
            &_pResultA);
        if (_lStatus || _pResultA == nullptr)
            return _lStatus;

        _lStatus = Convert(_pResultA, _ppResult);
        freeaddrinfo(_pResultA);
        return _lStatus;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT5_1_SP2)

    // 最低受支持的客户端	Windows XP SP2 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2003[桌面应用 | UWP 应用]
    __DEFINE_THUNK(
    ws2_32,
    4,
    void,
    WSAAPI,
    FreeAddrInfoW,
        _In_opt_ PADDRINFOW _pAddrInfo
        )
    {
        if (auto const _pfnFreeAddrInfoW = try_get_FreeAddrInfoW())
        {
            return _pfnFreeAddrInfoW(_pAddrInfo);
        }
            
        // 跟微软实现存在略微差异，微软的实现每一个成员都需要申请内存。
        // 而YY-Thunks为了优化性能，一块 _pAddrInfoEx的内存是一次内存申请。
        while (_pAddrInfo)
        {
            auto _pNext = _pAddrInfo->ai_next;
            internal::Free(_pAddrInfo);
            _pAddrInfo = _pNext;
        }
    }
#endif
} //namespace YY
