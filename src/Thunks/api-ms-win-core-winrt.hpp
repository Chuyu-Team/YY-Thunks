#if (YY_Thunks_Target < __WindowsNT6_2)
#include <roapi.h>
#include <inspectable.h>
#include <Activation.h>
#include <shellapi.h>
#include <locale.h>
#include <combaseapi.h>
#endif

#if (YY_Thunks_Target < __WindowsNT6_2) && !defined(__Comment_Lib_ole32)
#define __Comment_Lib_ole32
#pragma comment(lib, "Ole32.lib")
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)
#define SafeAllocEx(Heap, Flags, Type, Count) ((Type*)HeapAlloc((Heap), (Flags), sizeof(Type) * (Count)))
#define SafeAlloc(Type, Count) SafeAllocEx(GetProcessHeap(), 0, Type, (Count))
#define SafeFreeEx(Heap, Flags, Pointer)  do { if (Pointer) { HeapFree((Heap), (Flags), (Pointer)); (Pointer) = nullptr; } } while(0)
#define SafeFree(Pointer) SafeFreeEx(GetProcessHeap(), 0, (Pointer))

typedef enum _YY_AsyncStatus {
    YY_AsyncStatus_Started = 0,
    YY_AsyncStatus_Completed = 1,
    YY_AsyncStatus_Canceled = 2,
    YY_AsyncStatus_Error = 3
} YY_AsyncStatus;

typedef enum _YY_UserInteractionMode {
    UserInteractionMode_Mouse,
    UserInteractionMode_Touch
} YY_UserInteractionMode;

typedef enum _YY_DayOfWeek {
    DayOfWeek_Sunday = 0,
    DayOfWeek_Monday = 1,
    DayOfWeek_Tuesday = 2,
    DayOfWeek_Wednesday = 3,
    DayOfWeek_Thursday = 4,
    DayOfWeek_Friday = 5,
    DayOfWeek_Saturday = 6,
} YY_DayOfWeek;

MIDL_INTERFACE("9E365E57-48B2-4160-956F-C7385120BBFC")
IUriRuntimeClass : public IInspectable{
    virtual HRESULT STDMETHODCALLTYPE get_AbsoluteUri(HSTRING * value) = 0;
};

MIDL_INTERFACE("00000036-0000-0000-C000-000000000046")
IAsyncInfo : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE get_Id(ULONG * id) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Status(YY_AsyncStatus* asyncStatus) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ErrorCode(HRESULT* errorCode) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(void) = 0;
};

MIDL_INTERFACE("F51F7C4E-8905-4994-8A9D-439B38694297")
IAsyncOperationBoolean : public IAsyncInfo
{
    virtual HRESULT STDMETHODCALLTYPE GetResults(boolean * result) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Completed(void* handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Completed(void** handler) = 0;
};

MIDL_INTERFACE("277151C3-9E3E-42F6-91A4-5DFDEB232451")
ILauncherStatics : public IInspectable{
public:
    virtual HRESULT STDMETHODCALLTYPE TreatAsUntrusted(boolean * value) = 0;
    virtual HRESULT STDMETHODCALLTYPE LaunchFileAsync(IUnknown* file, IAsyncOperationBoolean** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE LaunchFileWithOptionsAsync(IUnknown* file, IUnknown* options, IAsyncOperationBoolean** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE LaunchUriAsync(IUriRuntimeClass* uri, IAsyncOperationBoolean** operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE LaunchUriWithOptionsAsync(IUriRuntimeClass* uri, IUnknown* options, IAsyncOperationBoolean** operation) = 0;
};

MIDL_INTERFACE("C63657F6-8850-470D-88F8-455E16EA2C26")
IUIViewSettings : public IInspectable
{
public:
virtual HRESULT STDMETHODCALLTYPE get_UserInteractionMode(YY_UserInteractionMode * InteractionMode) = 0;
};

MIDL_INTERFACE("3694DBF9-8F68-44BE-8FF5-195C98EDE8A6")
IUIViewSettingsInterop : public IInspectable{
    virtual HRESULT STDMETHODCALLTYPE GetForWindow(HWND appWindow, REFIID riid, void** ppv) = 0;
};

MIDL_INTERFACE("BBE1FA4C-B0E3-4583-BAEF-1F1B2E483E56")
IVectorView_HSTRING : public IInspectable{
    virtual HRESULT STDMETHODCALLTYPE get_Size(unsigned* size) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAt(unsigned index, HSTRING* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE IndexOf(HSTRING value, unsigned* index, boolean* found) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMany(unsigned startIndex, unsigned capacity, HSTRING* value, unsigned* actual) = 0;
};

MIDL_INTERFACE("01BF4326-ED37-4E96-B0E9-C1340D1EA158")
IGlobalizationPreferencesStatics : public IInspectable
{
public:
virtual HRESULT STDMETHODCALLTYPE get_Calendars(IVectorView_HSTRING * *value) = 0;
virtual HRESULT STDMETHODCALLTYPE get_Clocks(IVectorView_HSTRING** value) = 0;
virtual HRESULT STDMETHODCALLTYPE get_Currencies(IVectorView_HSTRING** value) = 0;
virtual HRESULT STDMETHODCALLTYPE get_Languages(IVectorView_HSTRING** value) = 0;
virtual HRESULT STDMETHODCALLTYPE get_HomeGeographicRegion(HSTRING* value) = 0;
virtual HRESULT STDMETHODCALLTYPE get_WeekStartsOn(YY_DayOfWeek* value) = 0;
};

class StubAsyncOperationBoolean : public IAsyncOperationBoolean
{
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER; *ppv = nullptr;
        if (riid == IID_IUnknown || riid == __uuidof(IAsyncInfo) || riid == __uuidof(IAsyncOperationBoolean)) {
            AddRef(); *ppv = static_cast<IAsyncOperationBoolean*>(this); return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return 1; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }

    STDMETHODIMP get_Id(ULONG*) override { return E_NOTIMPL; }
    STDMETHODIMP get_Status(YY_AsyncStatus* status) override {
        if (status) *status = YY_AsyncStatus_Completed; return S_OK;
    }
    STDMETHODIMP get_ErrorCode(HRESULT* error) override {
        if (error) *error = S_OK; return S_OK;
    }
    STDMETHODIMP Cancel() override { return S_OK; }
    STDMETHODIMP Close() override { return S_OK; }

    STDMETHODIMP GetResults(boolean* result) override {
        if (result) *result = TRUE; return S_OK;
    }
    STDMETHODIMP put_Completed(void*) override { return S_OK; }
    STDMETHODIMP get_Completed(void**) override { return E_NOTIMPL; }
};

__declspec(selectany) static StubAsyncOperationBoolean g_StubAsyncOpBool;

class fakeLauncherStatics : public ILauncherStatics
{
public:
    STDMETHODIMP QueryInterface(_In_ REFIID riid, _Out_ void** ppv) override {
        if (!ppv) return E_POINTER; *ppv = nullptr;
        if (riid == IID_IUnknown || riid == __uuidof(ILauncherStatics) || riid == __uuidof(IInspectable) || riid == __uuidof(IAgileObject)) {
            AddRef(); *ppv = static_cast<ILauncherStatics*>(this); return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return 1; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }
    STDMETHODIMP GetIids(_Out_ ULONG* iidCount, _Out_ IID** iids) override {
        if (!iidCount || !iids) return E_POINTER;
        *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(IID)));
        if (!*iids) return E_OUTOFMEMORY;
        (*iids)[0] = __uuidof(ILauncherStatics);
        *iidCount = 1;
        return S_OK;
    }
    STDMETHODIMP GetRuntimeClassName(_Out_ HSTRING* className) override {
        return WindowsCreateString(L"Windows.System.Launcher", 23, className);
    }
    STDMETHODIMP GetTrustLevel(_Out_ TrustLevel* trustLevel) override { *trustLevel = BaseTrust; return S_OK; }
    STDMETHODIMP TreatAsUntrusted(boolean* value) override { *value = FALSE; return S_OK; }
    STDMETHODIMP LaunchFileAsync(IUnknown*, IAsyncOperationBoolean**) override { return E_NOTIMPL; }
    STDMETHODIMP LaunchFileWithOptionsAsync(IUnknown*, IUnknown*, IAsyncOperationBoolean**) override { return E_NOTIMPL; }
    STDMETHODIMP LaunchUriWithOptionsAsync(IUriRuntimeClass* uri, IUnknown*, IAsyncOperationBoolean** operation) override {
        if (!uri || !operation) return E_POINTER;
        return LaunchUriAsync(uri, operation);
    }
    STDMETHODIMP LaunchUriAsync(_In_ IUriRuntimeClass* uri, _Out_ IAsyncOperationBoolean** operation) override {
        if (!uri || !operation) return E_POINTER;
        *operation = nullptr;

        HSTRING hstrUri = nullptr;
        HRESULT hr = uri->get_AbsoluteUri(&hstrUri);
        if (FAILED(hr)) return hr;

        bool launched = false;
        PCWSTR uriStr = WindowsGetStringRawBuffer(hstrUri, nullptr);
        if (uriStr) {
            INT_PTR res = reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"open", uriStr, nullptr, nullptr, SW_SHOWNORMAL));
            launched = (res > 32);
        }
        WindowsDeleteString(hstrUri);

        if (!launched) return E_FAIL;

        *operation = &g_StubAsyncOpBool;
        return S_OK;
    }
};

__declspec(selectany) static fakeLauncherStatics f_launcherstatics;

class fakeActivationFactory : public IActivationFactory
{
public:
    STDMETHODIMP QueryInterface(_In_ REFIID riid, _Out_ void** ppv) override {
        if (!ppv) return E_POINTER; *ppv = nullptr;
        if ((IID_IUnknown == riid) || (__uuidof(IActivationFactory) == riid) || (__uuidof(IInspectable) == riid) || (__uuidof(IAgileObject) == riid)) {
            AddRef(); *ppv = static_cast<IActivationFactory*>(this); return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return 1; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }
    STDMETHODIMP GetIids(_Out_ ULONG* iidCount, _Out_ IID** iids) override {
        if (!iidCount || !iids) return E_POINTER;
        *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(IID)));
        if (!*iids) return E_OUTOFMEMORY;
        (*iids)[0] = __uuidof(IActivationFactory);
        *iidCount = 1;
        return S_OK;
    }
    STDMETHODIMP GetRuntimeClassName(_Out_ HSTRING* className) override {
        PCWSTR Name = L"IActivationFactory";
        return WindowsCreateString(Name, (ULONG)wcslen(Name), className);
    }
    STDMETHODIMP GetTrustLevel(_Out_ TrustLevel* trustLevel) override { *trustLevel = BaseTrust; return S_OK; }
    STDMETHODIMP ActivateInstance(_Out_ IInspectable** instance) override { if (instance) *instance = nullptr; return E_NOTIMPL; }
};

__declspec(selectany) static fakeActivationFactory f_factory;

class fakeUiViewSettings : public IUIViewSettings
{
    STDMETHODIMP QueryInterface(_In_ REFIID riid, _Out_ void** ppv) override {
        if (!ppv) return E_POINTER; *ppv = nullptr;
        if ((riid == IID_IUnknown) || (riid == IID_IAgileObject) || (riid == IID_IInspectable) || (riid == __uuidof(IUIViewSettings))) {
            AddRef(); *ppv = static_cast<IUIViewSettings*>(this); return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return 1; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }
    STDMETHODIMP GetIids(_Out_ ULONG* iidCount, _Out_ IID** iids) override {
        if (!iidCount || !iids) return E_POINTER;
        *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(IID)));
        if (!*iids) return E_OUTOFMEMORY;
        (*iids)[0] = __uuidof(IUIViewSettings);
        *iidCount = 1;
        return S_OK;
    }
    STDMETHODIMP GetRuntimeClassName(_Out_ HSTRING* className) override {
        return WindowsCreateString(L"Windows.UI.ViewManagement.UIViewSettings", 36, className);
    }
    STDMETHODIMP GetTrustLevel(_Out_ TrustLevel* trustLevel) override { *trustLevel = BaseTrust; return S_OK; };
    STDMETHODIMP get_UserInteractionMode(_Out_	YY_UserInteractionMode* InteractionMode) override {
        if (!InteractionMode) return E_POINTER;
        *InteractionMode = UserInteractionMode_Mouse;
        return S_OK;
    }
};

__declspec(selectany) static fakeUiViewSettings f_viewsettings;

class fakeUiViewSettingsInterop : public IUIViewSettingsInterop {
public:
    STDMETHODIMP QueryInterface(_In_ REFIID riid, _Out_ void** ppv) override {
        if (!ppv) return E_POINTER; *ppv = nullptr;
        if (riid == IID_IUnknown || riid == IID_IAgileObject ||
            riid == IID_IInspectable || riid == __uuidof(IUIViewSettingsInterop)) {
            AddRef(); *ppv = static_cast<IUIViewSettingsInterop*>(this); return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return 1; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }
    STDMETHODIMP GetIids(_Out_ ULONG* iidCount, _Out_ IID** iids) override {
        if (!iidCount || !iids) return E_POINTER;
        *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(IID)));
        if (!*iids) return E_OUTOFMEMORY;
        (*iids)[0] = __uuidof(IUIViewSettingsInterop);
        *iidCount = 1;
        return S_OK;
    }
    STDMETHODIMP GetRuntimeClassName(_Out_ HSTRING* className) override {
        return WindowsCreateString(L"Windows.UI.ViewManagement.UIViewSettings", 36, className);
    }
    STDMETHODIMP GetTrustLevel(_Out_ TrustLevel* trustLevel) override { *trustLevel = BaseTrust; return S_OK; };

    STDMETHODIMP GetForWindow(HWND appWindow, REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        *ppv = NULL;

        if (!(riid == __uuidof(IUIViewSettings))) {
            return E_NOINTERFACE;
        }

        *ppv = &f_viewsettings;
        return S_OK;
    }
};

__declspec(selectany) static fakeUiViewSettingsInterop f_viewsettingsinterop;

class StubVectorView_HSTRING : public IVectorView_HSTRING {
public:
    LONG        RefCount;
    ULONG       NumberOfHstrings;
    HSTRING* HstringArray;

    StubVectorView_HSTRING(PCWSTR* strings, ULONG count)
        : RefCount(1), NumberOfHstrings(0), HstringArray(nullptr)
    {
        if (count == 0 || !strings) return;

        HstringArray = SafeAlloc(HSTRING, count);
        if (!HstringArray) return;

        for (ULONG i = 0; i < count; ++i) {
            HRESULT hr = WindowsCreateString(strings[i], (ULONG)wcslen(strings[i]), &HstringArray[i]);
            if (FAILED(hr)) {
                while (NumberOfHstrings > 0) {
                    WindowsDeleteString(HstringArray[--NumberOfHstrings]);
                }
                SafeFree(HstringArray);
                HstringArray = nullptr;
                return;
            }
            ++NumberOfHstrings;
        }
    }

    ~StubVectorView_HSTRING() {
        while (NumberOfHstrings > 0) {
            WindowsDeleteString(HstringArray[--NumberOfHstrings]);
        }
        SafeFree(HstringArray);
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER; *ppv = nullptr;
        if (riid == IID_IUnknown || riid == IID_IInspectable || riid == __uuidof(IVectorView_HSTRING)) {
            AddRef(); *ppv = static_cast<IVectorView_HSTRING*>(this); return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return _InterlockedIncrement(&RefCount); }
    STDMETHODIMP_(ULONG) Release() override {
        LONG newRef = _InterlockedDecrement(&RefCount);
        if (newRef == 0) {
            this->~StubVectorView_HSTRING();
            HeapFree(GetProcessHeap(), 0, this);
        }
        return newRef;
    }
    STDMETHODIMP GetIids(_Out_ ULONG* iidCount, _Out_ IID** iids) override {
        if (!iidCount || !iids) return E_POINTER;
        *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(IID)));
        if (!*iids) return E_OUTOFMEMORY;
        (*iids)[0] = __uuidof(IVectorView_HSTRING);
        *iidCount = 1;
        return S_OK;
    }
    STDMETHODIMP GetRuntimeClassName(HSTRING* className) override {
        PCWSTR Name = L"Windows.Foundation.Collections.IVector";
        return WindowsCreateString(Name, (ULONG)wcslen(Name), className);
    }
    STDMETHODIMP GetTrustLevel(_Out_ TrustLevel* trustLevel) override { *trustLevel = BaseTrust; return S_OK; };

    STDMETHODIMP get_Size(unsigned* size) override {
        if (!size) return E_POINTER;
        *size = NumberOfHstrings;
        return S_OK;
    }
    STDMETHODIMP GetAt(unsigned index, HSTRING* value) override {
        if (!value) return E_POINTER;
        *value = nullptr;
        if (!HstringArray || index >= NumberOfHstrings) return E_BOUNDS;
        return WindowsDuplicateString(HstringArray[index], value);
    }
    STDMETHODIMP IndexOf(HSTRING String, unsigned* IndexOut, boolean* WasFound) override {
        if (!IndexOut || !WasFound) return E_POINTER;
        *IndexOut = 0; *WasFound = FALSE;

        if (!String || !HstringArray) return S_OK;

        for (ULONG i = 0; i < NumberOfHstrings; ++i) {
            INT cmp;
            if (SUCCEEDED(WindowsCompareStringOrdinal(String, HstringArray[i], &cmp)) && cmp == 0) {
                *IndexOut = i; *WasFound = TRUE; return S_OK;
            }
        }
        return S_OK;
    }
    STDMETHODIMP GetMany(unsigned StartIndex, unsigned NumberOfItems, HSTRING* Items, unsigned* NumberOfItemsOut) override {
        if (!Items || !NumberOfItemsOut) return E_POINTER;
        *NumberOfItemsOut = 0;

        if (!HstringArray || StartIndex >= NumberOfHstrings) return S_OK;

        RtlZeroMemory(Items, NumberOfItems * sizeof(*Items));

        for (ULONG i = StartIndex; i < NumberOfHstrings && *NumberOfItemsOut < NumberOfItems; ++i) {
            HRESULT hr = WindowsDuplicateString(HstringArray[i], &Items[*NumberOfItemsOut]);
            if (FAILED(hr)) {
                while (*NumberOfItemsOut > 0) {
                    WindowsDeleteString(Items[--(*NumberOfItemsOut)]);
                }
                return hr;
            }
            ++(*NumberOfItemsOut);
        }
        return S_OK;
    }
};

inline IVectorView_HSTRING* CreateStubVectorView(PCWSTR* strings, ULONG count) {
    void* mem = SafeAlloc(BYTE, sizeof(StubVectorView_HSTRING));
    if (!mem) return nullptr;

    auto* obj = static_cast<StubVectorView_HSTRING*>(mem);

    obj->RefCount = 1;
    obj->NumberOfHstrings = 0;
    obj->HstringArray = SafeAlloc(HSTRING, count);

    if (!obj->HstringArray) {
        SafeFree(obj);
        return nullptr;
    }

    for (ULONG i = 0; i < count; ++i) {
        HRESULT hr = WindowsCreateString(strings[i], (ULONG)wcslen(strings[i]), &obj->HstringArray[i]);
        if (FAILED(hr)) {
            while (obj->NumberOfHstrings > 0) WindowsDeleteString(obj->HstringArray[--obj->NumberOfHstrings]);
            SafeFree(obj->HstringArray);
            SafeFree(obj);
            return nullptr;
        }
        ++obj->NumberOfHstrings;
    }
    return static_cast<IVectorView_HSTRING*>(obj);
}

class fakeGlobalizationPreferencesStatics : public IGlobalizationPreferencesStatics {
public:
    STDMETHODIMP QueryInterface(_In_ REFIID riid, _Out_ void** ppv) override {
        if (!ppv) return E_POINTER; *ppv = nullptr;
        if (riid == IID_IUnknown || riid == IID_IAgileObject ||
            riid == IID_IInspectable || riid == __uuidof(IGlobalizationPreferencesStatics)) {
            AddRef(); *ppv = static_cast<IGlobalizationPreferencesStatics*>(this); return S_OK;
        }
        return E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return 1; }
    STDMETHODIMP_(ULONG) Release() override { return 1; }

    STDMETHODIMP GetIids(_Out_ ULONG* iidCount, _Out_ IID** iids) override {
        if (!iidCount || !iids) return E_POINTER;
        *iids = static_cast<IID*>(CoTaskMemAlloc(sizeof(IID)));
        if (!*iids) return E_OUTOFMEMORY;
        (*iids)[0] = __uuidof(IGlobalizationPreferencesStatics);
        *iidCount = 1;
        return S_OK;
    }
    STDMETHODIMP GetRuntimeClassName(_Out_ HSTRING* className) override {
        return WindowsCreateString(L"Windows.System.UserProfile.GlobalizationPreferences", 45, className);
    }
    STDMETHODIMP GetTrustLevel(_Out_ TrustLevel* trustLevel) override {
        *trustLevel = BaseTrust; return S_OK;
    }

    // Коллекции не критичны для Qt6 → возвращаем E_NOTIMPL
    STDMETHODIMP get_Calendars(IVectorView_HSTRING** value) override { return E_NOTIMPL; }
    STDMETHODIMP get_Currencies(IVectorView_HSTRING**) override { return E_NOTIMPL; }
    STDMETHODIMP get_Clocks(IVectorView_HSTRING** value) override { return E_NOTIMPL; }

    STDMETHODIMP get_HomeGeographicRegion(HSTRING* value) override {
        if (!value) return E_POINTER;
        ULONG ResultCch;
        WCHAR CountryName[8];

        ResultCch = GetLocaleInfoEx(
            LOCALE_NAME_USER_DEFAULT,
            LOCALE_ICOUNTRY,
            CountryName,
            ARRAYSIZE(CountryName));

        if (ResultCch == 0) {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        return WindowsCreateString(CountryName, (ULONG)wcslen(CountryName), value);
    }

    STDMETHODIMP get_Languages(IVectorView_HSTRING** VectorView) override {
        if (!VectorView) return E_POINTER;
        *VectorView = nullptr;

        WCHAR LocaleName[LOCALE_NAME_MAX_LENGTH];
        int cch = GetUserDefaultLocaleName(LocaleName, LOCALE_NAME_MAX_LENGTH);
        if (cch == 0) return HRESULT_FROM_WIN32(GetLastError());

        PCWSTR langs[] = { LocaleName };
        IVectorView_HSTRING* view = CreateStubVectorView(langs, 1);

        *VectorView = view;
        return view ? S_OK : E_OUTOFMEMORY;
    }

    STDMETHODIMP get_WeekStartsOn(YY_DayOfWeek* value) override {
        ULONG ResultCch;
        ULONG WeekStartsOn;

        if (!value) return E_POINTER;

        ResultCch = GetLocaleInfoEx(
            LOCALE_NAME_USER_DEFAULT,
            LOCALE_RETURN_NUMBER | LOCALE_IFIRSTDAYOFWEEK,
            (PWSTR)&WeekStartsOn,
            2);
        if (ResultCch == 0) {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        if (WeekStartsOn == 6) {
            *value = DayOfWeek_Sunday;
        }
        else {
            *value = (YY_DayOfWeek)(WeekStartsOn + 1);
        }

        return S_OK;
    }
};

__declspec(selectany) static fakeGlobalizationPreferencesStatics f_globalization;
#endif // YY_Thunks_Target < __WindowsNT10_0

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    4,
    HRESULT,
    WINAPI,
    RoInitialize,
        _In_ RO_INIT_TYPE initType
        )
    {
        if (auto const pRoInitialize = try_get_RoInitialize())
        {
            return pRoInitialize(initType);
        }

        if ((unsigned)initType > (unsigned)RO_INIT_MULTITHREADED)
        {
            return E_INVALIDARG;
        }

        return CoInitializeEx(nullptr, initType ? COINIT_MULTITHREADED : COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    0,
    void,
    WINAPI,
    RoUninitialize,
        )
    {
        if (auto const pRoUninitialize = try_get_RoUninitialize())
        {
            return pRoUninitialize();
        }

        CoUninitialize();
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    8,
    HRESULT,
    WINAPI,
    RoActivateInstance,
        _In_ HSTRING activatableClassId,
        _COM_Outptr_ IInspectable** instance
        )
    {
        if (auto const pRoActivateInstance = try_get_RoActivateInstance())
        {
            return pRoActivateInstance(activatableClassId, instance);
        }
        
        *instance = (IInspectable*)&f_factory;

        return S_OK;
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    16,
    HRESULT,
    WINAPI,
    RoRegisterActivationFactories,
        _In_reads_(count) HSTRING* activatableClassIds,
        _In_reads_(count) PFNGETACTIVATIONFACTORY* activationFactoryCallbacks,
        _In_ UINT32 count,
        _Out_ RO_REGISTRATION_COOKIE* cookie
        )
    {
        if (auto const pRoRegisterActivationFactories = try_get_RoRegisterActivationFactories())
        {
            return pRoRegisterActivationFactories(activatableClassIds, activationFactoryCallbacks, count, cookie);
        }

        if (cookie)
            *cookie = nullptr;

        return E_NOTIMPL;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    4,
    void,
    WINAPI,
    RoRevokeActivationFactories,
        _In_ RO_REGISTRATION_COOKIE cookie
        )
    {
        if (auto const pRoRevokeActivationFactories = try_get_RoRevokeActivationFactories())
        {
            return pRoRevokeActivationFactories(cookie);
        }
    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    RoGetActivationFactory,
        _In_ HSTRING activatableClassId,
        _In_ REFIID iid,
        _COM_Outptr_ void** factory
        )
    {
        if (auto const pRoGetActivationFactory = try_get_RoGetActivationFactory())
        {
            return pRoGetActivationFactory(activatableClassId, iid, factory);
        }

        if (factory)
            *factory = nullptr;

        if (IsEqualIID(__uuidof(IActivationFactory), iid))
        {
            *factory = (IActivationFactory*)&f_factory;
        }
        else if (IsEqualIID(__uuidof(ILauncherStatics), iid))
        {
            *factory = (ILauncherStatics*)&f_launcherstatics;
        }
        else if (IsEqualIID(__uuidof(IUIViewSettings), iid))
        {
            *factory = (IUIViewSettings*)&f_viewsettings;
        }
        else if (IsEqualIID(__uuidof(IUIViewSettingsInterop), iid))
        {
            *factory = (IUIViewSettingsInterop*)&f_viewsettingsinterop;
        }
        else if (IsEqualIID(__uuidof(IGlobalizationPreferencesStatics), iid))
        {
            *factory = (IGlobalizationPreferencesStatics*)&f_globalization;
        }
        else
        {
            return E_NOINTERFACE;
        }

        return S_OK;
    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    12,
    HRESULT,
    WINAPI,
    RoRegisterForApartmentShutdown,
        _In_ IApartmentShutdown* callbackObject,
        _Out_ UINT64* apartmentIdentifier,
        _Out_ APARTMENT_SHUTDOWN_REGISTRATION_COOKIE* regCookie
        )
    {
        if (auto const pRoRegisterForApartmentShutdown = try_get_RoRegisterForApartmentShutdown())
        {
            return pRoRegisterForApartmentShutdown(callbackObject, apartmentIdentifier, regCookie);
        }

        if (regCookie)
            *regCookie = nullptr;


        return E_NOTIMPL;

    }
#endif


#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    4,
    HRESULT,
    WINAPI,
    RoUnregisterForApartmentShutdown,
        _In_ APARTMENT_SHUTDOWN_REGISTRATION_COOKIE regCookie
        )
    {
        if (auto const pRoUnregisterForApartmentShutdown = try_get_RoUnregisterForApartmentShutdown())
        {
            return pRoUnregisterForApartmentShutdown(regCookie);
        }

        return E_NOTIMPL;

    }
#endif

#if (YY_Thunks_Target < __WindowsNT6_2)

    //Windows 8 [desktop apps | UWP apps]
    //Windows Server 2012 [desktop apps | UWP apps]
    __DEFINE_THUNK(
    api_ms_win_core_winrt_l1_1_0,
    4,
    HRESULT,
    WINAPI,
    RoGetApartmentIdentifier,
        _Out_ UINT64* apartmentIdentifier
        )
    {
        if (auto const pRoGetApartmentIdentifier = try_get_RoGetApartmentIdentifier())
        {
            return pRoGetApartmentIdentifier(apartmentIdentifier);
        }

        return E_NOTIMPL;

    }
#endif
}
