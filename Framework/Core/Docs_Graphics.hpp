/**************************************************************************************************\
 * This file is part of the Voodoo Shader Framework, a comprehensive shader support library.
 * Copyright (c) 2010-2011 by Sean Sube
 *
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the 
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the 
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; 
 * if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301 US
 *
 * Support and more information may be found at http://www.voodooshader.com, or by contacting the
 * developer at peachykeen@voodooshader.com
\**************************************************************************************************/
  
/**
 * @page Shaders
 * <p>
 *    Voodoo shaders are split into a number of levels with a tree structure.
 * </p>
 * <p>
 *    Each shader may have one or more techniques. A technique describes a full
 *    rendering process, and may have one or more passes. Each pass describes a
 *    particular way to render the given geometry, and may have multiple programs.
 *    Each program controls a particular part of the render process. In addition,
 *    shaders, techniques and passes contain some metadata used by the system to
 *    control how and where they are rendered.
 * </p>
 *
 * @section programstages Program Stages
 * <p>
 *    Each program is designed to control a very specific stage of the render
 *    pipeline.
 * </p>
 * <p>
 *    The two most common stages, vertex and fragment, work with existing data and
 *    transform it. Both have been supported since DirectX 8 in some form, and
 *    most systems today support both. Given Voodoo's focus, it is safe to assume
 *    both stages are supported (the exact level of support may vary, however).
 * </p>
 * <p>
 *  The vertex stage handles each vertex, transforming them into view space and 
 *    sometimes calculating lighting or surface normals. 
 * </p>
 * <p>
 *    The fragment stage handles each pixel (or fragment) as it is rendered. The
 *    fragment program handles lighting in per-pixel lighting solutions, as well
 *    applying and blending texture stages, blending vertex colors, and any other
 *    operations that may vary for each pixel rendered.
 * </p>
 * <p> 
 *    The geometry stage has the opportunity to create vertexes if necessary and 
 *    can build geometry dynamically. This is often used with a form of render-to-
 *    vertex buffer, to generate and store the geometry for later use. Geometry
 *    stages will often depend heavily on parameters passed into the effect. 
 *    Geometry stages are supported on DirectX 10 or OpenGL 3.2 and better, usually. 
 * </p>
 * <p>
 *    Most recently, two tessellation control stages have been added. While these
 *    are supported in DirectX 11 and higher, and so are not wide spread, they
 *    provide detailed control over geometry and can manipulate or create it
 *    at runtime. They have some similarities to the geometry stage, but provide
 *    more control over the hardware tessellation process.
 * <p>
 * @note Voodoo does not yet support these shader stages, mostly because I
 *        don't have access to DX11 hardware. Geometry stages are supported, and
 *        should work, but aren't terribly useful (older games can't use them).
 *        The vertex and pixel stages are the most used, by far.
 *
 * @section shadersystem Shader System
 * <p>
 *    In addition to the standard Cg compiler, Voodoo adds a few additional
 *    features to the shader system.
 * </p>
 * @subsection shaderparameters Shader Parameters
 * <p>
 *    Voodoo provides a system to link effect-level parameters to application or
 *    system-level variables. This provides an easy way to provide data to shaders
 *    and is capable of linking many parameters from various effects to the same
 *    global parameter.
 * </p>
 * <p>
 *    Effect parameters may only be linked to global parameters. Global parameters
 *    are created by calling Core::CreateParameter(), then any number of other
 *    parameters may be linked by calling Parameter::Attach() on the global
 *    parameter and providing the effect parameter, like so:
 * </p>
 * <p>
 * @code
 *    ParameterRef global = Core->CreateParameter("time", PT_Float);
 *    ParameterRef effect = Shader->GetParameter("time");
 *    global->Attach(effect);
 * @endcode
 * </p>
 * <p>
 *    Whenever the global parameter is set to a new value, all attached parameters
 *    will also be updated.
 * </p>
 * <p>
 *    This mechanism is useful for things like system time, common matrices,
 *    random values or other data that changes each frame but not within the
 *    frame.
 * </p>
 * @subsection shadertargets Shader Targets
 * <p>
 *    Voodoo also provides a system to specify a target for each technique or
 *    pass. This provides an easy way to setup render to texture effects.
 * </p>
 * <p>
 *    Each shader technique and pass may have a target annotation, which gives
 *    the name of a texture. The results of the technique or pass, usually sent
 *    to the <code>lastshader</code> or <code>lastpass</code> texture, will
 *    instead go to the specified target. Targets may be specified like so:
 * </p>
 * <p>
 * @code
 * sampler2D diffuseSamp < texture=":buffer_diffuse"; >;
 * sampler2D specSamp < texture=":buffer_specular"; >;
 * 
 * technique dx9_high
 * {
 *     pass diffuse
 *        < string target=":buffer_diffuse"; >
 *     {
 *         ...
 *     }
 *     pass spec
 *        < string target=":buffer_specular"; >
 *     {
 *         ...
 *     }
 *     pass combine
 *     {
 *         return ( diffuseSamp + specSamp );
 *     }     
 * }
 * @endcode
 * </p>
 * <p>
 *    This allows pass and technique output to be stored and later used, by other
 *    passes from the same effect or even by other effects (chaining effects to
 *    use textures can be tricky, especially in how the effects are loaded, so be
 *    careful with that).
 * </p>
 * <p>
 *    If no target is specified in the technique or pass annotations, the shader
 *    linker uses the texture currently bound to the @ref VoodooShader::Core 
 *    "Core's" special textures. These may be set using 
 *    VoodooShader::Core::SetTexture(TextureType, TextureRef). Relinking a shader 
 *    may change these targets.
 * </p>
 *
 * @page Textures
 * <p>
 *    The shaders used in Voodoo are capable of accessing a number of texture
 *    types, and Voodoo will be able to load a number of texture formats. This
 *    provides a very powerful system and allows you to use a good variety of
 *    textures.
 * </p>
 * 
 * @section texturetypes Texture Types
 * <p>
 *    Three basic texture types are supported in Voodoo:
 *    <ul>
 *     <li>1-dimensional texture rows (a simple row of pixels, 1 by x)</li>
 *     <li>2-dimensional texture planes (the most common type, a rectangle of pixels,
 *        x by y)</li>
 *     <li>3-dimensional texture volumes (effectively a number of 2D textures stacked in
 *        slices, x by y by z)</li>
 *    </ul>
 * </p>
 * <p>
 *    A different sampler type is provided for each texture type (
 *    <code>sampler1D</code>, <code>sampler2D</code>, and <code>sampler3D</code>),
 *    each optimized to sample pixels from a particular texture type.
 * </p>
 * <p>
 *    Samplers require texture coordinates when used, and each type of texture
 *    requires one coordinate per dimension. Some texture sampling instructions
 *    can perform additional filtering or draw from specific mipmaps and may need
 *    more coordinates.
 * </p>
 *
 * @section textureformats Texture Formats
 * <p>
 *    Voodoo supports a decent sampling of internal texture formats. These formats
 *    specify how much data a texture holds and how it is stored. There are some
 *    limitations and some potentially useful formats are not yet supported, due
 *    to the differences between DirectX and OpenGL, but many common formats are
 *    available.
 * </p>
 * <p>
 *    All the listed formats are usable as a 2D texture, and with the exception
 *    of the depth formats, should be usable as 1D or 3D textures as well. For 
 *    most of these formats, DirectX and OpenGL equivalents are indicated below, 
 *    along with a notation indicating if the texture may be used as a 
 *    render target or copied to and from.
 * </p>
 * <p>
 *    Textures are always delivered to shaders as floats through sampling functions.
 *    This may cause some confusion as to how texture data is stored; the data is <em>not usually</em>
 *    stored as a float, but instead an integer using the number of bits given below. Textures 
 *    that are not stored in floating-point formats are typically clamped to range (0-1) in 
 *    hardware. Shaders may output higher or lower values, but they will usually be clamped.
 *    If you need to store values outside of that  range, use one of the formats suffixed 
 *    with 'F' to store the data as floats (single and double texture formats are available).
 * </p>
 * <table>
 *    <tr><th>Voodoo Format</th>        <th>DirectX Format</th>        
 *        <th>OpenGL Format</th>        <th>RT/Copy/Load<sup>4</sup></th>
 *        <th>Bits</th>                 <th>Channels</th></tr>
 *    <tr><td>TF_Unknown</td>           <td>D3DFMT_UNKNOWN</td>        
 *        <td>(unknown)</td>            <td>None</td></tr>
 *    <tr><td>TF_RGB5</td>              <td>D3DFMT_X1R5G5B5</td>    
 *        <td>GL_RGB5</td>              <td>Y/Y/N</td>
 *        <td>15/16</td>                <td>RGB</td></tr>
 *    <tr><td>TF_RGB5A1</td>            <td>D3DFMT_A1R5G5B5</td>    
 *        <td>GL_RGB5_A1</td>           <td>Y/Y/N</td>
 *        <td>16</td>                   <td>RGBA</td></tr>
 *    <tr><td>TF_RGB8</td>              <td>D3DFMT_X8R8G8B8</td>    
 *        <td>GL_RGB8</td>              <td>Yes</td>
 *        <td>24</td>                   <td>RGB</td></tr>
 *    <tr><td>TF_RGBA8</td>             <td>D3DFMT_A8R8G8B8</td>    
 *        <td>GL_RGBA8</td>             <td>Yes</td>
 *        <td>32</td>                   <td>RGBA</td></tr>
 *    <tr><td>TF_RGB10A2</td>           <td>D3DFMT_A2R10G10B10</td>    
 *        <td>GL_RGB10_A2</td>          <td>Y/Y/N</td>
 *        <td>32</td>                   <td>RGBA</td></tr>
 *    <tr><td>TF_RGBA16F<sup>1</sup></td><td>D3DFMT_A16B16G16R16F</td>
 *        <td>GL_RGBA16F</td>           <td>N/Y/Y</td>
 *        <td>64</td>                   <td>RGBA</td></tr>
 *    <tr><td>TF_RGBA32F<sup>1</sup></td><td>D3DFMT_A32B32G32R32F</td>
 *        <td>GL_RGBA32F</td>           <td>N/Y/Y</td>
 *        <td>128</td>                  <td>RGBA</td></tr>
 *    <tr><td>TF_D16F</td>              <td>D3DFMT_D16F</td>    
 *        <td>GL_DEPTH_COMPONENT16</td> <td>N/Y-ish<sup>2</sup>/N</td>
 *        <td>16</td>                   <td>R<sup>3</sup></td></tr>
 *    <tr><td>TF_D32F</td>              <td>D3DFMT_D32F</td>
 *        <td>GL_DEPTH_COMPONENT32</td> <td>N/Y-ish<sup>2</sup>/N</td>
 *        <td>32</td>                   <td>R<sup>3</sup></td></tr>
 * </table>
 *  <em><sup>1</sup>:</em>
 *     Floating-point texture formats (RGBA16F and RGBA32F) may not be supported 
 *     on all hardware.
 *     Any features requiring these formats should provide an alternate render 
 *     path for old hardware (quality may suffer).
 * <br />
 * <em><sup>2</sup>:</em>
 *     Depth textures suffer additional restrictions when used with the DirectX 9 
 *     API.
 *     See the @ref depthbuffers "depth buffers" section for more information.
 * <br />
 * <em><sup>3</sup>:</em>
 *     Depth textures do not map to components in all APIs. The most likely
 *     mapping is given, but access is not guaranteed.
 * <br />
 * <em><sup>4</sup>:</em>
 *     Texture loading is restricted by the filesystem library. The values
 *     given apply only to Voodoo/Filesystem, which uses DevIL. Only 8, 16 and
 *     32 bpc formats can be copied into by IImage::CopyImageData(); DevIL lacks 
 *     the values to describe packed formats and depth formats are not yet fully
 *     supported in the filesystem module. The supported formats are more widely 
 *     supported in other areas of Voodoo, so it is highly recommended you favor 
 *     these whenever possible. It <em>is</em>, however, possible to load images
 *     that are in other formats and convert them.
 *
 * @section depthbuffers Depth Buffers
 * <p>
 *    For an adapter to be considered fully Voodoo compatible, it must supply realtime
 *    depth data to a shader-readable texture. How this is performed is left to the 
 *    discretion of the adapter, as it varies greatly between old D3D versions and 
 *    OpenGL or newer D3D. This data should be stored such that greater distances are
 *    represented by higher values and the distance should be (if possible) not
 *    normalized (not bounded by <code>(0-1)</code>).
 * </p>
 * <p>
 *    When possible, it is recommended that the depth information be stored in a
 *    32-bit floating point component. Depth data must be accessible to shaders
 *    from the R channel of the depth texture. At this time, the GBA components of
 *    the texture are considered undefined, but reserved for future use. Depending
 *    on implementation, they may contain depth data, uninitialized memory, or
 *    other data. They should not be used.
 * </p>
 * <p>
 *    As DirectX 9 does not usually allow D32F textures to be read, this may 
 *    require rendering to a second target and using a RGBA32F texture. Two-pass
 *    or two-target rendering can both meet this requirement neatly. Using two passes
 *    is slower but more compatible, so this should be made an option if possible.
 * </p>
 * 
 * @section textureloading Texture Loading
 * <p>
 *     Texture loading is implemented through an Adapter callback, so Voodoo
 *     textures can be created with a variety of options or using any number
 *     of functions.
 * </p>
 * <p>
 *     To simplify getting texture data into memory in the first place, the Voodoo 
 *     filesystem library includes specialized Image loading classes. These use 
 *     DevIL as the loading library, so support a ridiculous number of formats. 
 *     Among the most useful formats are DDS, PNG and TGA, all supporting high 
 *     resolutions and transparency. Other formats may not be as suitable; JPEG and 
 *     BMP for example are very lossy and large, respectively. 
 * </p>
 * 
 * @section texturefileformats Supported File Formats
 * <p>
 *    Below is provided a full list of the file formats Voodoo/Filesystem is
 *    capable of loading and saving (via DevIL). This list is taken from the
 *    DevIL documentation.
 * </p>
 * <table>
 * <tr><th>Format</th><th>Extension</th><th>Load Support</th><th>Save Support</th></tr>
 * 
 * <tr><td>Windows Bitmap</td><td>bmp</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Dr. Halo</td><td>cut</td><td>Yes</td></tr>
 * <tr><td>Multi-PCX</td><td>dcx</td><td>Yes</td></tr>
 * <tr><td>Dicom</td><td>dcm</td><td>Yes</td></tr>
 * <tr><td>DirectDraw Surface<sup>3</sup></td><td>dds</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>OpenEXR</td><td>ext</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Flexible Image Transport System</td><td>fits, fit</td><td>Yes</td></tr>
 * <tr><td>Heavy Metal: FAKK 2</td><td>ftx</td><td>Yes</td>
 * <tr><td>C-style Header</td><td>h</td><td>Yes</td></tr>
 * <tr><td>Radiance High Dynamic</td><td>hdr</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Macintosh Icon</td><td>icns</td><td>Yes</td></tr>
 * <tr><td>Windows icon/cursor</td><td>ico, cur</td><td>Yes<sup>1</sup></td></tr>
 * <tr><td>Interchange FileFormat</td><td>iff</td><td>Yes</td></tr>
 * <tr><td>Infinity Ward Image</td><td>iwi</td><td>Yes</td></tr>
 * <tr><td>Graphics Interchange Format</td><td>gif</td><td>Yes<sup>1</sup></td></tr>
 * <tr><td>JPEG</td><td>jpg, jpe, jpeg</td><td>Yes<sup>2</sup></td><td>Yes</td></tr>
 * <tr><td>JPEG 2000</td><td>jp2</td><td>Yes<sup>2</sup></td><td>Yes</td></tr>
 * <tr><td>Interlaced Bitmap</td><td>lbm</td><td>Yes</td></tr>
 * <tr><td>Homeworld Texture</td><td>lif</td><td>Yes</td></tr>
 * <tr><td>Half-Life Model</td><td>mdl</td><td>Yes</td></tr>
 * <tr><td>MPEG-1 Audio Layer 3</td><td>mp3</td><td>Yes</td></tr>
 * <tr><td>Palette</td><td>pal</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Kodak PhotoCD</td><td>pcd</td><td>Yes</td></tr>
 * <tr><td>ZSoft PCX</td><td>pcx</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Softimage PIC</td><td>pic</td><td>Yes</td></tr>
 * <tr><td>Portable Network Graphics<sup>3</sup></td><td>png</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Portable Anymap</td><td>pbm, pgm, pnm</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Alias | Wavefront</td><td>pix</td><td>Yes</td></tr>
 * <tr><td>Adobe Photoshop</td><td>psd</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Paintshop Pro</td><td>psp</td><td>Yes</td></tr>
 * <tr><td>Pixar</td><td>pxr</td><td>Yes</td></tr>
 * <tr><td>Raw Data</td><td>raw</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Homeworld 2 Texture</td><td>rot</td><td>Yes</td></tr>
 * <tr><td>Silicon Graphics</td><td>sgi, bw, rgb, rgba</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Creative Assembly Texture</td><td>texture</td><td>Yes</td></tr>
 * <tr><td>Truevision Targa</td><td>tga</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Tagged Image Format</td><td>tif</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>Gamecube Texture</td><td>tpl</td><td>Yes</td></tr>
 * <tr><td>Unreal Texture</td><td>utx</td><td>Yes</td></tr>
 * <tr><td>Quake 2 Texture</td><td>wal</td><td>Yes</td></tr>
 * <tr><td>Valve Texture Format</td><td>vtf</td><td>Yes</td><td>Yes</td></tr>
 * <tr><td>HD Photo</td><td>wdp, hdp</td><td>Yes</td></tr>
 * <tr><td>X Pixel Map</td><td>xpm</td><td>Yes</td></tr>
 * <tr><td>Doom Graphics</td><td></td><td>Yes</td></tr>
 * </table>
 * <em><sup>1</sup>:</em> These formats support animated textures, but animations are not
 * supported within Voodoo. Only the first frame will be used.<br />
 * <em><sup>2</sup>:</em> These formats are not recommended for use, due to poor quality or
 * capabilities.<br />
 * <em><sup>3</sup>:</em> These formats are recommended for use and support most or all of
 * the features Voodoo uses.<br />
 * <p>
 *    Support for these formats is provided directly by DevIL, including the ability to convert
 *    most internal formats. Questions regarding formats and support, as they apply to DevIL,
 *    should be directed to the developer of this fine library. Questions regarding Voodoo's
 *    support or use of DevIL, or that of any Voodoo module, should be directed to Voodoo or
 *    module developer(s).
 * </p>
 * <p>
 *    Some of these formats (GIF, for example) do not correspond to any internal Voodoo formats.
 *    To use images in these formats, you should load the image, then call IImage::CopyImageData()
 *    and provide the desired format. DevIL will handle conversion internally. This may be slow
 *    and so should not be used often, but provides a way to load almost any format into Voodoo.
 * </p>
 * <p>
 *    The above list of formats is quite large, and some formats are obscure or not well suited
 *    for use with Voodoo. However, DevIL supports them and there is no reason to add artificial
 *    limitations to Voodoo when it takes less work to provide a more flexible structure. Thus,
 *    these formats are all supported and may be used as desired. Some of the more unusual
 *    formats may have some innate restrictions or lose data during conversion to Voodoo-
 *    supported formats, so take care to properly test any image resources you use.
 * </p>
 */
