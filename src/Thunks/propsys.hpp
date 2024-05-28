
#include <propvarutil.h>
#include <propsys.h>


namespace YY
{
    namespace Thunks
    {
#ifdef YY_Thunks_Implemented
        namespace internal
        {
#if (YY_Thunks_Support_Version < NTDDI_WIN6)
            static __forceinline BOOL __fastcall myisemptyornull(const PROPVARIANT* propvar)
            {
                if (propvar->vt == VT_EMPTY || propvar->vt == VT_NULL)
                    return TRUE;
                if ((propvar->vt & VT_ARRAY) == VT_ARRAY)
                {
                    int i;
                    for (i = 0; i < propvar->parray->cDims; i++)
                    {
                        if (propvar->parray->rgsabound[i].cElements != 0)
                            break;
                    }
                    return i == propvar->parray->cDims;
                }
                if (propvar->vt == VT_CLSID)
                    return !propvar->puuid;

                if (propvar->vt & VT_VECTOR)
                    return !propvar->caub.cElems;

                /* FIXME: byrefs, errors? */
                return FALSE;
            }

            static __forceinline INT __fastcall MyVariantCompareEx(const PROPVARIANT* propvar1, const PROPVARIANT* propvar2, PROPVAR_COMPARE_FLAGS flags)
            {
                const PROPVARIANT* propvar2_converted;
                unsigned int count;
                HRESULT hr;
                INT res = -1;


                if (myisemptyornull(propvar1))
                {
                    if (myisemptyornull(propvar2))
                        return 0;
                    return (flags & PVCF_TREATEMPTYASGREATERTHAN) ? 1 : -1;
                }

                if (myisemptyornull(propvar2))
                    return (flags & PVCF_TREATEMPTYASGREATERTHAN) ? -1 : 1;

                if (propvar1->vt != propvar2->vt)
                {
                    return -1;
                }

                propvar2_converted = propvar2;

#define CMP_NUM_VALUE(var) do { \
    if (propvar1->var > propvar2_converted->var) \
        res = 1; \
    else if (propvar1->var < propvar2_converted->var) \
        res = -1; \
    else \
        res = 0; \
    } while (0)

                switch (propvar1->vt)
                {
                case VT_I1:
                    CMP_NUM_VALUE(cVal);
                    break;
                case VT_UI1:
                    CMP_NUM_VALUE(bVal);
                    break;
                case VT_I2:
                    CMP_NUM_VALUE(iVal);
                    break;
                case VT_UI2:
                    CMP_NUM_VALUE(uiVal);
                    break;
                case VT_I4:
                    CMP_NUM_VALUE(lVal);
                    break;
                case VT_UI4:
                    CMP_NUM_VALUE(ulVal);
                    break;
                case VT_I8:
                    CMP_NUM_VALUE(hVal.QuadPart);
                    break;
                case VT_UI8:
                    CMP_NUM_VALUE(uhVal.QuadPart);
                    break;
                case VT_R4:
                    CMP_NUM_VALUE(fltVal);
                    break;
                case VT_R8:
                    CMP_NUM_VALUE(dblVal);
                    break;
                case VT_BSTR:
                case VT_LPWSTR:
                    /* FIXME: Use other string flags. */
                    if (flags & (PVCF_USESTRCMPI | PVCF_USESTRCMPIC))
                        res = lstrcmpiW(propvar1->bstrVal, propvar2_converted->bstrVal);
                    else
                        res = lstrcmpW(propvar1->bstrVal, propvar2_converted->bstrVal);
                    break;
                case VT_LPSTR:
                    /* FIXME: Use other string flags. */
                    if (flags & (PVCF_USESTRCMPI | PVCF_USESTRCMPIC))
                        res = lstrcmpiA(propvar1->pszVal, propvar2_converted->pszVal);
                    else
                        res = lstrcmpA(propvar1->pszVal, propvar2_converted->pszVal);
                    break;
                case VT_CLSID:
                    res = memcmp(propvar1->puuid, propvar2->puuid, sizeof(*propvar1->puuid));
                    if (res) res = res > 0 ? 1 : -1;
                    break;
                case VT_VECTOR | VT_UI1:
                    count = min(propvar1->caub.cElems, propvar2->caub.cElems);
                    res = count ? memcmp(propvar1->caub.pElems, propvar2->caub.pElems, sizeof(*propvar1->caub.pElems) * count) : 0;
                    if (res) res = res > 0 ? 1 : -1;
                    if (!res && propvar1->caub.cElems != propvar2->caub.cElems)
                        res = propvar1->caub.cElems > propvar2->caub.cElems ? 1 : -1;
                    break;
                default:
                    res = -1;
                    break;
                }

                return res;
            }

#endif       
        }
#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //InitPropVariantFromCLSID
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
        __DEFINE_THUNK(
            propsys,
            8,
            HRESULT,
            WINAPI,
            InitPropVariantFromCLSID,
            _In_ REFCLSID clsid,
            _Out_ PROPVARIANT* ppropvar
        )
        {
            if (auto const pInitPropVariantFromCLSID = try_get_InitPropVariantFromCLSID())
            {
                return pInitPropVariantFromCLSID(clsid, ppropvar);
            }

            return  E_INVALIDARG;
        }

#endif


#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //PSGetPropertyKeyFromName
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
        __DEFINE_THUNK(
            propsys,
            8,
            HRESULT,
            WINAPI,
            PSGetPropertyKeyFromName,
            _In_ PCWSTR      pszName,
            _Out_ PROPERTYKEY* ppropkey
        )
        {
            if (auto const pPSGetPropertyKeyFromName = try_get_PSGetPropertyKeyFromName())
            {
                return pPSGetPropertyKeyFromName(pszName, ppropkey);
            }

            return  E_INVALIDARG;
        }

#endif



#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //VariantCompare
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
        __DEFINE_THUNK(
            propsys,
            8,
            int,
            WINAPI,
            VariantCompare,
            _In_ REFVARIANT var1,
            _In_ REFVARIANT var2
        )
        {
            if (auto const pVariantCompare = try_get_VariantCompare())
            {
                return pVariantCompare(var1, var2);
            }

            return  -1;
        }

#endif

#if (YY_Thunks_Support_Version < NTDDI_WIN6)
        //VariantCompare
        //Minimum supported client	Windows Vista
        //Minimum supported server	Windows Server 2008
        __DEFINE_THUNK(
            propsys,
            8,
            HRESULT,
            WINAPI,
            PSCreateMemoryPropertyStore,
            _In_ REFIID riid,
            _Outptr_ void** ppv
        )
        {
            if (auto const pPSCreateMemoryPropertyStore = try_get_PSCreateMemoryPropertyStore())
            {
                return pPSCreateMemoryPropertyStore(riid, ppv);
            }

            return E_INVALIDARG;
        }

#endif


    }//namespace Thunks

} //namespace YY
