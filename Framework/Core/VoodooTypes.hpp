/*
 * This file is part of the Voodoo Shader Framework.
 *
 * Copyright (c) 2010-2013 by Sean Sube
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

#pragma warning(push,3)
#include <cstdint>
#pragma warning(pop)

#if !defined(VOODOO_NO_BOOST)
#   pragma warning(push,3)
#   include <boost/intrusive_ptr.hpp>
#   include <boost/uuid/uuid.hpp>
#   pragma warning(pop)
#   if !defined(VOODOO_NO_STDLIB)
#       pragma warning(push,3)
#       include <list>
#       include <map>
#       include <vector>
#       pragma warning(pop)
#   endif
#endif

#if !defined(VOODOO_NO_PUGIXML)
#   pragma warning(push,3)
#   include <pugixml.hpp>
#   pragma warning(pop)
#endif

namespace VoodooShader
{    
    // Disable typed-enum warning
#pragma warning(push)
#pragma warning(disable: 4480)
    /**
     * @defgroup voodoo_types Voodoo Shader Types
     * @{
     * @defgroup voodoo_enums Enums
     * @{
     */
    /**
     * Texture formats for use by @ref VoodooShader::ITexture "textures", describing the layout and size of the texture
     * data. These may not be implemented by the underlying graphics API exactly as they are indicated here, but the
     * available components and sizes are guaranteed to be equal to or greater than the indicated values (with the exception
     * of VSFmt_Null and VSFmt_DMax, which have no and variable precision). Further information on texture formats may be 
     * found on the @ref voodoo_textures_formats "texture formats chart".
     * 
     * Many bindings restrict the use of depth textures as inputs to effects. The listed formats are @a mostly compatible
     * for use as such, but this is still very hardware-dependent. See @ref voodoo_textures_depth for details.
     */
    enum TextureFormat : uint32_t
    {
        VSFmt_Unknown       = 0x0000,   /* !< Unknown texture format. */
        // Null texture format
        VSFmt_Null          = 0x0001,   /* !< Has no data and takes no space. */
        // General texture formats
        VSFmt_RGB5A1        = 0x0102,   /* !< 5 bit RGB, 1 bit alpha */
        VSFmt_RGB8          = 0x0103,   /* !< 8 bit RGB (8 bit X in D3D). Most common backbuffer format, common texture format. */
        VSFmt_RGBA8         = 0x0104,   /* !< 8 bit RGBA. Common texture format. */
        VSFmt_RGB10A2       = 0x0105,   /* !< 10 bit RGB, 2 bit A */
        // Float texture formats
        VSFmt_RGBA16F       = 0x0201,   /* !< Half-precision RGBA. HDR format. */
        VSFmt_RGBA32F       = 0x0202,   /* !< Full-precision RGBA (float/single). HDR format. */
        // Depth texture formats
        VSFmt_D16           = 0x0401,   /* !< Half-precision depth, with no stencil buffer. */
        VSFmt_D24           = 0x0402,   /* !< Partial precision depth buffer, with no stencil buffer. */
        VSFmt_D24S8         = 0x0403,   /* !< 24-bit precision depth buffer, with stencil buffer. */
        VSFmt_D32           = 0x0404,   /* !< Full-precision depth, with no stencil buffer. */
        VSFmt_DMax          = 0x0405,   /* !< Unspecified precision depth buffer, with no stencil buffer. */
    };

    enum ParameterType : uint32_t
    {
        VSPT_Unknown        = 0x00,     /* !< Unknown parameter category. */
        VSPT_Bool           = 0x01,
        VSPT_Int            = 0x02,
        VSPT_Float          = 0x03,     /* !< float vector parameter (may have 1 to 4 components). */
        VSPT_String         = 0x04,
        VSPT_Texture        = 0x05,
        VSPT_Texture1D      = 0x06,
        VSPT_Texture2D      = 0x07,
        VSPT_Texture3D      = 0x08,
        VSPT_TextureCube    = 0x09,
        VSPT_Sampler        = 0x0A,
        VSPT_Sampler1D      = 0x0B,     /* !< Sampler parameter (may sample 1D to 3D textures). */
        VSPT_Sampler2D      = 0x0C,
        VSPT_Sampler3D      = 0x0D,
        VSPT_SamplerCube    = 0x0E,
    };

    enum ShaderStage : uint32_t
    {
        VSStage_Unknown      = 0x00,     /* !< Unknown stage */
        VSStage_Vertex       = 0x01,     /* !< Vertex shader stage (usually supported, see @ref programstages "program stages" for more info). */
        VSStage_Hull         = 0x02,     /* !< Hull shader stage (not always supported, see  @ref programstages "program stages" for more info). */
        VSStage_Pixel        = 0x04,     /* !< Fragment shader stage (usually supported, see @ref programstages "program stages" for more info). */
        VSStage_Domain       = 0x08,     /* !< Domain shader stage (not always supported, see @ref programstages "program stages" for more info). */
        VSStage_Geometry     = 0x10,     /* !< Geometry shader stage (sometimes supported, see @ref programstages "program stages" for more info). */
        VSStage_Compute      = 0x20,     /* !< Compute shader stage. Rarely supported. */
        VSStage_All          = 0x3F,     /* !< All available shader stages. */
    };

    /**
     * Target compiler profiles. These control the compiler used for Voodoo Shader resources.
     *
     * Various portions of this enum are reserved or have a special purpose:
     *   - @a 0x00FF: Version within the API. Typically this matches the version the API itself reports.
     *   - @a 0x7F00: Major API or vendor. The unset API is reserved for internal Voodoo Shader profiles.
     *   - @a 0x8000: Debug flag, where the API provides a debug version.
     */
    enum CompilerProfile : uint32_t
    {
        VSProfile_None      = 0x0000,   /* !< Compile with the core's default flags. */
        VSProfile_Refr      = 0x0010,
        VSProfile_D3D9      = 0x0190,
        VSProfile_D3D10     = 0x01A0,
        VSProfile_D3D10_1   = 0x01A1,
        VSProfile_D3D11     = 0x01B0,
        VSProfile_D3D11_1   = 0x01B1,
        VSProfile_OpenGL2   = 0x0220,
        VSProfile_OpenGL3   = 0x0230,
        VSProfile_OpenGL4   = 0x0240,
    };

    enum TextureMode : uint32_t
    {
        VSTexMode_Target    = 0x00,     /* !< The texture is bound as a target, to be rendered to. This imposes many restrictions on the texture, see @ref voodoo_textures_target. */
        VSTexMode_Source    = 0x01,     /* !< The texture is bound as a source, for sampling operations. This is typically handled automatically by the pass. */
    };

    enum TextureFlags : uint32_t
    {
        VSTexFlag_None      = 0x00,
        VSTexFlag_Target    = 0x01,
        VSTexFlag_Dynamic   = 0x02,
        VSTexFlag_MipMaps   = 0x04,
        VSTexFlag_AutoMip   = 0x08,
    };

    /**
     * File stream type for seek and get operations.
     */
    enum StreamType : uint32_t
    {
        VSStream_Unknown    = 0x00,
        VSStream_Get        = 0x01,
        VSStream_Put        = 0x02
    };

    /**
     * File seek offset modes.
     */
    enum SeekMode : uint32_t
    {
        VSSeek_Unknown      = 0x00,
        VSSeek_Begin        = 0x01,     /* !< Seek relative to the beginning of the file (an absolute offset). */
        VSSeek_Current      = 0x02,     /* !< Seek relative to the current position (forward or back). */
        VSSeek_End          = 0x03      /* !< Seek relative to the end of the file. */
    };

    /**
     * File find modes.
     */
    enum GetFileMode : uint32_t
    {
        VSSearch_Open       = 0x00,     /* !< Open the file if it exists, or create it if it does not. */
        VSSearch_Create     = 0x01,     /* !< Create the file in the first possible directory, or fail if it already exists. */
        VSSearch_Exists     = 0x02,     /* !< Open the file, or fail if it does not exist. */
    };

    /**
     * File open access modes.
     */
    enum FileOpenMode : uint32_t
    {
        VSOpen_Unknown      = 0x00,
        VSOpen_Read         = 0x01,     /* !< Read-only access. */
        VSOpen_Write        = 0x02,     /* !< Write-only access. */
        VSOpen_ReadWrite    = VSOpen_Read | VSOpen_Write, /* !< Read/write access. */
        VSOpen_Binary       = 0x10,     /* !< Open the file in binary mode. This guarantees the data will not be modified by the system when read (ie, normalizing line breaks). */
        VSOpen_End          = 0x10,     /* !< Set position to the end of the file (if not set, position defaults to the beginning). */
        VSOpen_Append       = 0x20,     /* !< Write operations are performed at the end of the file. Not compatible with VSOpen_Read. */
        VSOpen_Truncate     = 0x40,     /* !< If the file is not empty, all contents are erased. */
    };

    enum PathType : uint32_t
    {
        VSPath_Directory    = 0x00,
        VSPath_Archive      = 0x01,
    };

    /**
     * Log message levels. These are set up to quickly filter messages based on severity and source. Each message must have
     * a severity and source bit set, which the logger tests against its internal filter, like so:
     *
     * @code
     * LogLevel maskedLevel = level & storedLevel;
     * if ( (maskedLevel & VSLog_Origin) && (maskedLevel & VSLog_Severity) )
     * {
     *     // Log message
     * }
     * @endcode
     */
    enum LogLevel : uint32_t
    {
        VSLog_Unknown      = 0x00,
        // Severity
        VSLog_Debug        = 0x0001,   /* !< Verbose debugging message. May cause significant performance hit. */
        VSLog_Info         = 0x0002,   /* !< General information. */
        VSLog_Warning      = 0x0004,   /* !< Potential problem, not fatal within program or function scope. */
        VSLog_Error        = 0x0008,   /* !< Problem, fatal at function scope but not program. */
        // Source
        VSLog_External     = 0x0100,   /* !< Completely external source (network, pipe, IPC, etc). */
        VSLog_Plugin       = 0x0200,   /* !< Voodoo plugin module. Module must be loaded as a plugin. */
        VSLog_Binding      = 0x0400,   /* !< Voodoo binding module. Module must be loaded as a plugin and acting as the hardware binding. */
        VSLog_Core         = 0x0800,   /* !< Voodoo core module. Must not be used elsewhere. */
        // Flags
        VSLog_Critical     = 0x010000, /* !< Special flag: message must be logged, regardless of settings. Typically for notices. */
        // Compound
        VSLog_Exception    = VSLog_Debug | VSLog_Critical, /* !< Caught exception from Voodoo or other code. */
        VSLog_Notice       = VSLog_Info  | VSLog_Critical, /* !< Important info (module loaded, copyright banner, etc). */
        // User
        VSLog_CoreDebug    = VSLog_Core | VSLog_Debug,
        VSLog_CoreException= VSLog_Core | VSLog_Exception,
        VSLog_CoreInfo     = VSLog_Core | VSLog_Info,
        VSLog_CoreNotice   = VSLog_Core | VSLog_Notice,
        VSLog_CoreWarning  = VSLog_Core | VSLog_Warning,
        VSLog_CoreError    = VSLog_Core | VSLog_Error,
        VSLog_BindDebug    = VSLog_Binding | VSLog_Debug,
        VSLog_BindException= VSLog_Binding | VSLog_Exception,
        VSLog_BindInfo     = VSLog_Binding | VSLog_Info,
        VSLog_BindNotice   = VSLog_Binding | VSLog_Notice,
        VSLog_BindWarning  = VSLog_Binding | VSLog_Warning,
        VSLog_BindError    = VSLog_Binding | VSLog_Error,
        VSLog_PlugDebug    = VSLog_Plugin | VSLog_Debug,
        VSLog_PlugException= VSLog_Plugin | VSLog_Exception,
        VSLog_PlugInfo     = VSLog_Plugin | VSLog_Info,
        VSLog_PlugNotice   = VSLog_Plugin | VSLog_Notice,
        VSLog_PlugWarning  = VSLog_Plugin | VSLog_Warning,
        VSLog_PlugError    = VSLog_Plugin | VSLog_Error,
        // Masks
        VSLog_Severity     = VSLog_Debug | VSLog_Info | VSLog_Warning | VSLog_Error,
        VSLog_Flags        = VSLog_Critical,
        VSLog_Origin       = VSLog_External | VSLog_Plugin | VSLog_Core | VSLog_Binding,
        // Other
        VSLog_Default      = VSLog_Info | VSLog_Warning | VSLog_Plugin | VSLog_Core | VSLog_Binding,
        VSLog_All          = VSLog_Severity | VSLog_Origin | VSLog_Flags
    };

    enum LogFlags  : uint32_t
    {
        VSLogFlag_Unknown   = 0x00,
        VSLogFlag_Flush     = 0x01,     /* !< Log will be flushed to disk after every message. */
    };

    /**
     * String parsing flags. These modify the behavior of the string parser.
     * 
     * Slash flags are handled before path flags. Path flags use system API calls to perform their processing, and are
     * dependent on system behavior. More flags will be added when logic, functions and operators are added to variables.
     */
    enum ParseFlags : uint32_t
    {
        VSParse_None        = 0x00,

        VSParse_SlashSingle = 0x0001,   /* !< Compress any consecutive slashes into one. */
        VSParse_SlashDouble = 0x0002,   /* !< Double any slashes found. */
        VSParse_SlashEscape = 0x0004,   /* !< Prefix any slashes with a backslash. This occurs after all other slash flags. */
        VSParse_SlashOnly   = 0x0010,   /* !< Replace all slashes with forward slashes. */
        VSParse_SlashBack   = 0x0020,   /* !< Replace all slashes with back slashes. */
        VSParse_SlashTrail  = 0x0100,   /* !< Append a slash to the end of the string. This occurs before all other slash flags. */

        VSParse_PathRoot    = 0x001000, /* !< Isolate the root of a path (eg "C:\file.txt" becomes "C:"). */
        VSParse_PathOnly    = 0x002000, /* !< Strip a trailing filename, if one is present (eg "C:\dir\file.txt" becomes "C:\dir"). */
        VSParse_PathFile    = 0x004000, /* !< Strip a path, leaving only filename or last element (eg "C:\dir\file.txt" becomes "file.txt", and "C:\dir\other" becomes "other"). */
        VSParse_PathExt     = 0x008000, /* !< Isolate the file extension, if one is present (eg "C:\dir\file.txt" becomes "txt"). */
        VSParse_PathCanon   = 0x010000, /* !< Canonicalize the path (replace any relative tokens, eg "C:\dir\..\file.txt" becomes "C:\file.txt"). */

        VSParse_SlashFlags  = 0x00000137,
        VSParse_PathFlags   = 0x000FF000,
    };

    enum VariableType  : uint32_t
    {
        VSVar_Normal        = 0x00,
        VSVar_State         = 0x01,
        VSVar_Global        = 0x02,
        VSVar_System        = 0x10,
    };

    enum UnionType : uint32_t
    {
        VSUT_Unknown        = 0x00,
        VSUT_None           = 0x01,
        VSUT_Bool           = 0x02,
        VSUT_Int8           = 0x03,
        VSUT_UInt8          = 0x04,
        VSUT_Int16          = 0x05,
        VSUT_UInt16         = 0x06,
        VSUT_Int32          = 0x07,
        VSUT_UInt32         = 0x08,
        VSUT_Float          = 0x09,
        VSUT_Double         = 0x0A,
        VSUT_Uuid           = 0x0B,
        VSUT_String         = 0x0C,
        VSUT_IObject        = 0x0D,
        VSUT_PVoid          = 0x0F,
    };
#pragma warning(pop)
    /**
     * @}
     * @defgroup voodoo_classes_conditional Conditional Classes
     * @{
     */
#if !defined(VOODOO_NO_BOOST)
    typedef boost::uuids::uuid Uuid;
#else
    typedef struct
    {
        uint8_t data[16];
    } Uuid;
#endif
#if !defined(VOODOO_NO_PUGIXML)
    typedef pugi::xml_document * XmlDocument;
    typedef pugi::xml_node * XmlNode;
#else
    typedef void * XmlDocument;
    typedef void * XmlNode;
#endif
    /**
     * @}
     * @defgroup voodoo_classes_uuids Predefined Uuids
     * @{
     */
    DEFINE_UUID(NilUuid) = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    namespace PropIds
    {
        DEFINE_UUID(D3D8Object)    = {0xe0, 0xc0, 0x4d, 0x6a, 0x21, 0x37, 0x43, 0x68, 0x9b, 0x80, 0xfa, 0xe4, 0x30, 0x94, 0x52, 0xa5};
        DEFINE_UUID(D3D9Object)    = {0x9c, 0x34, 0x50, 0x6a, 0x72, 0xf4, 0x48, 0x82, 0xb1, 0xa4, 0x61, 0xc0, 0x30, 0x9a, 0xc6, 0x15};
        DEFINE_UUID(D3D8Device)    = {0xb4, 0x44, 0x4f, 0xfb, 0x7a, 0x5b, 0x47, 0xa5, 0x88, 0x5f, 0xa4, 0x34, 0x31, 0xb9, 0x08, 0xf7};
        DEFINE_UUID(D3D9Device)    = {0x63, 0xd7, 0xff, 0xbf, 0x54, 0x43, 0x45, 0x7b, 0xa9, 0xc7, 0xf4, 0x11, 0xf2, 0x8b, 0x3b, 0x0e};
        DEFINE_UUID(D3D9Texture)   = {0xb3, 0x82, 0x30, 0x82, 0x53, 0x62, 0x46, 0xd4, 0xb8, 0x98, 0xb3, 0x58, 0x95, 0x0f, 0x6f, 0x0e};
        DEFINE_UUID(D3D9Surface)   = {0x4c, 0xe7, 0xa6, 0xb5, 0x13, 0x1b, 0x4d, 0x08, 0x84, 0x4c, 0x55, 0xa6, 0x17, 0xb6, 0xb3, 0x5d};
        DEFINE_UUID(D3DX9Effect)   = {0xb5, 0x8d, 0xc7, 0xf0, 0xde, 0x72, 0x4e, 0xcd, 0xa0, 0x0c, 0x01, 0x8e, 0xae, 0xf5, 0x63, 0x4b};
        DEFINE_UUID(D3DX9Handle)   = {0xdf, 0x34, 0x48, 0x55, 0xc1, 0x1e, 0x4f, 0xf6, 0x8c, 0x0a, 0x1e, 0x1f, 0xba, 0xea, 0xb6, 0xa8};
        DEFINE_UUID(D3DX9PassId)   = {0x3b, 0xfe, 0x94, 0x9c, 0x11, 0x5e, 0x48, 0x12, 0xb1, 0xa2, 0x77, 0xc0, 0x41, 0xbb, 0xcd, 0x69};
        DEFINE_UUID(D3DSdkVersion) = {0xf7, 0xc8, 0xec, 0x01, 0x95, 0x7f, 0x48, 0xf1, 0x91, 0xbc, 0x1a, 0x9b, 0xa0, 0x84, 0xec, 0xc4};
        DEFINE_UUID(OpenGLTexture) = {0x98, 0x9f, 0x58, 0x94, 0x19, 0xb9, 0x47, 0x8f, 0xb0, 0xb7, 0x0f, 0x40, 0xbc, 0x76, 0xeb, 0xd5};
        DEFINE_UUID(OpenGLContext) = {0x1e, 0x88, 0xc6, 0x7a, 0x49, 0xa6, 0x42, 0x2b, 0xb7, 0x95, 0xb1, 0x54, 0x26, 0x90, 0xf7, 0x19};
        DEFINE_UUID(DI8Version)    = {0xe6, 0x22, 0x28, 0x0b, 0x2c, 0x19, 0x41, 0xdd, 0xa0, 0xa4, 0x25, 0x57, 0xd9, 0x71, 0x4b, 0xc3};
        DEFINE_UUID(DI8Device)     = {0x79, 0x41, 0x74, 0x93, 0x65, 0x19, 0x42, 0x4f, 0xa1, 0x75, 0x89, 0x44, 0x3c, 0x6e, 0xeb, 0x82};
        DEFINE_UUID(DS8Device)     = {0x35, 0x67, 0xe0, 0x47, 0x13, 0x41, 0x44, 0x30, 0xa3, 0xb6, 0x12, 0xf9, 0x6e, 0x71, 0x8b, 0x9c};
        DEFINE_UUID(DIADevice)     = {0xa0, 0x4b, 0xa7, 0xfe, 0x46, 0x09, 0x45, 0x9e, 0x8c, 0xc4, 0xdb, 0x2a, 0x05, 0xa5, 0x92, 0x89};
        DEFINE_UUID(DIWDevice)     = {0x21, 0xd7, 0xdd, 0xc7, 0x17, 0xac, 0x45, 0xf7, 0x9e, 0xd3, 0x74, 0x0a, 0x9a, 0x86, 0x67, 0xb5};
    }
    namespace EventIds
    {
        DEFINE_UUID(Finalize)       = {0xc3, 0x15, 0xac, 0xc2, 0x76, 0x82, 0x3d, 0x4e, 0xb4, 0x43, 0x6e, 0xa6, 0xcc, 0x56, 0xb7, 0x31};
    }
    /**
     * @}
     */
    /**
     * Standard return type for Voodoo Shader functions, indicating various levels of success or failure.
     */
    typedef int32_t VoodooResult;
#define VSUCCESS(vr) (((VoodooResult)(vr)) >= 0)
#define VFAILURE(vr) (((VoodooResult)(vr)) <  0)
    /**
     * @defgroup voodoo_types_codes Return Codes
     * @{
     */
#define VSF_OK                  MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_ITF, 0x0000)
#define VSF_FAIL                MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0000)

#define VSFOK_PROPERTYCHANGED   MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_ITF, 0x1001)
#define VSFOK_REDUNDANT         MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_ITF, 0x1002)

#define VSFERR_INVALIDCALL      MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1001)
#define VSFERR_INVALIDPARAMS    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1002)
#define VSFERR_INVALIDXML       MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1003)
#define VSFERR_COMPILEFAILED    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1004)
#define VSFERR_INVALIDPROFILE   MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1005)
#define VSFERR_INVALIDPROPERTY  MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1006)
#define VSFERR_FILENOTFOUND     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1007)
#define VSFERR_NOTIMPLEMENTED   MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1008)
#define VSFERR_CONFLICTING      MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x1009)
#define VSFERR_INVALIDUUID      MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x100A)
#define VSFERR_PROPERTYNOTFOUND MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x100B)
#define VSFERR_APIERROR         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x100C)
    /**
     * @}
     * @defgroup voodoo_types_textures Special Texture Slots
     * @{
     */
#define VOODOO_TEXTURE_INVALID   uint32_t(-1)
#define VOODOO_TEXTURE_DEPTH     uint32_t(-2)
    /**
     * @}
     * @defgroup voodoo_types_vectors Vectors
     * @{
     */
    template <typename ValType>
    struct Vector1
    {
        ValType X;
    };

    template <typename ValType>
    struct Vector2
    {
        ValType X, Y;
    };

    template <typename ValType>
    struct Vector3
    {
        ValType X, Y, Z;
    };

    template <typename ValType>
    struct Vector4
    {
        ValType X, Y, Z, W;
    };

    template <typename ValType>
    struct Rectangle
    {
        Vector2<ValType> Min, Max;
    };

    template <typename ValType>
    struct Volume
    {
        Vector3<ValType> Min, Max;
    };

    typedef Vector1<int8_t>     Byte1;
    typedef Vector2<int8_t>     Byte2;
    typedef Vector3<int8_t>     Byte3;
    typedef Vector4<int8_t>     Byte4;
    typedef Vector1<uint8_t>    UByte1;
    typedef Vector2<uint8_t>    UByte2;
    typedef Vector3<uint8_t>    UByte3;
    typedef Vector4<uint8_t>    UByte4;
    typedef Vector1<int16_t>    Short1;
    typedef Vector2<int16_t>    Short2;
    typedef Vector3<int16_t>    Short3;
    typedef Vector4<int16_t>    Short4;
    typedef Vector1<uint16_t>   UShort1;
    typedef Vector2<uint16_t>   UShort2;
    typedef Vector3<uint16_t>   UShort3;
    typedef Vector4<uint16_t>   UShort4;
    typedef Vector1<int32_t>    Int1;
    typedef Vector2<int32_t>    Int2;
    typedef Vector3<int32_t>    Int3;
    typedef Vector4<int32_t>    Int4;
    typedef Vector1<uint32_t>   UInt1;
    typedef Vector2<uint32_t>   UInt2;
    typedef Vector3<uint32_t>   UInt3;
    typedef Vector4<uint32_t>   UInt4;
    typedef Vector1<float>      Float1;
    typedef Vector2<float>      Float2;
    typedef Vector3<float>      Float3;
    typedef Vector4<float>      Float4;
    typedef Vector1<double>     Double1;
    typedef Vector2<double>     Double2;
    typedef Vector3<double>     Double3;
    typedef Vector4<double>     Double4;
    typedef Rectangle<uint32_t> Rect;
    typedef Volume<uint32_t>    Box;
    /**
     * @}
     */
    struct Light;
    struct ParameterDesc;
    struct TextureDesc;
    struct TextureRegion;
    struct Variant;
    struct Version;
    struct Vertex;
    /**
     * @defgroup voodoo_interfaces Interfaces
     * @{
     */
    class IBinding;
    class ICore;
    class IEffect;
    class IFile;
    class IFileSystem;
    class IHookManager;
    class ILogger;
    class IPlugin;
    class IPluginServer;
    class IObject;
    class IParameter;
    class IParser;
    class IPass;
    class ITechnique;
    class ITexture;
    /**
     * @}
     * @defgroup voodoo_classes_utility Utility Classes
     * @{
     */
    class Exception;
    class StringFormat;
    class Regex;
    class RegexMatch;
    class String;
    /**
     * @}
     * @defgroup voodoo_functions Function Typedefs
     * @note Behavior and use of these functions is described in @ref voodoo_spec_plugin.
     * @{
     */
    namespace Functions
    {
        typedef ICore *         (VOODOO_CALLTYPE * CoreCreateFunc)(uint32_t);
        typedef const Version * (VOODOO_CALLTYPE * PluginInitFunc)(ICore *);
        typedef void            (VOODOO_CALLTYPE * PluginResetFunc)(ICore *);
        typedef const uint32_t  (VOODOO_CALLTYPE * ClassCountFunc)();
        typedef const wchar_t * (VOODOO_CALLTYPE * ClassInfoFunc)(const uint32_t, Uuid *);
        typedef IObject *       (VOODOO_CALLTYPE * ClassCreateFunc)(const uint32_t, ICore *);
        typedef VoodooResult    (VOODOO_CALLTYPE * CallbackFunc)(ICore *, uint32_t, Variant *);
    }
    /**
     * @}
     * @defgroup voodoo_references Reference Typedefs
     * @note To provide smart intrusive pointers, Boost is required.
     * @{
     */
#if !defined(VOODOO_NO_BOOST)
    VOODOO_FUNCTION(void, intrusive_ptr_add_ref)(IObject * obj);
    VOODOO_FUNCTION(void, intrusive_ptr_release)(IObject * obj);
    typedef boost::intrusive_ptr<IBinding>       BindingRef;
    typedef boost::intrusive_ptr<ICore>          CoreRef;
    typedef boost::intrusive_ptr<IEffect>        EffectRef;
    typedef boost::intrusive_ptr<IFile>          FileRef;
    typedef boost::intrusive_ptr<IFileSystem>    FileSystemRef;
    typedef boost::intrusive_ptr<IHookManager>   HookManagerRef;
    typedef boost::intrusive_ptr<ILogger>        LoggerRef;
    typedef boost::intrusive_ptr<IPlugin>        PluginRef;
    typedef boost::intrusive_ptr<IPluginServer>  PluginServerRef;
    typedef boost::intrusive_ptr<IObject>        ObjectRef;
    typedef boost::intrusive_ptr<IParameter>     ParameterRef;
    typedef boost::intrusive_ptr<IParser>        ParserRef;
    typedef boost::intrusive_ptr<IPass>          PassRef;
    typedef boost::intrusive_ptr<ITechnique>     TechniqueRef;
    typedef boost::intrusive_ptr<ITexture>       TextureRef;
    /**
     * @}
     * @defgroup voodoo_collections Collections
     * @note To provide collections of Voodoo Shader objects, smart pointers are required.
     * @{
     */
#if !defined(VOODOO_NO_STDLIB)
    typedef std::pair<String, String>           StringPair;
    typedef std::map<String, String>            StringMap;
    typedef std::list<String>                   StringList;
    typedef std::vector<String>                 StringVector;
    typedef std::map<String, EffectRef>         EffectMap;
    typedef std::list<EffectRef>                EffectList;
    typedef std::vector<EffectRef>              EffectVector;
    typedef std::map<String, TechniqueRef>      TechniqueMap;
    typedef std::list<TechniqueRef>             TechniqueList;
    typedef std::vector<TechniqueRef>           TechniqueVector;
    typedef std::map<String, PassRef>           PassMap;
    typedef std::list<PassRef>                  PassList;
    typedef std::vector<PassRef>                PassVector;
    typedef std::map<String, ParameterRef>      ParameterMap;
    typedef std::list<ParameterRef>             ParameterList;
    typedef std::vector<ParameterRef>           ParameterVector;
    typedef std::map<String, TextureRef>        TextureMap;
    typedef std::list<TextureRef>               TextureList;
    typedef std::vector<TextureRef>             TextureVector;
    typedef std::map<String, Variant>           VariantMap;
    typedef std::list<Variant>                  VariantList;
    typedef std::vector<Variant>                VariantVector;
    typedef std::map<Uuid, Variant>             PropertyMap;
    typedef std::pair<String, uint32_t>         Variable;
    typedef std::map<String, Variable>          VariableMap;
    typedef std::map<TextureRef, EffectRef>     MaterialMap;
    typedef std::map<String, Uuid>              StrongNameMap;
    typedef std::map<Uuid, PluginRef>           StrongPluginMap;
    typedef std::pair<PluginRef, uint32_t>      ClassSource;
    typedef std::map<Uuid, ClassSource>         ClassMap;
#endif
#endif
    /**
     * @}
     * @defgroup voodoo_structs Structures
     * @{
     */
    /**
     * Describes the precise version of a particular library, including name, main version, revision and debug status.
     */
    struct Version
    {
        const wchar_t * Name;
        const wchar_t * RevId;
        Uuid            LibId;
        int32_t         Major;
        int32_t         Minor;
        int32_t         Patch;
        int32_t         Build;
        bool            Debug;
    };
    /**
     * Property variant type. Consists of the value type (filled union field), components in the value (for vector
     * fields), and the value union capable of containing all common basic, vector and pointer types used in the
     * library.
     */
    struct Variant
    {
        UnionType   Type;
        uint32_t    Components;
        union
        {
            bool        VBool;
            Byte4       VInt8;
            UByte4      VUInt8;
            Short4      VInt16;
            UShort4     VUInt16;
            Int4        VInt32;
            UInt4       VUInt32;
            Float4      VFloat;
            Double4     VDouble;
            Uuid *      VPUuid;
            String *    VPString;
            IObject *   VPIObject;
            void *      VPVoid;
        };
    };
    /**
     * Describes a texture, including size and format.
     */
    struct TextureDesc
    {
        UInt3         Size;
        uint32_t      Levels;
        TextureFlags  Usage;
        TextureFormat Format;
    };
    /**
     * Describes a portion of a texture. This defines a cube region and provides an optional format the region should be
     * set up as.
     */
    struct TextureRegion :
        public TextureDesc
    {
        UInt3 Origin;
    };
    /**
     * Standard vertex structure.
     */
    struct VertexDesc
    {
        Float4  Position;
        UByte4  Color;
        Float4  TexCoord[2];
    };
    /**
     * Shader-compatible light structure
     */
    struct LightDesc
    {
        uint32_t Type;
        Float4   Diffuse;
        Float4   Specular;
        Float4   Ambient;
        Float3   Position;
        Float3   Direction;
        float    Range;
        float    Falloff;
        Float3   Attenuation;
        float    Theta;
        float    Phi;
    };
    /**
     * Parameter description.
     */
    struct ParameterDesc
    {
        ParameterType Type;
        uint32_t      Rows;
        uint32_t      Columns;
        uint32_t      Elements;
    };
    /**
     * @defgroup voodoo_variant_decl Variant Declaration & Init
     * @{
     */
#define DECLARE_AND_ZERO(type, name)                type name; ZeroMemory(&name, sizeof(type))
#define DECLARE_VARIANT(name)                       DECLARE_AND_ZERO(Variant, name)
#define INITIALIZE_VARIANTC(name, type, comp)       name.Type = VSUT_##type; name.Components = comp
#define INITIALIZE_VARIANT1(name, type, x)          INITIALIZE_VARIANTC(name, type, 1); name.V##type.X = x
#define INITIALIZE_VARIANT2(name, type, x, y)       INITIALIZE_VARIANTC(name, type, 2); name.V##type.X = x; name.V##type.Y = y
#define INITIALIZE_VARIANT3(name, type, x, y, z)    INITIALIZE_VARIANTC(name, type, 3); name.V##type.X = x; name.V##type.Y = y; name.V##type.Z = z
#define INITIALIZE_VARIANT4(name, type, x, y, z, w) INITIALIZE_VARIANTC(name, type, 4); name.V##type.X = x; name.V##type.Y = y; name.V##type.Z = z; name.V##type.W = w
    inline Variant CreateVariant()                      { DECLARE_VARIANT(var); return var; }
    inline Variant CreateVariant(const UnionType t)     { DECLARE_VARIANT(var); var.Type = t; return var; }
    inline Variant CreateVariant(const bool & v)        { DECLARE_VARIANT(var); INITIALIZE_VARIANTC(var, Bool, 0); var.VBool = v; return var; }
    inline Variant CreateVariant(const int8_t & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, Int8, v);          return var; }
    inline Variant CreateVariant(const Byte1 & v)       { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, Int8, v.X);        return var; }
    inline Variant CreateVariant(const Byte2 & v)       { DECLARE_VARIANT(var); INITIALIZE_VARIANT2(var, Int8, v.X, v.Y);   return var; }
    inline Variant CreateVariant(const Byte3 & v)       { DECLARE_VARIANT(var); INITIALIZE_VARIANT3(var, Int8, v.X, v.Y, v.Z); return var; }
    inline Variant CreateVariant(const Byte4 & v)       { DECLARE_VARIANT(var); INITIALIZE_VARIANT4(var, Int8, v.X, v.Y, v.Z, v.W); return var; }
    inline Variant CreateVariant(const uint8_t & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, UInt8, v);         return var; }
    inline Variant CreateVariant(const UByte1 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, UInt8, v.X);       return var; }
    inline Variant CreateVariant(const UByte2 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT2(var, UInt8, v.X, v.Y);  return var; }
    inline Variant CreateVariant(const UByte3 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT3(var, UInt8, v.X, v.Y, v.Z); return var; }
    inline Variant CreateVariant(const UByte4 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT4(var, UInt8, v.X, v.Y, v.Z, v.W); return var; }
    inline Variant CreateVariant(const int16_t & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, Int16, v);         return var; }
    inline Variant CreateVariant(const Short1 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, Int16, v.X);       return var; }
    inline Variant CreateVariant(const Short2 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT2(var, Int16, v.X, v.Y);  return var; }
    inline Variant CreateVariant(const Short3 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT3(var, Int16, v.X, v.Y, v.Z); return var; }
    inline Variant CreateVariant(const Short4 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT4(var, Int16, v.X, v.Y, v.Z, v.W); return var; }
    inline Variant CreateVariant(const uint16_t & v)    { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, UInt16, v);        return var; }
    inline Variant CreateVariant(const UShort1 & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, UInt16, v.X);      return var; }
    inline Variant CreateVariant(const UShort2 & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT2(var, UInt16, v.X, v.Y); return var; }
    inline Variant CreateVariant(const UShort3 & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT3(var, UInt16, v.X, v.Y, v.Z); return var; }
    inline Variant CreateVariant(const UShort4 & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT4(var, UInt16, v.X, v.Y, v.Z, v.W); return var; }
    inline Variant CreateVariant(const int32_t & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, Int32, v);         return var; }
    inline Variant CreateVariant(const Int1 & v)        { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, Int32, v.X);       return var; }
    inline Variant CreateVariant(const Int2 & v)        { DECLARE_VARIANT(var); INITIALIZE_VARIANT2(var, Int32, v.X, v.Y);  return var; }
    inline Variant CreateVariant(const Int3 & v)        { DECLARE_VARIANT(var); INITIALIZE_VARIANT3(var, Int32, v.X, v.Y, v.Z); return var; }
    inline Variant CreateVariant(const Int4 & v)        { DECLARE_VARIANT(var); INITIALIZE_VARIANT4(var, Int32, v.X, v.Y, v.Z, v.W); return var; }
    inline Variant CreateVariant(const uint32_t & v)    { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, UInt32, v);        return var; }
    inline Variant CreateVariant(const UInt1 & v)       { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, UInt32, v.X);      return var; }
    inline Variant CreateVariant(const UInt2 & v)       { DECLARE_VARIANT(var); INITIALIZE_VARIANT2(var, UInt32, v.X, v.Y); return var; }
    inline Variant CreateVariant(const UInt3 & v)       { DECLARE_VARIANT(var); INITIALIZE_VARIANT3(var, UInt32, v.X, v.Y, v.Z); return var; }
    inline Variant CreateVariant(const UInt4 & v)       { DECLARE_VARIANT(var); INITIALIZE_VARIANT4(var, UInt32, v.X, v.Y, v.Z, v.W); return var; }
    inline Variant CreateVariant(const float & v)       { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, Float, v);         return var; }
    inline Variant CreateVariant(const Float1 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, Float, v.X);       return var; }
    inline Variant CreateVariant(const Float2 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT2(var, Float, v.X, v.Y);  return var; }
    inline Variant CreateVariant(const Float3 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT3(var, Float, v.X, v.Y, v.Z); return var; }
    inline Variant CreateVariant(const Float4 & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT4(var, Float, v.X, v.Y, v.Z, v.W); return var; }
    inline Variant CreateVariant(const double & v)      { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, Double, v);        return var; }
    inline Variant CreateVariant(const Double1 & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT1(var, Double, v.X);      return var; }
    inline Variant CreateVariant(const Double2 & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT2(var, Double, v.X, v.Y); return var; }
    inline Variant CreateVariant(const Double3 & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT3(var, Double, v.X, v.Y, v.Z); return var; }
    inline Variant CreateVariant(const Double4 & v)     { DECLARE_VARIANT(var); INITIALIZE_VARIANT4(var, Double, v.X, v.Y, v.Z, v.W); return var; }
    inline Variant CreateVariant(Uuid * pV)             { DECLARE_VARIANT(var); INITIALIZE_VARIANTC(var, Uuid, 0); var.VPUuid = pV; }
    inline Variant CreateVariant(String * pV)           { DECLARE_VARIANT(var); INITIALIZE_VARIANTC(var, String, 0); var.VPString = pV; }
    inline Variant CreateVariant(IObject * pV)          { DECLARE_VARIANT(var); INITIALIZE_VARIANTC(var, IObject, 0); var.VPIObject = pV; }
    inline Variant CreateVariant(void * pV)             { DECLARE_VARIANT(var); INITIALIZE_VARIANTC(var, PVoid, 0); var.VPVoid = pV; }
    template<typename T>
    inline Variant CreateVariant(T * pV)                { DECLARE_VARIANT(var); INITIALIZE_VARIANTC(var, PVoid, 0); var.VPVoid = reinterpret_cast<void*>(pV); return var; }
    template<typename T>
    inline Variant CreateVariant(const T * pV)          { DECLARE_VARIANT(var); INITIALIZE_VARIANTC(var, PVoid, 0); var.VPVoid = reinterpret_cast<void*>(const_cast<T*>(pV)); return var; }
    /**
     * @}
     */
    /**
     * @}
     */
}
