/**
 * This file is part of the Voodoo Shader Framework, a comprehensive shader support library. 
 * 
 * Copyright (c) 2010-2011 by Sean Sube 
 * 
 * This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version.  This program is distributed in the hope that it will be useful, but WITHOUT ANY 
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License along with this program; if not, write to 
 * the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 US 
 * 
 * Support and more information may be found at 
 *   http://www.voodooshader.com
 * or by contacting the lead developer at 
 *   peachykeen@voodooshader.com
 */

#include "VSHookManager.hpp"

namespace VoodooShader
{
    VSHookManager::VSHookManager(_In_ ICore * pCore) 
        : m_Core(pCore)
    { }

    VSHookManager::~VSHookManager(void)
    { }

    int32_t VSHookManager::AddRef() const
    {
        return ++m_Refs;
    }

    int32_t VSHookManager::Release() const
    {
        if (--m_Refs == 0)
        {
            delete this;
            return 0;
        } else {
            return m_Refs;
        }
    }

    String VSHookManager::ToString(void) const
    {
        return L"VSHookManager";
    }

    ICore * VSHookManager::GetCore(void) const
    {
        return m_Core;
    }

    bool VSHookManager::Add(_In_ const String & name, _In_ void * pSrc, _In_ void * pDest)
    {
        UNREFERENCED_PARAMETER(name);
        UNREFERENCED_PARAMETER(pSrc);
        UNREFERENCED_PARAMETER(pDest);

        return true;
    }

    bool VSHookManager::Remove(_In_ const String & name)
    {
        UNREFERENCED_PARAMETER(name);

        return true;
    }

    void VSHookManager::RemoveAll(void)
    { }
}
