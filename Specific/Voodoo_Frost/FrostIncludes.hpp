
#ifndef VOODOO_NWS_MODULE_HPP
#define VOODOO_NWS_MODULE_HPP

#define VOODOO_IMPORT
#include "../../Framework/VoodooFramework.hpp"
#pragma comment(lib, "../../Debug/VoodooFramework.lib")

#include "GL/gl.h"
#include "Cg/cgGL.h"
//#pragma comment(lib, "cg.lib")
#pragma comment(lib, "cgGL.lib")

// GLEW OpenGL extensions
#include <GL/glew.h>
#include <GL/wglew.h>
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "EasyHook.h"
#pragma comment(lib, "EasyHook32.lib")

// NWShader module

extern VoodooShader::Core * VoodooCore;

#endif /*VOODOO_NWS_MODULE_HPP*/
