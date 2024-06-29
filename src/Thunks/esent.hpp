#if (YY_Thunks_Target < __WindowsNT6)
#include <Esent.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6) && !defined(__Comment_Lib_esent)
#define __Comment_Lib_esent
#pragma comment(lib, "esent.lib")
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista
    // Minimum supported server	Windows Server 2008
    __DEFINE_THUNK(
    esent,
    16,
    JET_ERR,
    JET_API,
    JetAttachDatabase2W,
        _In_ JET_SESID              sesid,
        _In_ JET_PCWSTR             szFilename,
        _In_ const unsigned long    cpgDatabaseSizeMax,
        _In_ JET_GRBIT              grbit
        )
    {
        if (auto const _pfnJetAttachDatabase2W = try_get_JetAttachDatabase2W())
        {
            return _pfnJetAttachDatabase2W(sesid, szFilename, cpgDatabaseSizeMax, grbit);
        }

        // 路径的话理论上MAX_PATH就足够了
        char szFilenameBufferA[512] = {};
        if (szFilename && *szFilename)
        {
            auto _iResult = WideCharToMultiByte(CP_ACP, 0, szFilename, -1, szFilenameBufferA, _countof(szFilenameBufferA), nullptr, nullptr);
            if (_iResult <= 0)
            {
                return JET_errFileNotFound;
            }
        }

        return JetAttachDatabase2A(sesid, szFilename ? szFilenameBufferA : nullptr, cpgDatabaseSizeMax, grbit);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista
    // Minimum supported server	Windows Server 2008
    __DEFINE_THUNK(
    esent,
    16,
    JET_ERR,
    JET_API,
    JetBeginSessionW,
        _In_ JET_INSTANCE   instance,
        _Out_ JET_SESID* psesid,
        _In_opt_ JET_PCWSTR szUserName,
        _In_opt_ JET_PCWSTR szPassword
        )
    {
        if (auto const _pfnJetBeginSessionW = try_get_JetBeginSessionW())
        {
            return _pfnJetBeginSessionW(instance, psesid, szUserName, szPassword);
        }

        internal::StringBuffer<char> _szUserNameBuffer;
        if (szUserName || *szUserName)
        {
            auto _lStatus = internal::Convert(szUserName, -1, &_szUserNameBuffer);
            if (_lStatus)
            {
                return JET_errOutOfMemory;
            }
        }

        internal::StringBuffer<char> _szPasswordBuffer;
        if (szPassword && *szPassword)
        {
            auto _lStatus = internal::Convert(szPassword, -1, &_szPasswordBuffer);
            if (_lStatus)
            {
                return JET_errOutOfMemory;
            }
        }
        return JetBeginSessionA(instance, psesid, szUserName ? _szUserNameBuffer.GetC_String() : nullptr, szPassword ? _szPasswordBuffer.GetC_String() : nullptr);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista
    //Minimum supported server	Windows Server 2008
    __DEFINE_THUNK(
    esent,
    8,
    JET_ERR,
    JET_API,
    JetCreateInstanceW,
        _Out_ JET_INSTANCE* pinstance,
        _In_opt_ JET_PCWSTR     szInstanceName
        )
    {
        if (auto const _pfnJetCreateInstanceW = try_get_JetCreateInstanceW())
        {
            return _pfnJetCreateInstanceW(pinstance, szInstanceName);
        }
        internal::StringBuffer<char> _szInstanceNameBuffer;
        if (szInstanceName && *szInstanceName)
        {
            auto _lStatus = internal::Convert(szInstanceName, -1, &_szInstanceNameBuffer);
            if (_lStatus)
            {
                return JET_errOutOfMemory;
            }
        }
        return JetCreateInstanceA(pinstance, szInstanceName ? _szInstanceNameBuffer.GetC_String() : nullptr);;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista
    // Minimum supported server	Windows Server 2008
    __DEFINE_THUNK(
    esent,
    24,
    JET_ERR,
    JET_API,
    JetGetTableColumnInfoW,
        _In_ JET_SESID                  sesid,
        _In_ JET_TABLEID                tableid,
        _In_opt_ JET_PCWSTR             szColumnName,
        _Out_writes_bytes_(cbMax) void* pvResult,
        _In_ unsigned long              cbMax,
        _In_ unsigned long              InfoLevel
        )
    {
        if (auto const _pfnJetGetTableColumnInfoW = try_get_JetGetTableColumnInfoW())
        {
            return _pfnJetGetTableColumnInfoW(sesid, tableid, szColumnName, pvResult, cbMax, InfoLevel);
        }
        internal::StringBuffer<char> _szColumnNameBuffer;
        if (szColumnName && *szColumnName)
        {
            auto _lStatus = internal::Convert(szColumnName, -1, &_szColumnNameBuffer);
            if (_lStatus)
            {
                return JET_errOutOfMemory;
            }
        }
        return JetGetTableColumnInfoA(sesid, tableid, szColumnName ? _szColumnNameBuffer.GetC_String() : nullptr, pvResult, cbMax, InfoLevel);
    }
#endif
        

#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista
    // Minimum supported server	Windows Server 2008
    __DEFINE_THUNK(
    esent,
    20,
    JET_ERR,
    JET_API,
    JetOpenDatabaseW,
        _In_ JET_SESID      sesid,
        _In_ JET_PCWSTR     szFilename,
        _In_opt_ JET_PCWSTR szConnect,
        _Out_ JET_DBID* pdbid,
        _In_ JET_GRBIT      grbit
        )
    {
        if (auto const _pfnJetOpenDatabaseW = try_get_JetOpenDatabaseW())
        {
            return _pfnJetOpenDatabaseW(sesid, szFilename, szConnect, pdbid, grbit);
        }
        
        char szFileNameBuffer[512] = {};
        if (szFilename && *szFilename)
        {
            if (WideCharToMultiByte(CP_ACP, 0, szFilename, 01, szFileNameBuffer, _countof(szFileNameBuffer), nullptr, nullptr) <= 0)
            {
                return JET_errOutOfMemory;
            }
        }

        internal::StringBuffer<char> _szConnectBuffer;
        if (szConnect && *szConnect)
        {
            auto _lStatus = internal::Convert(szConnect, -1, &_szConnectBuffer);
            if (_lStatus)
            {
                return JET_errFileNotFound;
            }
        }

        return JetOpenDatabaseA(sesid, szFilename ? szFileNameBuffer : nullptr, szConnect ? _szConnectBuffer.GetC_String() : nullptr, pdbid, grbit);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    // Minimum supported client	Windows Vista
    // Minimum supported server	Windows Server 2008
    __DEFINE_THUNK(
    esent,
    28,
    JET_ERR,
    JET_API,
    JetOpenTableW,
        _In_ JET_SESID                                  sesid,
        _In_ JET_DBID                                   dbid,
        _In_ JET_PCWSTR                                 szTableName,
        _In_reads_bytes_opt_(cbParameters) const void* pvParameters,
        _In_ unsigned long                              cbParameters,
        _In_ JET_GRBIT                                  grbit,
        _Out_ JET_TABLEID* ptableid
        )
    {
        if (auto const _pfnJetOpenTableW = try_get_JetOpenTableW())
        {
            return _pfnJetOpenTableW(sesid, dbid, szTableName, pvParameters, cbParameters, grbit, ptableid);
        }

        internal::StringBuffer<char> _szTableNameBuffer;
        if (szTableName && *szTableName)
        {
            auto _lStatus = internal::Convert(szTableName, -1, &_szTableNameBuffer);
            if (_lStatus)
            {
                return JET_errOutOfMemory;
            }
        }

        return JetOpenTableA(sesid, dbid, szTableName ? _szTableNameBuffer.GetC_String() : nullptr, pvParameters, cbParameters, grbit, ptableid);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista
    //Minimum supported server	Windows Server 2008
    __DEFINE_THUNK(
    esent,
    20,
    JET_ERR,
    JET_API,
    JetSetSystemParameterW,
        _Inout_opt_ JET_INSTANCE* pinstance,
        _In_opt_ JET_SESID          sesid,
        _In_ unsigned long          paramid,
        _In_opt_ JET_API_PTR        lParam,
        _In_opt_ JET_PCWSTR         szParam
        )
    {
        if (auto const _pfnJetSetSystemParameterW = try_get_JetSetSystemParameterW())
        {
            return _pfnJetSetSystemParameterW(pinstance, sesid, paramid, lParam, szParam);
        }

        internal::StringBuffer<char> _szParamBuffer;
        if (szParam && *szParam)
        {
            auto _lStatus = internal::Convert(szParam, -1, &_szParamBuffer);
            if (_lStatus)
            {
                return JET_errOutOfMemory;
            }
        }

        return JetSetSystemParameterA(pinstance, sesid, paramid, lParam, szParam ? _szParamBuffer.GetC_String() : nullptr);
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6)

    //Minimum supported client	Windows Vista
    //Minimum supported server	Windows Server 2008
    __DEFINE_THUNK(
    esent,
    24,
    JET_ERR,
    JET_API,
    JetGetSystemParameterW,
        _In_ JET_INSTANCE                   instance,
        _In_opt_ JET_SESID                  sesid,
        _In_ unsigned long                  paramid,
        _Out_opt_ JET_API_PTR* plParam,
        _Out_writes_bytes_opt_(cbMax) JET_PWSTR   szParam,
        _In_ unsigned long                  cbMax
        )
    {
        if (auto const _pfnJetGetSystemParameterW = try_get_JetGetSystemParameterW())
        {
            return _pfnJetGetSystemParameterW(instance, sesid, paramid, plParam, szParam, cbMax);
        }

        if (!szParam)
        {
            return JetGetSystemParameterA(instance, sesid, paramid, plParam, nullptr, cbMax);
        }

        internal::StringBuffer<char> _szParamBuffer;
        auto _pBuffer = _szParamBuffer.GetBuffer(max(cbMax, 512));
        if (!_pBuffer)
        {
            return JET_errOutOfMemory;
        }
        memset(_pBuffer, 0, _szParamBuffer.uBufferLength);

        const auto _cchMax = cbMax / sizeof(szParam[0]);

        const auto _Error = JetGetSystemParameterA(instance, sesid, paramid, plParam, _pBuffer, _szParamBuffer.uBufferLength);
        if (_Error == JET_errSuccess)
        {
            if (_pBuffer[_szParamBuffer.uBufferLength - 2])
            {
                if (_cchMax)
                {
                    const auto _iResult = MultiByteToWideChar(CP_ACP, 0, _pBuffer, _szParamBuffer.uBufferLength, szParam, _cchMax);
                    if (_iResult <= 0)
                    {
                        return JET_errInstanceUnavailable;
                    }

                    szParam[min(size_t(_iResult), _cchMax - 1)] = L'\0';
                    return JET_wrnBufferTruncated;
                }
                return JET_errBufferTooSmall;
            }
            else if (_pBuffer[0])
            {
                if (_cchMax)
                {
                    const auto _iResult = MultiByteToWideChar(CP_ACP, 0, _pBuffer, -1, szParam, _cchMax);
                    if (_iResult <= 0)
                    {
                        return JET_errInstanceUnavailable;
                    }
                    
                    if (size_t(_iResult) < _cchMax)
                    {
                        szParam[_iResult] = L'\0';
                        return JET_errSuccess;
                    }
                    else
                    {
                        szParam[_cchMax - 1] = L'\0';
                        return JET_wrnBufferTruncated;
                    }               
                }
                return JET_errBufferTooSmall;
            }
        }
        else if (_Error == JET_wrnBufferTruncated)
        {
            if (_cchMax)
            {
                const auto _iResult = MultiByteToWideChar(CP_ACP, 0, _pBuffer, _szParamBuffer.uBufferLength, szParam, _cchMax);
                if (_iResult <= 0)
                {
                    return JET_errInstanceUnavailable;
                }

                szParam[min(size_t(_iResult), _cchMax - 1)] = L'\0';
                return JET_wrnBufferTruncated;
            }
            return JET_errBufferTooSmall;
        }
        return _Error;
    }
#endif
} //namespace YY::Thunks
