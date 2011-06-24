
#include "Inject.hpp"
#include "Inject_Version.hpp"

BOOL WINAPI DllMain
(
    _In_ HINSTANCE hInst, 
    _In_ unsigned Reason, 
    _In_opt_ void * Reserved
)
{
    UNREFERENCED_PARAMETER(Reserved);

    if ( Reason == DLL_PROCESS_ATTACH )
    {
        ZeroMemory(&gInitParams, sizeof(gInitParams));

        bool init = true;
        init &= GetRunRoot();
        init &= GetLoader(hInst);
        return (BOOL)init;
    } else if ( Reason == DLL_PROCESS_DETACH ) {
        return (BOOL)VoodooShutdown();
    }
}

// Support functions

/**
 * Most DirectX libraries use an identical loading function, with only the name varying.
 * This function takes an SDK version, library name and function name, loads and calls the 
 * proper init function.
 */
IUnknown * WINAPI VoodooDXCreateGeneric(UINT sdkVersion, const PTCHAR lib, const char * func)
{
    typedef IUnknown * (WINAPI * DXInitFunc)(UINT);

    HMODULE baselib = LoadSystemLibrary(lib);

    if ( !baselib )
    {
        ErrorMessage(VSTR("Voodoo Loader: Unable to load system DLL \"%s\"."), lib);
        exit(1);
    }

    DXInitFunc initFunc = (DXInitFunc)GetProcAddress(baselib, func);

    if ( !initFunc )
    {
#ifdef _UNICODE
        ErrorMessage(VSTR("Voodoo Loader: Unable to find system DLL function \"%S\"."), func);
#else
        ErrorMessage(VSTR("Voodoo Loader: Unable to find system DLL function \"%s\"."), func);
#endif
        exit(1);
    }

    IUnknown * dxObj = (*initFunc)(sdkVersion);

    return dxObj;
}

// Various adapter-loading entry points
// Direct3D 8
IUnknown * WINAPI Voodoo3DCreate8(UINT sdkVersion)
{
    IUnknown * D3D8Obj = VoodooDXCreateGeneric(sdkVersion, VSTR("d3d8.dll"), "Direct3DCreate8");

    if ( VoodooStartup() )
    {
        VARIANT * v = ToVariant(D3D8Obj);
        gVoodooAdapter->SetProperty(L"D3D8Object", v);
        D3D8Obj = V_UNKNOWN(v);
        delete v;
    }

    return D3D8Obj;
}

// Direct3D 9
IUnknown * WINAPI Voodoo3DCreate9(UINT sdkVersion)
{
    // Start D3D regularly
    IUnknown * D3D9Obj = VoodooDXCreateGeneric(sdkVersion, VSTR("d3d9.dll"), "Direct3DCreate9");

    // Start Voodoo
    if ( VoodooStartup() )
    {
        // Send the D3D object to the Voodoo adapter, then send whatever is returned to the target.
        // The adapter may replace the pointer to hook d3d at this point.
        VARIANT * v = ToVariant(D3D9Obj);
        gVoodooAdapter->SetProperty(L"D3D9Object", v);
        D3D9Obj = V_UNKNOWN(v);
        delete v;
    }

    return D3D9Obj;
}

// DirectInput 8
HRESULT WINAPI VoodooInput8Create
(
    HINSTANCE hinst,
    DWORD dwVersion,
    REFIID riidltf,
    LPVOID * ppvOut,
    LPVOID punkOuter
)
{
    typedef HRESULT (WINAPI * DIInitFunc)(HINSTANCE, DWORD, REFIID, LPVOID*, LPVOID);

    HMODULE baselib = LoadSystemLibrary(VSTR("dinput8.dll"));
    if ( !baselib )
    {
        ErrorMessage(VSTR("Voodoo Loader: Unable to load system DLL \"dinput8.dll\"."));
        exit(1);
    }

    DIInitFunc initFunc = (DIInitFunc)GetProcAddress(baselib, "DirectInput8Create");
    if ( !initFunc )
    {
        ErrorMessage(VSTR("Voodoo Loader: Unable to find system DLL function \"DirectInput8Create\"."));
        exit(1);
    }

    IUnknown * pDInput = NULL;
    HRESULT hr = (*initFunc)(hinst, dwVersion, riidltf, (void**)&pDInput, punkOuter);

    if ( SUCCEEDED(hr) && VoodooStartup() )
    {
        // Send the D3D object to the Voodoo adapter, then send whatever is returned to the target.
        // The adapter may replace the pointer to hook d3d at this point.
        VARIANT * v = ToVariant(pDInput);
        gVoodooAdapter->SetProperty(L"DInput8Object", v);
        pDInput = V_UNKNOWN(v);
        delete v;
    }

    *ppvOut = pDInput;
    return hr;
}

// DirectInput (unknown version)
HRESULT WINAPI VoodooInputCreateGeneric
(
    HINSTANCE hinst,
    DWORD dwVersion,
    LPVOID * lplpDirectInput,
    LPVOID punkOuter,
    const char * func
)
{
    typedef HRESULT (WINAPI * DIInitFunc)(HINSTANCE, DWORD, LPVOID, LPVOID);

    HMODULE baselib = LoadSystemLibrary(VSTR("dinput.dll"));

    if ( !baselib )
    {
        ErrorMessage(VSTR("Voodoo Loader: Unable to load system DLL \"dinput.dll\"."));
        exit(1);
    }

    DIInitFunc initFunc = (DIInitFunc)GetProcAddress(baselib, func);

    if ( !initFunc )
    {
#ifdef _UNICODE
        ErrorMessage(VSTR("Voodoo Loader: Unable to find system DLL function \"%S\"."), func);
#else
        ErrorMessage(VSTR("Voodoo Loader: Unable to find system DLL function \"%s\"."), func);
#endif
        exit(1);
    }

    IUnknown * pDInput = NULL;
    HRESULT hr = (*initFunc)(hinst, dwVersion, &pDInput, punkOuter);

    if ( SUCCEEDED(hr) && VoodooStartup() )
    {
        // Send the D3D object to the Voodoo adapter, then send whatever is returned to the target.
        // The adapter may replace the pointer to hook d3d at this point.
        VARIANT * v = ToVariant(pDInput);
        gVoodooAdapter->SetProperty(L"DInputObject", v);
        pDInput = V_UNKNOWN(v);
        delete v;
    }

    *lplpDirectInput = pDInput;
    return hr;
}

HRESULT WINAPI VoodooInputCreateA
(
    HINSTANCE hinst,
    DWORD dwVersion,
    LPVOID * lplpDirectInput,
    LPVOID punkOuter
)
{
    return VoodooInputCreateGeneric(hinst, dwVersion, lplpDirectInput, punkOuter, "DirectInputCreateA");
}

HRESULT WINAPI VoodooInputCreateW
(
    HINSTANCE hinst,
    DWORD dwVersion,
    LPVOID * lplpDirectInput,
    LPVOID punkOuter
)
{
    return VoodooInputCreateGeneric(hinst, dwVersion, lplpDirectInput, punkOuter, "DirectInputCreateW");
}


// DirectSound 8
HRESULT WINAPI  VoodooSoundCreate8
(
    LPCGUID lpcGuidDevice,
    LPVOID * ppDS8,
    LPVOID pUnkOuter
)
{
    typedef HRESULT (WINAPI * DSInitFunc)(LPCGUID, LPVOID*, LPVOID);

    HMODULE baselib = LoadSystemLibrary(VSTR("dsound8.dll"));

    if ( !baselib )
    {
        ErrorMessage(VSTR("Voodoo Loader: Unable to load system DLL \"dsound8.dll\"."));
        exit(1);
    }

    DSInitFunc initFunc = (DSInitFunc)GetProcAddress(baselib, "DirectSoundCreate8");

    if ( !initFunc )
    {
        ErrorMessage(VSTR("Voodoo Loader: Unable to find system DLL function \"DirectSoundCreate8\"."));
        exit(1);
    }

    return (*initFunc)(lpcGuidDevice, ppDS8, pUnkOuter);
}
