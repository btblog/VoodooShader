
#ifndef VOODOO_DX9_MODULE_HPP
#define VOODOO_DX9_MODULE_HPP

#define VOODOO_IMPORT
#include "VoodooFramework.hpp"

#include "D3D9.h"
#include "Cg/cgD3D9.h"
#pragma comment(lib, "cgD3D9.lib")

#ifndef VOODOO_IMPORT_DX9
#	define VOODOO_API_DX9 __declspec(dllexport)
#else
#	define VOODOO_API_DX9 __declspec(dllimport)
#endif

#include "DX9_Adapter.hpp"

class IVoodoo3D9;
class IVoodoo3DDevice9;

extern VoodooShader::Core * VoodooCore;
extern VoodooShader::DirectX9::Adapter * VoodooDX9;
extern VoodooShader::ShaderRef testShader;

extern IVoodoo3D9 * VoodooObject;
extern IVoodoo3DDevice9 * VoodooDevice;

IDirect3DSurface9 * backbufferSurf = NULL;

IDirect3DTexture9 * texture_Scratch = NULL;
IDirect3DSurface9 * surface_Scratch = NULL;

VoodooShader::TextureRef texture_ThisFrame();
IDirect3DSurface9 * surface_ThisFrame = NULL;

VoodooShader::TextureRef texture_LastPass();
IDirect3DSurface9 * surface_LastPass = NULL;

VoodooShader::TextureRef texture_LastShader();
IDirect3DSurface9 * surface_LastShader = NULL;

typedef IDirect3D9 * (__stdcall *D3DFunc9)(UINT);


#endif /*VOODOO_DX9_MODULE_HPP*/
