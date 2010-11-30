
#ifndef VOODOO_CONVERTER_HPP
#define VOODOO_CONVERTER_HPP

#include "Meta.hpp"

namespace VoodooShader
{
	class Converter
	{
	public:
		static inline CGtype ToCGType(ParameterType pt)
		{
			switch ( pt )
			{
			case PT_Sampler1D:
				return CG_SAMPLER1D;
			case PT_Sampler2D:
				return CG_SAMPLER2D;
			case PT_Sampler3D:
				return CG_SAMPLER3D;
			case PT_Float1:
				return CG_FLOAT;
			case PT_Float2:
				return CG_FLOAT2;
			case PT_Float3:
				return CG_FLOAT3;
			case PT_Float4:
				return CG_FLOAT4;
			case PT_Unknown:
			default:
				return CG_UNKNOWN_TYPE;
			}
		}

		static inline ParameterType ToParameterType(CGtype pt)
		{
			switch ( pt )
			{
			case CG_SAMPLER1D:
				return PT_Sampler1D;
			case CG_SAMPLER2D:
				return PT_Sampler2D;
			case CG_SAMPLER3D:
				return PT_Sampler3D;
			case CG_FLOAT:
				return PT_Float1;
			case CG_FLOAT2:
				return PT_Float2;
			case CG_FLOAT3:
				return PT_Float3;
			case CG_FLOAT4:
				return PT_Float4;
			case CG_UNKNOWN_TYPE:
			default:
				return PT_Unknown;
			}
		}

		static inline const char * ToString(TextureFormat tf)
		{
			switch ( tf )
			{
			case TF_RGB5:
				return "TF_RGB5";
			case TF_RGB5A1:
				return "TF_RGB5A1";
			case TF_RGB8:
				return "TF_RGB8";
			case TF_RGBA8:
				return "TF_RGBA8";
			case TF_RGB10A2:
				return "TF_RGB10A2";
			case TF_RGBA16F:
				return "TF_RGBA16f";
			case TF_RGBA32F:
				return "TF_RGBA32f";
			case TF_D16:
				return "TF_D16";
			case TF_D32:
				return "TF_D32";
			case TF_Unknown:
			default:
				return "TF_Unknown";
			}
		}

		static inline const char * ToString(ParameterType pt)
		{
			switch ( pt )
			{
			case PT_Sampler1D:
				return "PT_Sampler1D";
			case PT_Sampler2D:
				return "PT_Sampler2D";
			case PT_Sampler3D:
				return "PT_Sampler3D";
			case PT_Float1:
				return "PT_Float1";
			case PT_Float2:
				return "PT_Float2";
			case PT_Float3:
				return "PT_Float3";
			case PT_Float4:
				return "PT_Float4";
			case PT_Unknown:
			default:
				return "PT_Unknown";
			}
		}
	};
}

#endif /*VOODOO_CONVERTER_HPP*/