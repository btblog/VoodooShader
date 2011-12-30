/*
 * This file is part of the Voodoo Shader Framework.
 *
 * Copyright (c) 2010-2011 by Sean Sube
 *
 * The Voodoo Shader Framework is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser
 * General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this program; if not, write to
 * the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 US
 *
 * Support and more information may be found at
 *   http://www.voodooshader.com
 * or by contacting the lead developer at
 *   peachykeen@voodooshader.com
 */

#include "CVoodoo3D8.hpp"
// CVoodoo3D8
#include "CVoodoo3DDevice8.hpp"
// Voodoo DX89
#include "DX9_Adapter.hpp"
#include "DX9_Converter.hpp"
#include "DX9_Version.hpp"
// Voodoo Core
#include "Format.hpp"
#include "Support.inl"

namespace VoodooShader
{
    namespace VoodooDX8
    {
        /**
        * The default, public constructor for CVoodoo3D objects.
        */
        CVoodoo3D8::CVoodoo3D8(UINT sdkVersion, IDirect3D9 * pRealObject) :
            m_SdkVersion(sdkVersion), m_RealObject(pRealObject)
        { }

        CVoodoo3D8::~CVoodoo3D8()
        {

        }

        // IUnknown methods
        HRESULT STDMETHODCALLTYPE CVoodoo3D8::QueryInterface(REFIID riid, void **ppvObj)
        {
            return m_RealObject->QueryInterface(riid, ppvObj);
        }

        ULONG STDMETHODCALLTYPE CVoodoo3D8::AddRef()
        {
            return m_RealObject->AddRef();
        }

        ULONG STDMETHODCALLTYPE CVoodoo3D8::Release()
        {
            ULONG refCount = m_RealObject->Release();

            if (refCount == 0)
            {
                delete this;
                return 0;
            }
            else
            {
                return refCount;
            }
        }

        HRESULT STDMETHODCALLTYPE CVoodoo3D8::CheckDepthStencilMatch
            (
            UINT Adapter,
            D3DDEVTYPE DeviceType,
            D3DFORMAT AdapterFormat,
            D3DFORMAT RenderTargetFormat,
            D3DFORMAT DepthStencilFormat
            )
        {
            HRESULT hr = m_RealObject->CheckDepthStencilMatch
                (Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);

            gpVoodooLogger->LogMessage
            (
                LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::CheckDepthStencilMatch(%d, %d, %d, %d, %d) == %d") << Adapter << DeviceType <<
                AdapterFormat << RenderTargetFormat << DepthStencilFormat << hr
            );

            return hr;
        }

        HRESULT STDMETHODCALLTYPE CVoodoo3D8::CheckDeviceFormat
            (
            UINT Adapter,
            D3DDEVTYPE DeviceType,
            D3DFORMAT AdapterFormat,
            DWORD Usage,
            D3DRESOURCETYPE RType,
            D3DFORMAT CheckFormat
            )
        {
            HRESULT hr = m_RealObject->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);

            gpVoodooLogger->LogMessage
            (
                LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::CheckDeviceFormat(%d, %d, %d, %d, %d, %d) == %d") << Adapter << DeviceType <<
                AdapterFormat << Usage << RType << CheckFormat << hr
            );

            return hr;
        }

        HRESULT STDMETHODCALLTYPE CVoodoo3D8::CheckDeviceMultiSampleType
            (
            UINT Adapter,
            D3DDEVTYPE DeviceType,
            D3DFORMAT SurfaceFormat,
            BOOL Windowed,
            D3DMULTISAMPLE_TYPE MultiSampleType
            )
        {
            HRESULT hr = m_RealObject->CheckDeviceMultiSampleType
                (Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType, NULL);

            gpVoodooLogger->LogMessage
            (
                LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::CheckDeviceMultiSampleType(%d, %d, %d, %d, %d) == %d") << Adapter << DeviceType <<
                SurfaceFormat << Windowed << MultiSampleType << hr
            );

            return hr;
        }

        HRESULT STDMETHODCALLTYPE CVoodoo3D8::CheckDeviceType
            (
            UINT Adapter,
            D3DDEVTYPE CheckType,
            D3DFORMAT DisplayFormat,
            D3DFORMAT BackBufferFormat,
            BOOL Windowed
            )
        {
            HRESULT hr = m_RealObject->CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed);

            gpVoodooLogger->LogMessage
                (
                LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::CheckDeviceType(%d, %d, %d, %d, %d) == %d") << Adapter << CheckType << DisplayFormat <<
                BackBufferFormat << Windowed << hr
                );

            return hr;
        }

        HRESULT STDMETHODCALLTYPE CVoodoo3D8::CreateDevice
            (
            UINT Adapter,
            D3DDEVTYPE DeviceType,
            HWND hFocusWindow,
            DWORD BehaviorFlags,
            D3DPRESENT_PARAMETERS8 * pPresentationParameters,
            IDirect3DDevice8 **ppReturnedDeviceInterface
            )
        {
            D3DPRESENT_PARAMETERS mpPresentationParameters;

            mpPresentationParameters.Flags = pPresentationParameters->Flags;

            if (mpPresentationParameters.Flags & D3DPRESENTFLAG_LOCKABLE_BACKBUFFER)
            {
                mpPresentationParameters.Flags ^= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
            }
            mpPresentationParameters.Windowed = pPresentationParameters->Windowed;
            mpPresentationParameters.BackBufferCount = pPresentationParameters->BackBufferCount;
            mpPresentationParameters.BackBufferFormat = pPresentationParameters->BackBufferFormat;
            mpPresentationParameters.BackBufferHeight = pPresentationParameters->BackBufferHeight;
            mpPresentationParameters.BackBufferWidth = pPresentationParameters->BackBufferWidth;
            mpPresentationParameters.hDeviceWindow = pPresentationParameters->hDeviceWindow;
            mpPresentationParameters.EnableAutoDepthStencil = pPresentationParameters->EnableAutoDepthStencil;
            mpPresentationParameters.SwapEffect = pPresentationParameters->SwapEffect;
            mpPresentationParameters.MultiSampleQuality = 0;
            mpPresentationParameters.MultiSampleType = pPresentationParameters->MultiSampleType;
            mpPresentationParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
            mpPresentationParameters.FullScreen_RefreshRateInHz = 0;
            mpPresentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

#ifdef _DEBUG
            const wchar_t * textureType = VoodooShader::Converter::ToString
                (VoodooShader::VoodooDX9::DX9_Converter::ToTextureFormat(mpPresentationParameters.BackBufferFormat));

            gpVoodooLogger->LogMessage
            (
                LL_ModInfo, VOODOO_DX89_NAME, Format("Backbuffer parameters for new device: %d by %d (%d buffers), %s.") <<
                mpPresentationParameters.BackBufferWidth << mpPresentationParameters.BackBufferHeight <<
                mpPresentationParameters.BackBufferCount << textureType
            );
#endif

            IDirect3DDevice9 * pRealDevice;
            HRESULT hr = m_RealObject->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, &mpPresentationParameters, &pRealDevice);

#ifdef _DEBUG
            gpVoodooLogger->LogMessage
                (
                LL_ModInfo, VOODOO_DX89_NAME, Format("CVoodoo3D8::CreateDevice(%d, %d, %p, %d, %p, %p) == %d") << Adapter << DeviceType <<
                hFocusWindow << BehaviorFlags << &mpPresentationParameters << pRealDevice << hr
                );
#endif
            if (!SUCCEEDED(hr))
            {
                gpVoodooLogger->LogMessage(LL_ModError, VOODOO_DX89_NAME, Format("CVoodoo3D8::CreateDevice failed with error %d on adapter %d.") << hr << Adapter);
                return hr;
            }
            else
            {
                // Succeeded, create a fake device and go from there
                *ppReturnedDeviceInterface = new CVoodoo3DDevice8(pRealDevice);

                Uuid adapterID = VoodooShader::VoodooDX9::CLSID_DX9Adapter;
                VoodooShader::VoodooDX9::DX9Adapter * pDXAdapter = nullptr;
                if (gpVoodooCore->GetAdapter()->QueryInterface(adapterID, (const void**)&pDXAdapter) && pDXAdapter)
                {
                    pDXAdapter->SetDXDevice(pRealDevice);
                }

                HRESULT hrt = pRealDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_LEFT, &backbufferSurf);

                if (SUCCEEDED(hrt))
                {
                    gpVoodooLogger->LogMessage(LL_ModInfo, VOODOO_DX89_NAME, L"Cached backbuffer surface.");
                }
                else
                {
                    gpVoodooLogger->LogMessage(LL_ModError, VOODOO_DX89_NAME, L"Failed to retrieve backbuffer surface.");
                }

                TextureDesc backbufferDesc;
                ZeroMemory(&backbufferDesc, sizeof(TextureDesc));
                backbufferDesc.Size.X = pPresentationParameters->BackBufferWidth;
                backbufferDesc.Size.Y = pPresentationParameters->BackBufferHeight;
                backbufferDesc.Size.Z = 0;
                backbufferDesc.Mipmaps = true;
                backbufferDesc.RenderTarget = true;
                backbufferDesc.Format = VoodooShader::TF_RGB8;

                texture_ThisFrame = gpVoodooCore->CreateTexture(L":thisframe", backbufferDesc);

                if (texture_ThisFrame)
                {
                    IDirect3DTexture9 * texture = (IDirect3DTexture9 *)texture_ThisFrame->GetData();
                    hrt = texture->GetSurfaceLevel(0, &surface_ThisFrame);
                    if (SUCCEEDED(hrt))
                    {
                        gpVoodooLogger->LogMessage(LL_ModInfo, VOODOO_DX89_NAME, L"Cached :thisframe surface.");
                    }
                    else
                    {
                        gpVoodooLogger->LogMessage(LL_ModError, VOODOO_DX89_NAME, L"Failed to cache :thisframe surface.");
                    }
                }

                try
                {
                    IFileRef shaderfile = gpVoodooCore->GetFileSystem()->GetFile(L"test.cgfx");
                    testShader = gpVoodooCore->CreateShader(shaderfile.get(), nullptr);
                }
                catch(std::exception & exc)
                {
                    gpVoodooLogger->LogMessage(LL_ModError, VOODOO_DX89_NAME, Format("Error loading shader: %S") << exc.what());
                }

                return hr;
            }
        }

        HRESULT STDMETHODCALLTYPE CVoodoo3D8::EnumAdapterModes(UINT Adapter, UINT Mode, D3DDISPLAYMODE * pMode)
        {
            HRESULT hr = m_RealObject->EnumAdapterModes(Adapter, D3DFMT_X8R8G8B8, Mode, pMode);

            gpVoodooLogger->LogMessage(LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::EnumAdapterModes(%d, %d, %p) == %d") << Adapter << Mode << pMode << hr);

            return hr;
        }

        UINT STDMETHODCALLTYPE CVoodoo3D8::GetAdapterCount()
        {
            UINT count = m_RealObject->GetAdapterCount();

            gpVoodooLogger->LogMessage(LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::GetAdapterCount() == %d") << count);

            return count;
        }

        HRESULT STDMETHODCALLTYPE CVoodoo3D8::GetAdapterDisplayMode(UINT Adapter, D3DDISPLAYMODE * pMode)
        {
            HRESULT hr = m_RealObject->GetAdapterDisplayMode(Adapter, pMode);

            gpVoodooLogger->LogMessage(LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::GetAdapterDisplayMode(%d, %p) == %d") << Adapter << pMode << hr);

            //! @todo Check if this force-format is necessary.
            //pMode->Format = D3DFMT_X8R8G8B8;

            return hr;
        }

        HRESULT STDMETHODCALLTYPE CVoodoo3D8::GetAdapterIdentifier(UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER8 * pIdentifier)
        {
            D3DADAPTER_IDENTIFIER9 realIdentifier;

            HRESULT hr = m_RealObject->GetAdapterIdentifier(Adapter, 0, &realIdentifier);

            gpVoodooLogger->LogMessage(LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::GetAdapterIdentifier(%d, %d, %p) == %d") << Adapter << Flags << pIdentifier << hr);

            if (SUCCEEDED(hr))
            {
                // Strings, so use copymemory instead of assignment
                ::CopyMemory(pIdentifier->Driver, realIdentifier.Driver, MAX_DEVICE_IDENTIFIER_STRING);
                ::CopyMemory(pIdentifier->Description, realIdentifier.Description, MAX_DEVICE_IDENTIFIER_STRING);
                pIdentifier->DeviceId = realIdentifier.DeviceId;
                pIdentifier->DeviceIdentifier = realIdentifier.DeviceIdentifier;
                pIdentifier->DriverVersion = realIdentifier.DriverVersion;
                pIdentifier->Revision = realIdentifier.Revision;
                pIdentifier->SubSysId = realIdentifier.SubSysId;
                pIdentifier->VendorId = realIdentifier.VendorId;
                pIdentifier->WHQLLevel = realIdentifier.WHQLLevel;
            }
            return hr;
        }

        UINT STDMETHODCALLTYPE CVoodoo3D8::GetAdapterModeCount(UINT Adapter)
        {
            UINT r = m_RealObject->GetAdapterModeCount(Adapter, D3DFMT_X8R8G8B8);
            
            gpVoodooLogger->LogMessage(LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::GetAdapterModeCount(%d) == %d") << Adapter << r);

            return r;
        }

        HMONITOR STDMETHODCALLTYPE CVoodoo3D8::GetAdapterMonitor(UINT Adapter)
        {
            HMONITOR hm = m_RealObject->GetAdapterMonitor(Adapter);

            gpVoodooLogger->LogMessage(LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::GetAdapterMonitor(%d) == %p") << Adapter << hm);

            return hm;
        }

        HRESULT STDMETHODCALLTYPE CVoodoo3D8::GetDeviceCaps(UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS8 * pCaps)
        {
            static D3DCAPS8 d3d8caps;
            static bool d3d8caps_fetched = false;

            if (!d3d8caps_fetched)
            {
                HMODULE d3d8module = LoadSystemLibrary(VSTR("d3d8.dll"));
                if (d3d8module)
                {
                    typedef IDirect3D8 * (WINAPI * d3d8createtype)(UINT);

                    d3d8createtype d3d8create = (d3d8createtype)GetProcAddress(d3d8module, "Direct3DCreate8");
                    if (d3d8create)
                    {
                        IDirect3D8 * pDev8 = d3d8create(m_SdkVersion);
                        if (pDev8)
                        {
                            HRESULT hr = pDev8->GetDeviceCaps(Adapter, DeviceType, &d3d8caps);
                            if (SUCCEEDED(hr))
                            {
                                d3d8caps_fetched = true;
                            }
                        }
                    }
                }
            }

            *pCaps = d3d8caps;
            if (!d3d8caps_fetched)
            {
                return D3DERR_NOTAVAILABLE;
            } else {
                return D3D_OK;
            }
        }

        HRESULT STDMETHODCALLTYPE CVoodoo3D8::RegisterSoftwareDevice(void *pInitializeFunction)
        {
            HRESULT hr = m_RealObject->RegisterSoftwareDevice(pInitializeFunction);

#ifdef _DEBUG
            gpVoodooLogger->LogMessage(LL_ModDebug, VOODOO_DX89_NAME, Format("CVoodoo3D8::RegisterSoftwareDevice(%p) == %d") << pInitializeFunction << hr);
#else
            gpVoodooLogger->LogMessage(LL_ModWarn, VOODOO_DX89_NAME, VSTR("The application has registered a software device with D3D8, illegal behavior."));
#endif

            return hr;
        }
    }
}
