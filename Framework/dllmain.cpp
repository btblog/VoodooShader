// dllmain.cpp : Defines the entry point for the DLL application.

#include "Meta.hpp"
#include "Core.hpp"
#include "Version.hpp"

using namespace VoodooShader;

BOOL APIENTRY DllMain
(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(ul_reason_for_call);
    UNREFERENCED_PARAMETER(lpReserved);

    return TRUE;
}

