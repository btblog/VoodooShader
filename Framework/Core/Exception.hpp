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

#include "String.hpp"

#include "Includes.hpp"

namespace VoodooShader
{
    /**
     * @addtogroup voodoo_utility Voodoo Utility
     * @{
     */

    /**
     * Generic exception class for Voodoo applications. Combines a generic standard exception with extended error location
     * data.
     *
     * @note If possible, thrown Exceptions will automatically log as much data as they contain in a formatted way. You can
     *      retrieve this string with the Exception::what() method (compatible with std::exception).
     */
    class VOODOO_API Exception :
        public std::exception
    {
    public:
        Exception
        (
            _In_ wchar_t * module,
            _In_ wchar_t * message,
            _In_opt_ ICore * pCore,
            _In_z_ wchar_t * file,
            _In_z_ wchar_t * function,
            _In_ int line
        );

        Exception
        (
            _In_ String module,
            _In_ String Message,
            _In_opt_ ICore * pCore,
            _In_z_ wchar_t * file,
            _In_z_ wchar_t * function,
            _In_ int line
        );

        virtual ~Exception(void);

        virtual const char * what(void);

    private:
        ICore * m_Core;
        String m_Module;
        String m_Message;
        wchar_t * m_File;
        wchar_t * m_Function;
        int m_Line;
        char * m_FmtMsg;
    };
    /**
     * @}
     */
}
