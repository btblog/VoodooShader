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
#pragma once

#include "Includes.hpp"

namespace VoodooShader
{
    /** 
     * @addtogroup VoodooUtility 
     * @{ 
     */
    class StringImpl;

    /**
     * Voodoo internal string class, providing constant and compiler-safe string passing between various modules. Provides
     * methods similar to MFC's CString and Qt's QString, not so much like std::string. Can be easily converted to a
     * std::string (provides a cast if the proper headers are included).
     */
    class VOODOO_API String
    {
    public:
        String()
        {
            this->Init(nullptr);
        }

        String(char * str)
        {
            this->Init(str);
        }

        String(const char * str)
        {
            this->Init(str);
        }

#ifdef _STRING_
        String(std::string & str)
        {
            this->Init(str.c_str());
        }
#endif

        // Modify
        void ToLower();
        void ToUpper();
        void Clear();

        // Part
        String Left(UInt32 count);
        String Mid(UInt32 start, UInt32 count);
        String Right(UInt32 count);

        // Predicates
        Bool StartsWith(const char * str, Bool useCase = true);
        Bool EndsWith(const char * str, Bool useCase = true);
        Bool Contains(const char * str, Bool useCase = true);
        Bool Compare(const char * str, Bool useCase = true);

        // Find
        UInt32 Find(const char * str, Bool useCase = true);
        UInt32 ReverseFind(const char * str, Bool useCase = true);

        // Replace
        void Replace(const char * find, const char * replace, Bool useCase = true);
        void Erase(const char * find, Bool useCase = true);

        // Format
        static String Format(_Printf_format_string_ const char * fmt, ...);
        static String FormatV(const char * fmt, va_list args);
        void AppendFormat(_Printf_format_string_ const char * fmt, ...);

        // Buffer info/access
        UInt32 GetLength();
        Bool IsEmpty();
        const char * GetData();

        char GetAt(UInt32 pos);
        char & operator[](UInt32 pos);
        void SetAt(UInt32 pos, char data);

        operator const char *();
#ifdef _STRING_
        operator std::string()
        {
            return std::string(this->GetData());
        };
#endif

        String & operator=(const char * other);
        String operator+(const char * other);
        String & operator+=(const char * other);

        Bool operator==(const char * other);
        Bool operator!=(const char * other);
        Bool operator<(const char * other);

    private:
        void Init(const char * str);

        StringImpl * m_Impl;
    };
    /**
     * @}
     */
}
