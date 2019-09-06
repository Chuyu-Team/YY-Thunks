


namespace YY
{
	namespace Thunks
	{

#if (YY_Thunks_Support_Version < NTDDI_WS03SP1)
//Windows XP with SP2, Windows Server 2003 with SP1

EXTERN_C
BOOL
WINAPI
IsWow64Message(
	VOID
    )
#ifdef YY_Thunks_Defined
	;
#else
{
	if (auto const pIsWow64Message = try_get_IsWow64Message())
	{
		return pIsWow64Message();
	}

	return FALSE;
}
#endif

__YY_Thunks_Expand_Function(user32, IsWow64Message, 0);

#endif

	}//namespace Thunks

} //namespace YY