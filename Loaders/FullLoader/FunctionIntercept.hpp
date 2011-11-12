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
#pragma once

#include "easyhook.h"

struct ModuleHooks
{
    TCHAR * name;
    const char * symbol;
    void * func;
};

extern HMODULE gEH_Module;

typedef NTSTATUS (__stdcall * func_EH_Install)(void*, void*, void*, TRACED_HOOK_HANDLE);
typedef NTSTATUS (__stdcall * func_EH_Uninstall)(TRACED_HOOK_HANDLE);
typedef NTSTATUS (__stdcall * func_EH_UninstallAll)();
typedef NTSTATUS (__stdcall * func_EH_SetACL)(ULONG*,ULONG,TRACED_HOOK_HANDLE);
typedef NTSTATUS (__stdcall * func_EH_SetGlobalACL)(ULONG*,ULONG);
extern func_EH_Install gEH_Install;
extern func_EH_Uninstall gEH_Uninstall;
extern func_EH_UninstallAll gEH_UninstallAll;
extern func_EH_SetACL gEH_SetACL;
extern func_EH_SetGlobalACL gEH_SetGlobalACL;

bool WINAPI IsDllLoaded(_In_z_ LPTSTR name);

bool WINAPI InstallDllHook(_In_z_ LPTSTR name, _In_z_ LPCSTR symbol, LPVOID pFunc);
bool WINAPI RemoveDllHook(_In_z_ LPTSTR name, _In_z_ LPCSTR symbol);

int WINAPI InstallHookList(_In_ ModuleHooks * hooks);

bool WINAPI LoadEasyHook();
bool WINAPI UnloadEasyHook();