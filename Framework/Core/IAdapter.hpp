/**************************************************************************************************\
 * This file is part of the Voodoo Shader Framework, a comprehensive shader support library.
 * Copyright (c) 2010-2011 by Sean Sube
 *
 *
 * While the Voodoo Shader Framework, as a whole, is licensed under the GNU General Public license,
 * adapters designed to work with the framework do not constitute derivative works. In the case an
 * adapter needs to use this file (Adapter.hpp) or Meta.hpp in order to work with the Framework, and
 * is not licensed under the GNU GPL or a compatible license, please contact the developer to figure
 * out alternate licensing arrangements.
 *
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the 
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the 
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; 
 * if  not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301 US
 *
 * Support and more information may be found at http://www.voodooshader.com, or by contacting the
 * developer at peachykeen@voodooshader.com
\**************************************************************************************************/

#ifndef VOODOO_ADAPTER_HPP
#define VOODOO_ADAPTER_HPP

#include "Meta.hpp"
#include "IObject.hpp"

namespace VoodooShader
{
    /**
     * Generic vertex format for use with IAdapter draw calls. This format is
     * compatible with both OpenGL and DirectX.
     *
     * @note Because of the draw mechanism for adapters, most draws with
     *        user-provided vertexes will not use vertex buffers of any sort. 
     *        This can hurt performance if used heavily, so drawing through 
     *        IAdapter::DrawQuad() should be avoided as much as possible.
     *        Adapters may draw internally, of course, having full control over
     *        the graphics API.
     *
     * @note This vertex format provides a float3 position and float2 texture
     *        coordinate. For compatibility with Direct3D, a RHW value is also
     *        included (the vertex format is D3DFVF_XYZRHW|D3DFVF_TEX1). OpenGL
     *        adapters may ignore this winding value. The members of the vert are
     *        ordered so that <code>&x</code> is a valid float3 with the position
     *        and <code>&tu</code> is a valid float2 with the texture coordinate.
     */
    struct Vertex
    {
        float x, y, z;
        float winding;
        float tu, tv;
    };

    /**
     * Graphics adapter class, responsible for interfacing the Voodoo core with
     * a given graphics program. 
     *
     * @note This class should be implemented for a specific graphics application; 
     *       some generic adapters are provided for major APIs, but in most cases 
     *       application-specific behavior will make these only partially helpful. 
     * @sa See the @ref adapterspec "adapter specifications" for details on what
     *     is required of adapter modules and classes.
     */
    class IAdapter
        : public IObject
    {
    public:
        virtual ~IAdapter() throw()
        { }

        /**
         * Loads a pass in a manner that is compatible with the adapter's
         * underlying API. For Cg-supported APIs, this may use the appropriate 
         * module loading function (cgD3D9LoadProgram, cgGLLoadProgram, etc).
         * This may involve additional compilation or linking. In some cases,
         * the pass may not be specifically loaded and this may always return
         * true. 
         * 
         * @note If this returns false, do not bind the pass given. It may
         *       cause errors or fail.
         * 
         * @warning There is no guarantee what programs, if any, will be
         *      loaded or how this will be performed. The only guarantee is
         *      that if this call returns true, the pass may be bound to this
         *      adapter in the future.
         *
         * @param pass The pass to be loaded.
         * @return Whether or not the pass was successfully loaded.
         */
        virtual bool LoadPass
        (
            _In_ Pass * pass
        ) = 0;

        /**
         * Unloads a pass from the adapter.
         * 
         * @warning There is no guarantee what will occur within this function,
         *      but the pass given should <em>never</em> be bound after being
         *      unloaded.
         * 
         * @param pass The pass to unload.
         * 
         * @since 0.2.4.145
         */
        virtual bool UnloadPass
        (
            _In_ Pass * pass
        ) = 0;

        /**
         * Causes the adapter to load a pass for drawing. The adapter may choose 
         * which programs to bind and how to handle the binding, as well as 
         * managing any errors that may arise.
         *
         * @param pass A shared pointer to the pass to be bound. This should be a
         *        valid pass, but adapters must validate it.
         *
         * @note Adapters may, at their discretion, bind some or even no programs
         *       to the true graphics API. An example of this is the Direct3D 9 
         *       adapter, which binds only vertex and fragment programs, ignoring
         *       any geometry, domain or hull programs.
         * @note The Adapter may implement passes using deferred parameters; if so
         *       it <em>must</em> update all pass parameters, virtual first and actual
         *       parameters second (to make sure virtual param values get propagated).
         *       All parameters updates must be performed before this function returns
         *       and take effect for any draw calls coming after this call.
         */
        virtual void BindPass
        (
            _In_ PassRef pass
        ) = 0;

        /**
         * Unbinds the last bound pass. This resets the shader state of the underlying graphics 
         * API to the same state as before the pass was bound. 
         *
         * @note Adapters must restore all states modified by an IAdapter::BindPass() call. 
         *       Caching the changes is a simple method, but the exact implementation is left to 
         *       the adapter.
         *
         * @warning If IAdapter::BindPass() is called, render states are likely to change. These are 
         *       unlikely to be reset until IAdapter::UnbindPass() is called and may affect other
         *       draw calls. This behavior may be desired, but should be noted.
         */
        virtual void UnbindPass() = 0;

        /**
         * Draws a quad to the screen.
         *
         * @param vertexData This must contain vertex data for 4 vertexes or be NULL.
         *
         * @note The provided vertexes must be ordered in a Z formation. This provides the
         *       proper winding for two discrete triangles, which most APIs use for the actual
         *       draw operation.
         *        
         * @note The draw operation must meet the following requirements:
         * <ol>
         *   <li>Depth and stencil buffers must not be written to, color buffers must.</li>
         *   <li>Alpha testing, culling, depth testing and other states that could cull
         *       portions of the quad must be disabled (the whole quad must be drawn).</li>
         *   <li>Alpha blending and testing must be disabled.</li>
         *   <li>Any render states modified <em>within this call</em> must be restored to their 
         *       original state before the function returns.</li>
         * </ol>
         */
        virtual void DrawQuad
        (
            _In_opt_count_c_(4) Vertex * vertexData
        ) = 0;

        /**
         * Downloads a parameter's value from system RAM to VRAM, verifying that the value on 
         * the GPU (and in use) is the latest value set in the CPU memory buffer (all parameter 
         * set commands operate on the system RAM buffer for speed and compatibility). 
         */
        virtual void ApplyParameter
        (
            _In_ ParameterRef param
        ) = 0;

        /**
         * <p>
         * Helper function to handle binding and drawing a single effect. This must take targets 
         * into consideration, and should contain <em>all</em> code necessary to render a basic 
         * post-processing or deferred effect.
         * </p><p>
         * In some adapter setups and APIs, this may involve blitting textures or handling various 
         * surfaces and RTT features. These should be implemented here to provide a clean interface 
         * for basic shader drawing.
         * </p><p>
         * This function should assume that a full screen effect is being drawn and use the default 
         * vertex setup (by calling <code>this->DrawQuad(NULL);</code>).
         */
        virtual void DrawShader
        (
            _In_ ShaderRef shader
        ) = 0;

        /**
         * Creates a named texture within the API and registers it with the Core. The various 
         * texture parameters are specified here in an API-independent form.
         *
         * @param name The name of the texture, usually a fully-qualified name.
         * @param desc Description of the texture, size and format.
         * @return A shared pointer to the created texture.
         *
         * @note Only Voodoo texture formats are supported, API-specific formats
         *       are not and <em>must</em> return errors.
         * @note This must automatically register textures with the adapter's 
         *       associated Core.
         * @note This must create a texture that can either be rendered directly
         *       into or that can have the backbuffer copied into it efficiently, 
         *       depending on how the Adapter and API implement render-to-texture.
         */
        virtual TextureRef CreateTexture
        (
            _In_ String name, 
            _In_ TextureDesc desc
        ) = 0;

        /**
         * Loads a named texture into the API and registers it with the Core. The texture source is
         * provided here and all dimensions should be drawn from that, with as little conversion as
         * possible.
         * 
         * @param image The image reference to load from.
         */
        virtual TextureRef LoadTexture
        (
            _In_ IImageRef image
        ) = 0;

        /**
         * Connects a texture to a sampler-type parameter. This is performed 
         * differently in each API, but often uses Cg-provided functions (in 
         * OpenGL, cgGLSetTextureParameter). The parameter and texture should 
         * be connected for the duration of the shader's life or until the parameter
         * is bound to a different texture.
         * 
         * @param param The parameter to bind to (must be a sampler type).
         * @param texture The texture to be bound.
         * @return Whether or not the binding was successful.
         */
        virtual bool ConnectTexture
        (
            _In_ ParameterRef param, 
            _In_ TextureRef texture
        ) = 0;

        /**
         * A generic error-handling callback provided to the Cg runtime. This 
         * will be called by Core::CgErrorHandler() in the event of a error. This allows
         * the adapter to internally handle any cleanup or other state changes that
         * become necessary.
         * 
         * @note Not all Cg errors will call this; the core performs error validation
         *       and handles all context-related errors internally, as well as logging
         *       what data it can find. Malformed errors will not be passed to the
         *       adapter.
         *       
         * @note In the case of compiler errors, the exact messages can be gotten by
         *       calling cgGetLastListing until it returns null. The Core error handler
         *       does just this, <em>after</em> the adapter's error handler returns.
         *       Thus, if the adapter does not dump compiler listings, the core will.
         *
         * @param context The associated Cg context.
         * @param error The error raised.
         * @param core The currently active Voodoo Core.
         */
        virtual void HandleError
        (
            _In_ CGcontext context, 
            _In_ CGerror error, 
            _In_ void * core
        ) = 0;
    };
}

#endif
