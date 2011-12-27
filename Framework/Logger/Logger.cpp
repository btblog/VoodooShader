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

#include "Logger.hpp"

#include "Logger_Version.hpp"

#include <strsafe.h>

#pragma warning(push)
#pragma warning(disable: 6246)
#include <boost/uuid/uuid_io.hpp>
#pragma warning(pop)

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

namespace VoodooShader
{
    namespace XmlLogger
    {
        static const Version moduleVersion = VOODOO_META_VERSION_STRUCT(LOGGER);
        const wchar_t * name_VSXmlLogger = L"VSXmlLogger";
        static const Uuid clsid_VSXmlLogger = CLSID_VSXmlLogger;

        const Version * VOODOO_CALLTYPE API_ModuleVersion()
        {
            return &moduleVersion;
        }

        const uint32_t VOODOO_CALLTYPE API_ClassCount()
        {
            return 1;
        }

        const wchar_t * VOODOO_CALLTYPE API_ClassInfo(_In_ const uint32_t index, _Out_ Uuid * pUuid)
        {
            if (!pUuid)
            {
                return nullptr;
            }

            if (index == 0)
            {
                *pUuid = clsid_VSXmlLogger;
                return name_VSXmlLogger;
            }

            return nullptr;
        }

        IObject * VOODOO_CALLTYPE API_ClassCreate (_In_ const uint32_t number, _In_ ICore * pCore)
        {
            if (number == 0)
            {
                return new VSXmlLogger(pCore);
            }
            else
            {
                return nullptr;
            }
        }

        VSXmlLogger::VSXmlLogger(_In_ ICore * pCore) :
            m_Core(pCore), m_LogLevel(LL_Initial)
        {
        }

        VSXmlLogger::~VSXmlLogger()
        {
            if (this->m_LogFile.is_open())
            {
                this->Close();
            }
        }

        uint32_t VSXmlLogger::AddRef() const
        {
            return SAFE_INCREMENT(m_Refs);
        }

        uint32_t VSXmlLogger::Release() const
        {
            if (SAFE_DECREMENT(m_Refs) == 0)
            {
                delete this;
                return 0;
            } else {
                return m_Refs;
            }
        }

        bool VSXmlLogger::QueryInterface(_In_ Uuid & clsid, _Deref_out_opt_ const void ** ppOut) const
        {
            if (!ppOut)
            {
                if (clsid.is_nil())
                {
                    clsid = CLSID_VSXmlLogger;
                    return true;
                } else {
                    return false;
                }
            } else {
                if (clsid == IID_IObject)
                {
                    *ppOut = static_cast<const IObject*>(this);
                } else if (clsid == IID_ILogger) {
                    *ppOut = static_cast<const ILogger*>(this);
                } else if (clsid == CLSID_VSXmlLogger) {
                    *ppOut = static_cast<const VSXmlLogger*>(this);
                } else {
                    *ppOut = nullptr;
                    return false;
                }

                reinterpret_cast<const IObject*>(*ppOut)->AddRef();
                return true;
            }
        }

        String VSXmlLogger::ToString() const
        {
            return name_VSXmlLogger;
        }

        ICore * VSXmlLogger::GetCore() const
        {
            return m_Core;
        }

        bool VSXmlLogger::Open(_In_ const String & filename, _In_ const bool append)
        {
            if (this->m_LogFile.is_open())
            {
                this->Close();
            }

            unsigned int flags = ios_base::out;

            if (append)
            {
                flags |= ios_base::app;
            }
            else
            {
                flags |= ios_base::trunc;
            }

            this->m_LogFile.open(filename.GetData(), flags);

            if (this->m_LogFile.is_open())
            {
#ifdef _DEBUG
                this->SetFlags(LF_Flush);
#endif

                wstringstream logMsg;

                logMsg << VSTR("<?xml version='1.0'?>\n");
                logMsg << VSTR("<VoodooLog ");
                logMsg << String::Data() << VSTR(" ");
                logMsg << String::Time().GetData() << VSTR(" ");
                logMsg << String::Ticks().GetData() << VSTR(" ");
                logMsg << VSTR(">\n");

#ifdef _DEBUG
                std::wcout << logMsg.str();
#endif
                m_LogFile << logMsg.str();

                this->Log(LL_ModInfo, VOODOO_LOGGER_NAME, VSTR("Log file opened by VSXmlLogger::Open."));

                this->LogModule(API_ModuleVersion());

                return true;
            }
            else
            {
                return false;
            }
        }

        bool VSXmlLogger::Open(_In_ IFile *const pFile, _In_ const bool append)
        {
            return this->Open(pFile->GetPath(), append);
        }

        bool VSXmlLogger::Close()
        {
            if (this->m_LogFile.is_open())
            {
                this->m_LogFile.close();
                return true;
            } else {
                return false;
            }
        }

        bool VSXmlLogger::Flush()
        {
            if (this->m_LogFile.is_open())
            {
                this->m_LogFile.flush();
                return true;
            } else {
                return false;
            }
        }

        void VSXmlLogger::LogModule(const Version * const pVersion)
        {
            if (!pVersion || !this->m_LogFile.is_open()) return;

            wstringstream logMsg;

            logMsg <<
                VSTR("    <Module uuid=\"") << pVersion->LibId << VSTR("\" ") <<
                VSTR(" major=\"") << pVersion->Major << VSTR("\" ") <<
                VSTR(" minor=\"") << pVersion->Minor << VSTR("\" ") <<
                VSTR(" patch=\"") << pVersion->Patch << VSTR("\" ") <<
                VSTR(" build=\"") << pVersion->Build << VSTR("\" ") <<
                VSTR(" debug=\"") << pVersion->Debug << VSTR("\" ");

            if (pVersion->Name)  logMsg << VSTR(" name=\"")  << pVersion->Name  << VSTR("\" ");
            if (pVersion->RevId) logMsg << VSTR(" revid=\"") << pVersion->RevId << VSTR("\" ");

            logMsg << VSTR(" />\n");

#ifdef VOODOO_DEBUG_CONSOLE
            cout << logMsg.str();
#endif
            m_LogFile << logMsg.str();

#ifndef _DEBUG
            if (m_Flags & LF_Flush)
#endif
            {
                m_LogFile << flush;
            }
        }

        void VSXmlLogger::Log(const LogLevel level, const String & source, const String & msg)
        {
            if (!this->m_LogFile.is_open()) return;

            LogLevel mask = (LogLevel) (level & m_Filter);
            if (!(mask & LL_Severity) || !(mask & LL_Origin)) return;

            try
            {
                // Format the message in memory to prevent partial messages from being dumped
                Stream logMsg;
                logMsg << VSTR("Message(Level: ") << level << VSTR("; Ticks: ") << String::Ticks() <<
                    VSTR("Source: ") << source << VSTR("; Message: ") << msg << VSTR(")");

#ifdef _DEBUG
                if (level & (LL_ModWarn | LL_ModError))
                {
                    OutputDebugString(logMsg.To);

#   ifdef VOODOO_DEBUG_CONSOLE
                    cout << logMsg.str();
#   endif
                }
#endif
                m_LogFile << logMsg.str();

#ifndef _DEBUG
                if (m_Flags & LF_Flush)
#endif
                {
                    m_LogFile << flush;
                }
            }
            catch(const std::exception & exc)
            {
#ifdef _DEBUG
                OutputDebugStringA(exc.what());
#else
                UNREFERENCED_PARAMETER(exc);
#endif
            }
        }

        void VSXmlLogger::SetFlags(const LogFlags flags)
        {
            m_Flags = flags;
        }

        LogFlags VSXmlLogger::GetFlags() const
        {
            return m_Flags;
        }

        void VSXmlLogger::SetLogLevel(const LogLevel level)
        {
            m_LogLevel = level;
        }

        LogLevel VSXmlLogger::GetLogLevel() const
        {
            return m_LogLevel;
        }

        String VSXmlLogger::LogTime() const
        {
            time_t now = time(nullptr);
            tm localTime;

            if (localtime_s(&localTime, &now) == 0)
            {
                wstringstream stamp;

                stamp << VSTR("time=\"") << put_time(&localTime, VSTR("%H%M%S")) << VSTR("\"");
                return stamp.str();
            }
            else
            {
                return String(VSTR("time=\"000000\""));
            }
        }

        String VSXmlLogger::LogDate() const
        {
            time_t now = time(nullptr);
            tm localTime;

            if (localtime_s(&localTime, &now) == 0)
            {
                wstringstream stamp;

                stamp << VSTR("date=\"") << put_time(&localTime, VSTR("%Y%m%d")) << VSTR("\"");
                return stamp.str();
            }
            else
            {
                return String(VSTR("date=\"00000000\""));
            }
        }

        String VSXmlLogger::LogTicks() const
        {
            return String::Format(VSTR("ticks=\"%d\""), GetTickCount());
        }
    }
}
