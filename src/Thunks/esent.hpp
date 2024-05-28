
#include <Esent.h>


namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
		//JetAttachDatabase2W
		//Minimum supported client	Windows Vista
		//Minimum supported server	Windows Server 2008
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
			if (auto const pJetAttachDatabase2W = try_get_JetAttachDatabase2W())
			{
				return pJetAttachDatabase2W(sesid, szFilename, cpgDatabaseSizeMax, grbit);
			}

			return  JET_errDatabaseNotFound;
		}
	
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //JetBeginSessionW
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
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
            if (auto const pJetBeginSessionW = try_get_JetBeginSessionW())
            {
                return pJetBeginSessionW(instance, psesid, szUserName, szPassword);
            }

            return  JET_errInvalidParameter;
        }

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //JetCreateInstanceW
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
            if (auto const pJetCreateInstanceW = try_get_JetCreateInstanceW())
            {
                return pJetCreateInstanceW(pinstance, szInstanceName);
            }

            return  JET_errInvalidParameter;
        }

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //JetGetTableColumnInfoW
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
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
            if (auto const pJetGetTableColumnInfoW = try_get_JetGetTableColumnInfoW())
            {
                return pJetGetTableColumnInfoW(sesid, tableid, szColumnName, pvResult, cbMax, InfoLevel);
            }

            return  JET_errInvalidParameter;
        }

#endif
		
#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //JetOpenDatabaseW
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
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
            if (auto const pJetOpenDatabaseW = try_get_JetOpenDatabaseW())
            {
                return pJetOpenDatabaseW(sesid, szFilename, szConnect, pdbid, grbit);
            }

            return  JET_errDatabaseInvalidPath;
        }

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //JetOpenTableW
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
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
            if (auto const pJetOpenTableW = try_get_JetOpenTableW())
            {
                return pJetOpenTableW(sesid, dbid, szTableName, pvParameters, cbParameters, grbit, ptableid);
            }

            return  JET_errInvalidName;
        }

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //JetSetSystemParameterW
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
            if (auto const pJetSetSystemParameterW = try_get_JetSetSystemParameterW())
            {
                return pJetSetSystemParameterW(pinstance, sesid, paramid, lParam, szParam);
            }

            return  JET_errInstanceUnavailable;
        }

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //JetGetSystemParameterW
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
            if (auto const pJetGetSystemParameterW = try_get_JetGetSystemParameterW())
            {
                return pJetGetSystemParameterW(instance, sesid, paramid, plParam, szParam, cbMax);
            }

            return  JET_errInstanceUnavailable;
        }

#endif

	}//namespace Thunks

} //namespace YY
