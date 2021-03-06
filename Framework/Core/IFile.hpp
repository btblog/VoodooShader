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

#include "VoodooFramework.hpp"

namespace VoodooShader
{
    /**
     * @addtogroup voodoo_interfaces
     * @{
     */

    /**
     * @class IFile
     *
     * Provides abstraction to a single file, for basic access.
     *
     * @iid e6f3128a-05af-11e1-9e05-005056c00008
     */
    VOODOO_INTERFACE(IFile, IObject, ({0x8a, 0x12, 0xF3, 0xE6, 0xAF, 0x05, 0xE1, 0x11, 0x9E, 0x05, 0x00, 0x50, 0x56, 0xC0, 0x00, 0x08}))
    {
    public:
        /**
         * @name IObject Methods
         * @{
         */
        VOODOO_METHOD_(uint32_t, AddRef)() CONST PURE;
        VOODOO_METHOD_(uint32_t, Release)() CONST PURE;
        VOODOO_METHOD(QueryInterface)(_In_ CONST Uuid refid, _Outptr_result_maybenull_ IObject ** ppOut) PURE;
        VOODOO_METHOD_(String, ToString)() CONST PURE;
        VOODOO_METHOD_(ICore *, GetCore)() CONST PURE;
        /**
         * @}
         * @name Open/Close Methods
         * Open or close the underlying file/stream, validate as an image or get the path to the stream.
         * @{
         */
        /**
         * Gets the absolute path of this file.
         */
        VOODOO_METHOD_(String, GetPath)() CONST PURE;
        /**
         * Open the file with the given access mode.
         *
         * @param mode Desired access mode.
         * @return Success of file open.
         */
        VOODOO_METHOD(Open)(_In_ CONST FileOpenMode mode) PURE;
        /**
         * Close the file if it is open.
         *
         * @return Success of file close.
         */
        VOODOO_METHOD(Close)() PURE;
        /**
         * @}
         * @name Access Methods
         * Manipulate the underlying file or stream.
         * @{
         */
        /**
         * Seek to the given position in the file.
         *
         * @param stream The stream to move.
         * @param mode The seek mode to use.
         * @param offset The offset to seek to.
         * @return Success of the seek operation.
         */
        VOODOO_METHOD(Seek)(_In_ CONST StreamType stream, _In_ CONST SeekMode mode, _In_ CONST int32_t offset) PURE;
        /**
         * Get the position of the given stream.
         *
         * @param stream The stream to check.
         * @return The current position, or -1 on error.
         */
        VOODOO_METHOD_(int32_t, Tell)(_In_ CONST StreamType stream) PURE;
        /**
         * Read the given number of bytes from the file, starting at the current position. The file must have been opened
         * for reading previously or this call will fail.
         *
         * @param size The number of bytes to read.
         * @param pBuffer A buffer of size bytes, allocated by the caller (will be filled with data).
         * @return The number of bytes read, or -1 on error.
         *
         * @note If @arg buffer is nullptr, the number of bytes that would have been read is returned but the file
         *      position is unchanged. If @arg count is -1, this returns the size to the end of the file.
         */
        VOODOO_METHOD_(int32_t, Read)(_In_ CONST int32_t size, _Out_writes_opt_(size) void * pBuffer) PURE;
        /**
         * Write the given number of bytes to the file, starting at the current position.
         *
         * @param size The number of bytes to write.
         * @param pBuffer A buffer containing the data to write.
         * @return The number of bytes written, or -1 on error.
         *
         * @note If @p buffer is nullptr, @p count zeros are written into the file. This is useful for padding binary
         *      formats.
         */
        VOODOO_METHOD_(int32_t, Write)(_In_ CONST int32_t size, _In_reads_opt_(size) void * pBuffer) PURE;
        /**
         * @}
         */
    };
    /**
     * @}
     */
}
