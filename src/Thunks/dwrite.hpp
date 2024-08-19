#if (YY_Thunks_Target < __WindowsNT10_10240)
#include <dwrite.h>
#include <dwrite_3.h>
#endif

#if defined(YY_Thunks_Implemented)
namespace YY::Thunks::Fallback
{
    namespace
    {
#if (YY_Thunks_Target < __WindowsNT10_10240)
        class DWriteRenderingParams : public IDWriteRenderingParams3, public internal::CppAlloc
        {
        private:
            ULONG uRef = 1;
            DWORD uLevel = 0;
            union
            {
                IDWriteRenderingParams* pDWriteRenderingParams = nullptr;
                IDWriteRenderingParams1* pDWriteRenderingParams1;
                IDWriteRenderingParams2* pDWriteRenderingParams2;
                IDWriteRenderingParams3* pDWriteRenderingParams3;
            };
            FLOAT enhancedContrastGrayscale = 0.0f;
            DWRITE_GRID_FIT_MODE eGridFitMode = DWRITE_GRID_FIT_MODE::DWRITE_GRID_FIT_MODE_DEFAULT;
            DWRITE_RENDERING_MODE1 eRenderingMode1 = DWRITE_RENDERING_MODE1::DWRITE_RENDERING_MODE1_DEFAULT;

        public:
            DWriteRenderingParams(DWORD uLevel, IDWriteRenderingParams* _pDWriteRenderingParams,
                FLOAT _EnhancedContrastGrayscale, 
                DWRITE_GRID_FIT_MODE _eGridFitMode = DWRITE_GRID_FIT_MODE::DWRITE_GRID_FIT_MODE_DEFAULT,
                DWRITE_RENDERING_MODE1 _eRenderingMode1 = DWRITE_RENDERING_MODE1::DWRITE_RENDERING_MODE1_DEFAULT)
                : uLevel(uLevel)
                , pDWriteRenderingParams(_pDWriteRenderingParams)
                , enhancedContrastGrayscale(_EnhancedContrastGrayscale)
                , eGridFitMode(_eGridFitMode)
            {
                pDWriteRenderingParams->AddRef();
            }

            DWriteRenderingParams(IDWriteRenderingParams1* _pDWriteRenderingParams,
                DWRITE_GRID_FIT_MODE _eGridFitMode,
                DWRITE_RENDERING_MODE1 _eRenderingMode1 = DWRITE_RENDERING_MODE1::DWRITE_RENDERING_MODE1_DEFAULT)
                : uLevel(1)
                , pDWriteRenderingParams1(_pDWriteRenderingParams)
                , eGridFitMode(_eGridFitMode)
            {
                pDWriteRenderingParams->AddRef();
            }

            DWriteRenderingParams(IDWriteRenderingParams2* _pDWriteRenderingParams, DWRITE_RENDERING_MODE1 _eRenderingMode1)
                : uLevel(2)
                , pDWriteRenderingParams2(_pDWriteRenderingParams)
                , eRenderingMode1(_eRenderingMode1)
            {
                pDWriteRenderingParams->AddRef();
            }

            DWriteRenderingParams(const DWriteRenderingParams&) = delete;

            ~DWriteRenderingParams()
            {
                pDWriteRenderingParams->Release();
            }

            ////////////////////////////////////////////////////////
            // IUnknown
            HRESULT STDMETHODCALLTYPE QueryInterface(
                /* [in] */ REFIID riid,
                /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
            {
                if (!ppvObject)
                    return E_POINTER;

                *ppvObject = nullptr;
                if (riid == __uuidof(IUnknown)
                    || riid == __uuidof(IDWriteRenderingParams)
                    || riid == __uuidof(IDWriteRenderingParams1)
                    || riid == __uuidof(IDWriteRenderingParams2)
                    || riid == __uuidof(IDWriteRenderingParams3))
                {
                    AddRef();
                    *ppvObject = this;
                    return S_OK;
                }
                else
                {
                    return E_NOINTERFACE;
                }
            }

            ULONG STDMETHODCALLTYPE AddRef(void) override
            {
                return InterlockedIncrement(&uRef);
            }

            ULONG STDMETHODCALLTYPE Release(void) override
            {
                const auto _uNewRef = InterlockedDecrement(&uRef);
                if (_uNewRef == 0)
                {
                    internal::Delete(this);
                }
                return _uNewRef;
            }

            /////////////////////////////////////////////////////////
            // IDWriteRenderingParams
            STDMETHOD_(FLOAT, GetGamma)() override
            {
                return pDWriteRenderingParams->GetGamma();
            }

            STDMETHOD_(FLOAT, GetEnhancedContrast)() override
            {
                return pDWriteRenderingParams->GetEnhancedContrast();
            }

            STDMETHOD_(FLOAT, GetClearTypeLevel)() override
            {
                return pDWriteRenderingParams->GetEnhancedContrast();
            }

            STDMETHOD_(DWRITE_PIXEL_GEOMETRY, GetPixelGeometry)() override
            {
                return pDWriteRenderingParams->GetPixelGeometry();
            }

            STDMETHOD_(DWRITE_RENDERING_MODE, GetRenderingMode)() override
            {
                return pDWriteRenderingParams->GetRenderingMode();
            }

            /////////////////////////////////////////////////////////
            // IDWriteRenderingParams1
            STDMETHOD_(FLOAT, GetGrayscaleEnhancedContrast)() override
            {
                if (uLevel >= 1)
                {
                    return pDWriteRenderingParams1->GetGrayscaleEnhancedContrast();
                }
                return enhancedContrastGrayscale;
            }

            /////////////////////////////////////////////////////////
            // IDWriteRenderingParams2
            STDMETHOD_(DWRITE_GRID_FIT_MODE, GetGridFitMode)() override
            {
                if (uLevel >= 2)
                {
                    return pDWriteRenderingParams2->GetGridFitMode();
                }
                return eGridFitMode;
            }

            /////////////////////////////////////////////////////////
            // IDWriteRenderingParams3
            STDMETHOD_(DWRITE_RENDERING_MODE1, GetRenderingMode1)() override
            {
                if (uLevel >= 3)
                {
                    return pDWriteRenderingParams3->GetRenderingMode1();
                }

                return eRenderingMode1;
            }
        };

        class DWriteFontCollection : public IDWriteFontCollection1, public internal::CppAlloc
        {
        private:
            ULONG uRef = 1;
            DWORD uLevel = 0;
            union
            {
                IDWriteFontCollection* pFontCollection = nullptr;
                IDWriteFontCollection1* pFontCollection1;
            };

        public:
            DWriteFontCollection(_In_opt_ IDWriteFontCollection* _pFontCollection = nullptr, DWORD _uLevel = 0)
                : uLevel(_uLevel)
                , pFontCollection(_pFontCollection)
            {
                if(pFontCollection)
                    pFontCollection->AddRef();
            }

            DWriteFontCollection(const DWriteFontCollection&) = delete;

            ~DWriteFontCollection()
            {
                if(pFontCollection)
                    pFontCollection->Release();
            }

            ////////////////////////////////////////////////////////
            // IUnknown
            HRESULT STDMETHODCALLTYPE QueryInterface(
                /* [in] */ REFIID riid,
                /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
            {
                if (!ppvObject)
                    return E_POINTER;

                *ppvObject = nullptr;
                if (riid == __uuidof(IUnknown)
                    || riid == __uuidof(IDWriteFontCollection)
                    || riid == __uuidof(IDWriteFontCollection1))
                {
                    AddRef();
                    *ppvObject = this;
                    return S_OK;
                }
                else if (pFontCollection)
                {
                    return pFontCollection->QueryInterface(riid, ppvObject);
                }
                else
                {
                    return E_NOINTERFACE;
                }
            }

            ULONG STDMETHODCALLTYPE AddRef(void) override
            {
                return InterlockedIncrement(&uRef);
            }

            ULONG STDMETHODCALLTYPE Release(void) override
            {
                const auto _uNewRef = InterlockedDecrement(&uRef);
                if (_uNewRef == 0)
                {
                    internal::Delete(this);
                }
                return _uNewRef;
            }

            ///////////////////////////////////////////////
            // IDWriteFontCollection
            STDMETHOD_(UINT32, GetFontFamilyCount)() override
            {
                return pFontCollection ? pFontCollection->GetFontFamilyCount() : 0u;
            }

            STDMETHOD(GetFontFamily)(
                UINT32 index,
                _COM_Outptr_ IDWriteFontFamily** fontFamily
                ) override
            {
                if (pFontCollection)
                {
                    return pFontCollection->GetFontFamily(index, fontFamily);
                }

                if (fontFamily)
                    *fontFamily = nullptr;
            
                return E_INVALIDARG;
            }

            STDMETHOD(FindFamilyName)(
                _In_z_ WCHAR const* familyName,
                _Out_ UINT32* index,
                _Out_ BOOL* exists
                ) override
            {
                if (pFontCollection)
                {
                    return pFontCollection->FindFamilyName(familyName, index, exists);
                }

                if (index == nullptr || exists == nullptr)
                    return E_INVALIDARG;

                *index = UINT_MAX;
                *exists = FALSE;
                return S_OK;
            }

            STDMETHOD(GetFontFromFontFace)(
                _In_ IDWriteFontFace* fontFace,
                _COM_Outptr_ IDWriteFont** font
                ) override
            {
                if (pFontCollection)
                {
                    return pFontCollection->GetFontFromFontFace(fontFace, font);
                }

                if (!font)
                    return E_NOINTERFACE;

                *font = nullptr;
                return E_NOT_SET;
            }

            ///////////////////////////////////////////////
            // IDWriteFontCollection1
            STDMETHOD(GetFontSet)(
                _COM_Outptr_ IDWriteFontSet** fontSet
                ) override
            {
                if (pFontCollection)
                {
                    if (uLevel >= 1)
                    {
                        return pFontCollection1->GetFontSet(fontSet);
                    }
                }

                if (!fontSet)
                    return E_NOINTERFACE;

                *fontSet = nullptr;
                return E_NOTIMPL;
            }

            STDMETHOD(GetFontFamily)(
                UINT32 index,
                _COM_Outptr_ IDWriteFontFamily1** fontFamily
                ) override
            {
                if (pFontCollection)
                {
                    if (uLevel >= 1)
                    {
                        return pFontCollection1->GetFontFamily(index, fontFamily);
                    }
                }

                if (!fontFamily)
                    return E_NOINTERFACE;

                *fontFamily = nullptr;
                return E_NOTIMPL;
            }

            using IDWriteFontCollection::GetFontFamily;
        };

        class DWriteFontFallback : public IDWriteFontFallback, public internal::CppAlloc
        {
        private:
            ULONG uRef = 1;
            DWORD uLevel = 0;
            IDWriteFontFallback* pFontFallback = nullptr;

        public:
            DWriteFontFallback() = default;

            DWriteFontFallback(const DWriteFontFallback&) = delete;

            ~DWriteFontFallback()
            {
                if (pFontFallback)
                    pFontFallback->Release();
            }

            ////////////////////////////////////////////////////////
            // IUnknown
            HRESULT STDMETHODCALLTYPE QueryInterface(
                /* [in] */ REFIID riid,
                /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
            {
                if (!ppvObject)
                    return E_POINTER;

                *ppvObject = nullptr;
                if (riid == __uuidof(IUnknown)
                    || riid == __uuidof(IDWriteFontFallback))
                {
                    AddRef();
                    *ppvObject = this;
                    return S_OK;
                }
                else if (pFontFallback)
                {
                    return pFontFallback->QueryInterface(riid, ppvObject);
                }
                else
                {
                    return E_NOINTERFACE;
                }
            }

            ULONG STDMETHODCALLTYPE AddRef(void) override
            {
                return InterlockedIncrement(&uRef);
            }

            ULONG STDMETHODCALLTYPE Release(void) override
            {
                const auto _uNewRef = InterlockedDecrement(&uRef);
                if (_uNewRef == 0)
                {
                    internal::Delete(this);
                }
                return _uNewRef;
            }

            ////////////////////////////////////////////////////////
            // IDWriteFontFallback
            STDMETHOD(MapCharacters)(
                IDWriteTextAnalysisSource* analysisSource,
                UINT32 textPosition,
                UINT32 textLength,
                _In_opt_ IDWriteFontCollection* baseFontCollection,
                _In_opt_z_ wchar_t const* baseFamilyName,
                DWRITE_FONT_WEIGHT baseWeight,
                DWRITE_FONT_STYLE baseStyle,
                DWRITE_FONT_STRETCH baseStretch,
                _Out_range_(0, textLength) UINT32* mappedLength,
                _COM_Outptr_result_maybenull_ IDWriteFont** mappedFont,
                _Out_ FLOAT* scale
                ) override
            {
                if (pFontFallback)
                {
                    return pFontFallback->MapCharacters(
                        analysisSource,
                        textPosition,
                        textLength,
                        baseFontCollection,
                        baseFamilyName,
                        baseWeight,
                        baseStyle,
                        baseStretch,
                        mappedLength,
                        mappedFont,
                        scale);
                }

                if (!mappedFont)
                    return E_NOINTERFACE;

                if (mappedLength == nullptr || scale == nullptr)
                    return E_NOINTERFACE;

                // 假装没有字体需要呈现
                *mappedLength = 0;
                *scale = 1.0;
                *mappedFont = nullptr;
                return S_OK;
            }
        };

        class DWriteFactory : public IDWriteFactory3, public internal::CppAlloc
        {
        private:
            ULONG uRef = 1;
            DWORD uLevel = 0;
            union
            {
                IDWriteFactory* pDWriteFactory = nullptr;
                IDWriteFactory1* pDWriteFactory1;
                IDWriteFactory2* pDWriteFactory2;
                IDWriteFactory3* pDWriteFactory3;
            };

        public:
            DWriteFactory(IDWriteFactory* _pDWriteFactory, DWORD _uLevel = 0)
                : uLevel(_uLevel)
                , pDWriteFactory(_pDWriteFactory)
            {
                pDWriteFactory->AddRef();
            }

            DWriteFactory(const DWriteFactory&) = delete;

            ~DWriteFactory()
            {
                pDWriteFactory->Release();
            }

            ////////////////////////////////////////////////////////
            // IUnknown
            HRESULT STDMETHODCALLTYPE QueryInterface(
                /* [in] */ REFIID riid,
                /* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override
            {
                if (riid == __uuidof(IUnknown)
                    || riid == __uuidof(IDWriteFactory)
                    || riid == __uuidof(IDWriteFactory1)
                    || riid == __uuidof(IDWriteFactory2)
                    || riid == __uuidof(IDWriteFactory3))
                {
                    if (!ppvObject)
                        return E_POINTER;

                    *ppvObject = nullptr;

                    AddRef();
                    *ppvObject = this;
                    return S_OK;
                }
                else
                {
                    return pDWriteFactory->QueryInterface(riid, ppvObject);
                }
            }

            ULONG STDMETHODCALLTYPE AddRef(void) override
            {
                return InterlockedIncrement(&uRef);
            }

            ULONG STDMETHODCALLTYPE Release(void) override
            {
                const auto _uNewRef = InterlockedDecrement(&uRef);
                if (_uNewRef == 0)
                {
                    internal::Delete(this);
                }
                return _uNewRef;
            }

            ////////////////////////////////////////////////////////
            // IDWriteFactory

            STDMETHOD(GetSystemFontCollection)(
                _COM_Outptr_ IDWriteFontCollection** fontCollection,
                BOOL checkForUpdates = FALSE
                ) override
            {
                return pDWriteFactory->GetSystemFontCollection(fontCollection, checkForUpdates);
            }


            STDMETHOD(CreateCustomFontCollection)(
                _In_ IDWriteFontCollectionLoader* collectionLoader,
                _In_reads_bytes_(collectionKeySize) void const* collectionKey,
                UINT32 collectionKeySize,
                _COM_Outptr_ IDWriteFontCollection** fontCollection
                ) override
            {
                return pDWriteFactory->CreateCustomFontCollection(collectionLoader, collectionKey, collectionKeySize, fontCollection);
            }

            STDMETHOD(RegisterFontCollectionLoader)(
                _In_ IDWriteFontCollectionLoader* fontCollectionLoader
                ) override
            {
                return pDWriteFactory->RegisterFontCollectionLoader(fontCollectionLoader);
            }

            STDMETHOD(UnregisterFontCollectionLoader)(
                _In_ IDWriteFontCollectionLoader* fontCollectionLoader
                ) override
            {
                return pDWriteFactory->UnregisterFontCollectionLoader(fontCollectionLoader);
            }

            STDMETHOD(CreateFontFileReference)(
                _In_z_ WCHAR const* filePath,
                _In_opt_ FILETIME const* lastWriteTime,
                _COM_Outptr_ IDWriteFontFile** fontFile
                ) override
            {
                return pDWriteFactory->CreateFontFileReference(filePath, lastWriteTime, fontFile);
            }

            STDMETHOD(CreateCustomFontFileReference)(
                _In_reads_bytes_(fontFileReferenceKeySize) void const* fontFileReferenceKey,
                UINT32 fontFileReferenceKeySize,
                _In_ IDWriteFontFileLoader* fontFileLoader,
                _COM_Outptr_ IDWriteFontFile** fontFile
                ) override
            {
                return pDWriteFactory->CreateCustomFontFileReference(fontFileReferenceKey, fontFileReferenceKeySize, fontFileLoader, fontFile);
            }

            STDMETHOD(CreateFontFace)(
                DWRITE_FONT_FACE_TYPE fontFaceType,
                UINT32 numberOfFiles,
                _In_reads_(numberOfFiles) IDWriteFontFile* const* fontFiles,
                UINT32 faceIndex,
                DWRITE_FONT_SIMULATIONS fontFaceSimulationFlags,
                _COM_Outptr_ IDWriteFontFace** fontFace
                ) override
            {
                return pDWriteFactory->CreateFontFace(fontFaceType, numberOfFiles, fontFiles, faceIndex, fontFaceSimulationFlags, fontFace);
            }

            STDMETHOD(CreateRenderingParams)(
                _COM_Outptr_ IDWriteRenderingParams** renderingParams
                ) override
            {
                return pDWriteFactory->CreateRenderingParams(renderingParams);
            }

            STDMETHOD(CreateMonitorRenderingParams)(
                HMONITOR monitor,
                _COM_Outptr_ IDWriteRenderingParams** renderingParams
                ) override
            {
                return pDWriteFactory->CreateMonitorRenderingParams(monitor, renderingParams);
            }

            STDMETHOD(CreateCustomRenderingParams)(
                FLOAT gamma,
                FLOAT enhancedContrast,
                FLOAT clearTypeLevel,
                DWRITE_PIXEL_GEOMETRY pixelGeometry,
                DWRITE_RENDERING_MODE renderingMode,
                _COM_Outptr_ IDWriteRenderingParams** renderingParams
                ) override
            {
                return pDWriteFactory->CreateCustomRenderingParams(gamma, enhancedContrast, clearTypeLevel, pixelGeometry, renderingMode, renderingParams);
            }

            STDMETHOD(RegisterFontFileLoader)(
                _In_ IDWriteFontFileLoader* fontFileLoader
                ) override
            {
                return pDWriteFactory->RegisterFontFileLoader(fontFileLoader);
            }

            STDMETHOD(UnregisterFontFileLoader)(
                _In_ IDWriteFontFileLoader* fontFileLoader
                ) override
            {
                return pDWriteFactory->UnregisterFontFileLoader(fontFileLoader);
            }

            STDMETHOD(CreateTextFormat)(
                _In_z_ WCHAR const* fontFamilyName,
                _In_opt_ IDWriteFontCollection* fontCollection,
                DWRITE_FONT_WEIGHT fontWeight,
                DWRITE_FONT_STYLE fontStyle,
                DWRITE_FONT_STRETCH fontStretch,
                FLOAT fontSize,
                _In_z_ WCHAR const* localeName,
                _COM_Outptr_ IDWriteTextFormat** textFormat
                ) override
            {
                return pDWriteFactory->CreateTextFormat(fontFamilyName, fontCollection, fontWeight, fontStyle, fontStretch, fontSize, localeName, textFormat);
            }

            STDMETHOD(CreateTypography)(
                _COM_Outptr_ IDWriteTypography** typography
                ) override
            {
                return pDWriteFactory->CreateTypography(typography);
            }

            STDMETHOD(GetGdiInterop)(
                _COM_Outptr_ IDWriteGdiInterop** gdiInterop
                ) override
            {
                return pDWriteFactory->GetGdiInterop(gdiInterop);
            }

            STDMETHOD(CreateTextLayout)(
                _In_reads_(stringLength) WCHAR const* string,
                UINT32 stringLength,
                _In_ IDWriteTextFormat* textFormat,
                FLOAT maxWidth,
                FLOAT maxHeight,
                _COM_Outptr_ IDWriteTextLayout** textLayout
                ) override
            {
                return pDWriteFactory->CreateTextLayout(string, stringLength, textFormat, maxWidth, maxHeight, textLayout);
            }

            STDMETHOD(CreateGdiCompatibleTextLayout)(
                _In_reads_(stringLength) WCHAR const* string,
                UINT32 stringLength,
                _In_ IDWriteTextFormat* textFormat,
                FLOAT layoutWidth,
                FLOAT layoutHeight,
                FLOAT pixelsPerDip,
                _In_opt_ DWRITE_MATRIX const* transform,
                BOOL useGdiNatural,
                _COM_Outptr_ IDWriteTextLayout** textLayout
                ) override
            {
                return pDWriteFactory->CreateGdiCompatibleTextLayout(string, stringLength, textFormat, layoutWidth, layoutHeight, pixelsPerDip, transform, useGdiNatural, textLayout);
            }

            STDMETHOD(CreateEllipsisTrimmingSign)(
                _In_ IDWriteTextFormat* textFormat,
                _COM_Outptr_ IDWriteInlineObject** trimmingSign
                ) override
            {
                return pDWriteFactory->CreateEllipsisTrimmingSign(textFormat, trimmingSign);
            }

            STDMETHOD(CreateTextAnalyzer)(
                _COM_Outptr_ IDWriteTextAnalyzer** textAnalyzer
                ) override
            {
                return pDWriteFactory->CreateTextAnalyzer(textAnalyzer);
            }

            STDMETHOD(CreateNumberSubstitution)(
                _In_ DWRITE_NUMBER_SUBSTITUTION_METHOD substitutionMethod,
                _In_z_ WCHAR const* localeName,
                _In_ BOOL ignoreUserOverride,
                _COM_Outptr_ IDWriteNumberSubstitution** numberSubstitution
                ) override
            {
                return pDWriteFactory->CreateNumberSubstitution(substitutionMethod, localeName, ignoreUserOverride, numberSubstitution);
            }

            STDMETHOD(CreateGlyphRunAnalysis)(
                _In_ DWRITE_GLYPH_RUN const* glyphRun,
                FLOAT pixelsPerDip,
                _In_opt_ DWRITE_MATRIX const* transform,
                DWRITE_RENDERING_MODE renderingMode,
                DWRITE_MEASURING_MODE measuringMode,
                FLOAT baselineOriginX,
                FLOAT baselineOriginY,
                _COM_Outptr_ IDWriteGlyphRunAnalysis** glyphRunAnalysis
                ) override
            {
                return pDWriteFactory->CreateGlyphRunAnalysis(glyphRun, pixelsPerDip, transform, renderingMode, measuringMode, baselineOriginX, baselineOriginY, glyphRunAnalysis);
            }

        
            ////////////////////////////////////////////////////////
            // IDWriteFactory1
            STDMETHOD(GetEudcFontCollection)(
                _COM_Outptr_ IDWriteFontCollection** fontCollection,
                BOOL checkForUpdates = FALSE
                ) override
            {
                if (uLevel >= 1)
                {
                    return pDWriteFactory1->GetEudcFontCollection(fontCollection, checkForUpdates);
                }

                // 假装没有安装 Eudc字体

                if (!fontCollection)
                    return E_INVALIDARG;
                *fontCollection = nullptr;

                auto _pEmptyCollection = internal::New<DWriteFontCollection>();
                if (!_pEmptyCollection)
                    return E_OUTOFMEMORY;
                *fontCollection = _pEmptyCollection;
                return S_OK;
            }

            STDMETHOD(CreateCustomRenderingParams)(
                FLOAT gamma,
                FLOAT enhancedContrast,
                FLOAT enhancedContrastGrayscale,
                FLOAT clearTypeLevel,
                DWRITE_PIXEL_GEOMETRY pixelGeometry,
                DWRITE_RENDERING_MODE renderingMode,
                _COM_Outptr_ IDWriteRenderingParams1** renderingParams
                ) override
            {
                if(uLevel >= 1)
                {
                    return pDWriteFactory1->CreateCustomRenderingParams(gamma, enhancedContrast, enhancedContrastGrayscale, clearTypeLevel, pixelGeometry, renderingMode, renderingParams);
                }

                if (!renderingParams)
                    return E_NOINTERFACE;

                *renderingParams = nullptr;
                if (enhancedContrastGrayscale < 0.0f)
                    return E_NOINTERFACE;

                IDWriteRenderingParams* _pRenderingParams = nullptr;
                auto _hr = pDWriteFactory->CreateCustomRenderingParams(gamma, enhancedContrast, clearTypeLevel, pixelGeometry, renderingMode, &_pRenderingParams);
                if (FAILED(_hr) || _pRenderingParams == nullptr)
                    return _hr;

                auto _pRenderingParams1 = internal::New<DWriteRenderingParams>(uLevel, _pRenderingParams, enhancedContrastGrayscale);
                _pRenderingParams->Release();
                if (!_pRenderingParams1)
                    return E_OUTOFMEMORY;

                *renderingParams = _pRenderingParams1;
                return S_OK;
            }

            using IDWriteFactory::CreateCustomRenderingParams;

            ////////////////////////////////////////////////////////
            // IDWriteFactory2
            STDMETHOD(GetSystemFontFallback)(
                _COM_Outptr_ IDWriteFontFallback** fontFallback
                ) override
            {
                if (uLevel >= 2)
                {
                    return pDWriteFactory2->GetSystemFontFallback(fontFallback);
                }

                if (!fontFallback)
                    return E_INVALIDARG;

                *fontFallback = nullptr;

                auto _pFontFallback = internal::New<DWriteFontFallback>();
                if (!_pFontFallback)
                    return E_OUTOFMEMORY;

                *fontFallback = _pFontFallback;
                return S_OK;
            }

            STDMETHOD(CreateFontFallbackBuilder)(
                _COM_Outptr_ IDWriteFontFallbackBuilder** fontFallbackBuilder
                ) override
            {
                if (uLevel >= 2)
                {
                    return pDWriteFactory2->CreateFontFallbackBuilder(fontFallbackBuilder);
                }

                if (!fontFallbackBuilder)
                    return E_INVALIDARG;

                *fontFallbackBuilder = nullptr;
                return E_NOTIMPL;
            }

            STDMETHOD(TranslateColorGlyphRun)(
                FLOAT baselineOriginX,
                FLOAT baselineOriginY,
                _In_ DWRITE_GLYPH_RUN const* glyphRun,
                _In_opt_ DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
                DWRITE_MEASURING_MODE measuringMode,
                _In_opt_ DWRITE_MATRIX const* worldToDeviceTransform,
                UINT32 colorPaletteIndex,
                _COM_Outptr_ IDWriteColorGlyphRunEnumerator** colorLayers
                ) override
            {
                if (uLevel >= 2)
                {
                    return pDWriteFactory2->TranslateColorGlyphRun(baselineOriginX, baselineOriginY, glyphRun, glyphRunDescription, measuringMode, worldToDeviceTransform, colorPaletteIndex, colorLayers);
                }

                if (!colorLayers)
                    return E_INVALIDARG;

                *colorLayers = nullptr;
                return E_NOTIMPL;
            }

            STDMETHOD(CreateCustomRenderingParams)(
                FLOAT gamma,
                FLOAT enhancedContrast,
                FLOAT grayscaleEnhancedContrast,
                FLOAT clearTypeLevel,
                DWRITE_PIXEL_GEOMETRY pixelGeometry,
                DWRITE_RENDERING_MODE renderingMode,
                DWRITE_GRID_FIT_MODE gridFitMode,
                _COM_Outptr_ IDWriteRenderingParams2** renderingParams
                ) override
            {
                if (uLevel >= 2)
                {
                    return pDWriteFactory2->CreateCustomRenderingParams(gamma, enhancedContrast, grayscaleEnhancedContrast, clearTypeLevel, pixelGeometry, renderingMode, gridFitMode, renderingParams);
                }

                if (!renderingParams)
                    return E_INVALIDARG;

                *renderingParams = nullptr;

                IDWriteRenderingParams* _pRenderingParams = nullptr;
                HRESULT _hr = S_OK;
                if (uLevel >= 1)
                {
                    _hr = pDWriteFactory1->CreateCustomRenderingParams(gamma, enhancedContrast, grayscaleEnhancedContrast, clearTypeLevel, pixelGeometry, renderingMode, (IDWriteRenderingParams1**)&_pRenderingParams);
                }
                else
                {
                    if (grayscaleEnhancedContrast < 0.0)
                        return E_INVALIDARG;

                    _hr = pDWriteFactory->CreateCustomRenderingParams(gamma, enhancedContrast, clearTypeLevel, pixelGeometry, renderingMode, &_pRenderingParams);
                }

                if (FAILED(_hr) || _pRenderingParams == nullptr)
                    return _hr;
                auto _pTemp = internal::New<DWriteRenderingParams>(uLevel, _pRenderingParams, grayscaleEnhancedContrast, gridFitMode);
                _pRenderingParams->Release();

                if (!_pTemp)
                    return E_OUTOFMEMORY;

                *renderingParams = _pTemp;
                return _hr;
            }

            using IDWriteFactory::CreateCustomRenderingParams;
            using IDWriteFactory1::CreateCustomRenderingParams;

            STDMETHOD(CreateGlyphRunAnalysis)(
                _In_ DWRITE_GLYPH_RUN const* glyphRun,
                _In_opt_ DWRITE_MATRIX const* transform,
                DWRITE_RENDERING_MODE renderingMode,
                DWRITE_MEASURING_MODE measuringMode,
                DWRITE_GRID_FIT_MODE gridFitMode,
                DWRITE_TEXT_ANTIALIAS_MODE antialiasMode,
                FLOAT baselineOriginX,
                FLOAT baselineOriginY,
                _COM_Outptr_ IDWriteGlyphRunAnalysis** glyphRunAnalysis
                ) override
            {
                if (uLevel >= 2)
                {
                    return pDWriteFactory2->CreateGlyphRunAnalysis(glyphRun, transform, renderingMode, measuringMode, gridFitMode, antialiasMode, baselineOriginX, baselineOriginY, glyphRunAnalysis);
                }

                return pDWriteFactory->CreateGlyphRunAnalysis(glyphRun, 1.0f, transform, renderingMode, measuringMode, baselineOriginX, baselineOriginY, glyphRunAnalysis);
            }

            using IDWriteFactory::CreateGlyphRunAnalysis;


            // IDWriteFactory3
            STDMETHOD(CreateGlyphRunAnalysis)(
                _In_ DWRITE_GLYPH_RUN const* glyphRun,
                _In_opt_ DWRITE_MATRIX const* transform,
                DWRITE_RENDERING_MODE1 renderingMode,
                DWRITE_MEASURING_MODE measuringMode,
                DWRITE_GRID_FIT_MODE gridFitMode,
                DWRITE_TEXT_ANTIALIAS_MODE antialiasMode,
                FLOAT baselineOriginX,
                FLOAT baselineOriginY,
                _COM_Outptr_ IDWriteGlyphRunAnalysis** glyphRunAnalysis
                ) override
            {
                if (uLevel >= 3)
                {
                    return pDWriteFactory3->CreateGlyphRunAnalysis(glyphRun, transform, renderingMode, measuringMode, gridFitMode, antialiasMode, baselineOriginX, baselineOriginY, glyphRunAnalysis);
                }

                DWRITE_RENDERING_MODE _eRenderingMode = renderingMode == DWRITE_RENDERING_MODE1_NATURAL_SYMMETRIC_DOWNSAMPLED ? DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC : DWRITE_RENDERING_MODE(renderingMode);
                return CreateGlyphRunAnalysis(glyphRun, transform, _eRenderingMode, measuringMode, gridFitMode, antialiasMode, baselineOriginX, baselineOriginY, glyphRunAnalysis);
            }

            using IDWriteFactory::CreateGlyphRunAnalysis;
            using IDWriteFactory2::CreateGlyphRunAnalysis;

            STDMETHOD(CreateCustomRenderingParams)(
                FLOAT gamma,
                FLOAT enhancedContrast,
                FLOAT grayscaleEnhancedContrast,
                FLOAT clearTypeLevel,
                DWRITE_PIXEL_GEOMETRY pixelGeometry,
                DWRITE_RENDERING_MODE1 renderingMode,
                DWRITE_GRID_FIT_MODE gridFitMode,
                _COM_Outptr_ IDWriteRenderingParams3** renderingParams
                ) override
            {
                if (uLevel >= 3)
                {
                    return pDWriteFactory3->CreateCustomRenderingParams(gamma, enhancedContrast, grayscaleEnhancedContrast, clearTypeLevel, pixelGeometry, renderingMode, gridFitMode, renderingParams);
                }

                if (!renderingParams)
                    return E_INVALIDARG;

                *renderingParams = nullptr;
                DWRITE_RENDERING_MODE _eRenderingMode = renderingMode == DWRITE_RENDERING_MODE1_NATURAL_SYMMETRIC_DOWNSAMPLED ? DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC : DWRITE_RENDERING_MODE(renderingMode);
                IDWriteRenderingParams* _pRenderingParams = nullptr;
                HRESULT _hr = S_OK;
                if (uLevel >= 2)
                {
                    _hr = pDWriteFactory2->CreateCustomRenderingParams(gamma, enhancedContrast, grayscaleEnhancedContrast, clearTypeLevel, pixelGeometry, _eRenderingMode, gridFitMode, (IDWriteRenderingParams2**)&_pRenderingParams);
                }
                else if (uLevel >= 1)
                {
                    DWRITE_RENDERING_MODE _eRenderingMode = renderingMode == DWRITE_RENDERING_MODE1_NATURAL_SYMMETRIC_DOWNSAMPLED ? DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC : DWRITE_RENDERING_MODE(renderingMode);
                    _hr = pDWriteFactory1->CreateCustomRenderingParams(gamma, enhancedContrast, grayscaleEnhancedContrast, clearTypeLevel, pixelGeometry, _eRenderingMode, (IDWriteRenderingParams1**)&_pRenderingParams);
                }
                else
                {
                    if (grayscaleEnhancedContrast < 0.0)
                        return E_INVALIDARG;

                    _hr = pDWriteFactory->CreateCustomRenderingParams(gamma, enhancedContrast, clearTypeLevel, pixelGeometry, _eRenderingMode, &_pRenderingParams);
                }

                if (FAILED(_hr) || _pRenderingParams == nullptr)
                    return _hr;

                auto _pTemp = internal::New<DWriteRenderingParams>(uLevel, _pRenderingParams, grayscaleEnhancedContrast, gridFitMode, renderingMode);
                _pRenderingParams->Release();

                if (!_pTemp)
                    return E_OUTOFMEMORY;

                *renderingParams = _pTemp;
                return _hr;
            }

            using IDWriteFactory::CreateCustomRenderingParams;
            using IDWriteFactory1::CreateCustomRenderingParams;
            using IDWriteFactory2::CreateCustomRenderingParams;

            STDMETHOD(CreateFontFaceReference)(
                _In_z_ WCHAR const* filePath,
                _In_opt_ FILETIME const* lastWriteTime,
                UINT32 faceIndex,
                DWRITE_FONT_SIMULATIONS fontSimulations,
                _COM_Outptr_ IDWriteFontFaceReference** fontFaceReference
                ) override
            {
                if (uLevel >= 3)
                {
                    return pDWriteFactory3->CreateFontFaceReference(filePath, lastWriteTime, faceIndex, fontSimulations, fontFaceReference);
                }

                if (!fontFaceReference)
                    return E_NOINTERFACE;

                *fontFaceReference = nullptr;
                return E_NOTIMPL;
            }

            STDMETHOD(CreateFontFaceReference)(
                _In_ IDWriteFontFile* fontFile,
                UINT32 faceIndex,
                DWRITE_FONT_SIMULATIONS fontSimulations,
                _COM_Outptr_ IDWriteFontFaceReference** fontFaceReference
                ) override
            {
                if (uLevel >= 3)
                {
                    return pDWriteFactory3->CreateFontFaceReference(fontFile, faceIndex, fontSimulations, fontFaceReference);
                }

                if (!fontFaceReference)
                    return E_NOINTERFACE;

                *fontFaceReference = nullptr;
                return E_NOTIMPL;
            }

            STDMETHOD(GetSystemFontSet)(
                _COM_Outptr_ IDWriteFontSet** fontSet
                ) override
            {
                if (uLevel >= 3)
                {
                    return pDWriteFactory3->GetSystemFontSet(fontSet);
                }

                // E:\Chromium\src\content\browser\renderer_host\dwrite_font_proxy_impl_win.cc
                __WarningMessage__("CHrome用到了这个接口 GetSystemFontSet");

                if (!fontSet)
                    return E_NOINTERFACE;

                *fontSet = nullptr;
                return E_NOTIMPL;
            }

            STDMETHOD(CreateFontSetBuilder)(
                _COM_Outptr_ IDWriteFontSetBuilder** fontSetBuilder
                ) override
            {
                if (uLevel >= 3)
                {
                    return pDWriteFactory3->CreateFontSetBuilder(fontSetBuilder);
                }

                __WarningMessage__("CHrome用到了这个接口 CreateFontSetBuilder");

                if (!fontSetBuilder)
                    return E_NOINTERFACE;

                *fontSetBuilder = nullptr;
                return E_NOTIMPL;
            }

            STDMETHOD(CreateFontCollectionFromFontSet)(
                IDWriteFontSet* fontSet,
                _COM_Outptr_ IDWriteFontCollection1** fontCollection
                ) override
            {
                if (uLevel >= 3)
                {
                    return pDWriteFactory3->CreateFontCollectionFromFontSet(fontSet, fontCollection);
                }

                if (!fontCollection)
                    return E_NOINTERFACE;

                *fontCollection = nullptr;
                return E_NOTIMPL;
            }

            STDMETHOD(GetSystemFontCollection)(
                BOOL includeDownloadableFonts,
                _COM_Outptr_ IDWriteFontCollection1** fontCollection,
                BOOL checkForUpdates = FALSE
                ) override
            {
                if (uLevel >= 3)
                {
                    return pDWriteFactory3->GetSystemFontCollection(includeDownloadableFonts, fontCollection, checkForUpdates);
                }

                if (!fontCollection)
                    return E_NOINTERFACE;

                *fontCollection = nullptr;

                IDWriteFontCollection* _pFontCollection = nullptr;
                auto _hr = pDWriteFactory->GetSystemFontCollection(&_pFontCollection, checkForUpdates);
                if (FAILED(_hr) || _pFontCollection == nullptr)
                    return _hr;

                auto _pResult = internal::New<DWriteFontCollection>(_pFontCollection, 0);
                _pFontCollection->Release();
                if (!_pResult)
                    return E_OUTOFMEMORY;

                *fontCollection = _pResult;
                return S_OK;
            }

            using IDWriteFactory::GetSystemFontCollection;

            STDMETHOD(GetFontDownloadQueue)(
                _COM_Outptr_ IDWriteFontDownloadQueue** fontDownloadQueue
                ) override
            {
                if (uLevel >= 3)
                {
                    return pDWriteFactory3->GetFontDownloadQueue(fontDownloadQueue);
                }

                if (!fontDownloadQueue)
                    return E_NOINTERFACE;

                *fontDownloadQueue = nullptr;
                return E_NOTIMPL;
            }
        };
#endif // (YY_Thunks_Target < __WindowsNT10_10240)
    }
}
#endif

namespace YY::Thunks
{
#if (YY_Thunks_Target < __WindowsNT10_10240)

    // 最低受支持的客户端	Windows 7、带 SP2 的 Windows Vista 和适用于 Windows Vista 的平台更新 [桌面应用 |UWP 应用]
    // 最低受支持的服务器	Windows Server 2008 R2、Windows Server 2008 SP2 和适用于 Windows Server 2008 的平台更新[桌面应用 | UWP 应用]
    // Windows 10引入IDWriteFactory3，所以额外进行Thunks
    __DEFINE_THUNK(
    dwrite,
    12,
    HRESULT,
    WINAPI,
    DWriteCreateFactory,
        _In_ DWRITE_FACTORY_TYPE factoryType,
        _In_ REFIID iid,
        _COM_Outptr_ IUnknown **factory
        )
    {
        const auto _pfnDWriteCreateFactory = try_get_DWriteCreateFactory();
        if (!_pfnDWriteCreateFactory)
        {
            if (!factory)
                return E_POINTER;
            *factory = nullptr;
            return E_NOINTERFACE;
        }

        if (iid == __uuidof(IDWriteFactory) || iid == __uuidof(IDWriteFactory1)
            || iid == __uuidof(IDWriteFactory2) || iid == __uuidof(IDWriteFactory3))
        {
            auto _hr = _pfnDWriteCreateFactory(factoryType, __uuidof(IDWriteFactory3), factory);
            if (_hr != E_NOINTERFACE)
            {
                return _hr;
            }

            if (!factory)
                return E_POINTER;
            *factory = nullptr;

            // 不支持IDWriteFactory3，开始伪装IDWriteFactory3支持
            static const GUID kIdLevel[] = 
            {
                __uuidof(IDWriteFactory),
                __uuidof(IDWriteFactory1),
                __uuidof(IDWriteFactory2),
            };

            for (DWORD _uLevel = _countof(kIdLevel); _uLevel;)
            {
                --_uLevel;
                IDWriteFactory* _pDWriteFactory = nullptr;
                _hr = _pfnDWriteCreateFactory(factoryType, kIdLevel[_uLevel], (IUnknown**)&_pDWriteFactory);
                if (_hr == E_NOINTERFACE)
                    continue;

                if (SUCCEEDED(_hr) && _pDWriteFactory)
                {
                    auto _pForward = internal::New<Fallback::DWriteFactory>(_pDWriteFactory, _uLevel);
                    if (_pForward)
                    {
                        _pDWriteFactory->Release();
                        *factory = _pForward;
                    }
                    else
                    {
                        *factory = _pDWriteFactory;
                    }
                }

                break;
            }

            return _hr;
        }
        else
        {
            return _pfnDWriteCreateFactory(factoryType, iid, factory);
        }
    }
#endif
}
