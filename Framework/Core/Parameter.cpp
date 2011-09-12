#include "Parameter.hpp"

#include "Converter.hpp"
#include "Core.hpp"
#include "Exception.hpp"
#include "ILogger.hpp"
#include "Shader.hpp"

namespace VoodooShader
{

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 Parameter::Parameter(_In_ Core *core, _In_ String name, _In_ ParameterType type) :
  m_Type(type),
  m_Parent(nullptr),
  m_Virtual(true),
  m_Core(core),
  m_Name(name)
 {
  m_Core->GetLogger()->Log
   (
    LL_Debug,
    VOODOO_CORE_NAME,
    "Creating a virtual parameter (%s, core %p) of type %s.",
    name.c_str(),
    core,
    Converter::ToString(type)
   );

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  CGcontext context = m_Core->GetCgContext();
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  if (!context || !cgIsContext(context))
  {
   throw std::exception("Unable to create parameter (core has no context).");
  }

  m_Param = cgCreateParameter(context, Converter::ToCGType(m_Type));

  memset(m_ValueFloat, 0, sizeof(float) * 16);
  m_ValueTexture = TextureRef();
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 Parameter::Parameter(_In_ Shader *parent, _In_ CGparameter param) :
  m_Parent(parent),
  m_Param(param),
  m_Virtual(false),
  m_Core(parent->GetCore())
 {
  m_Type = Converter::ToParameterType(cgGetParameterType(param));
  m_Name = cgGetParameterName(param);

  memset(m_ValueFloat, 0, sizeof(float) * 16);
  m_ValueTexture = TextureRef();
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 Parameter::~Parameter(void)
 {
  m_Core->GetLogger()->Log(LL_Debug, VOODOO_CORE_NAME, "Destroying parameter %s.", m_Name.c_str());

  if (m_Virtual && cgIsParameter(m_Param))
  {
   cgDestroyParameter(m_Param);
  }
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 String Parameter::GetName(void)
 {

  /*~~~~~~~~~*/
  /*~~~~~~~~~*/
  String name;
  /*~~~~~~~~~*/

  /*~~~~~~~~~*/
  if (m_Parent)
  {
   name = this->m_Parent->GetName();
  }

  name += ":" + m_Name;

  return name;
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 Core *Parameter::GetCore(void)
 {
  return m_Core;
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 ParameterType Parameter::GetType(void)
 {
  return m_Type;
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 bool Parameter::IsVirtual(void)
 {
  return m_Virtual;
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 void Parameter::AttachParameter(ParameterRef param)
 {
  if (!this->m_Virtual)
  {
   Throw(VOODOO_CORE_NAME, "Cannot attach to a non-virtual parameter.", m_Core);
  }

  cgConnectParameter(this->m_Param, param->GetCgParameter());
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 int Parameter::GetComponents(void)
 {
  return Converter::ToComponents(m_Type);
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 TextureRef Parameter::GetTexture(void)
 {
  return m_ValueTexture;
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 void Parameter::SetTexture(TextureRef Texture)
 {
  m_ValueTexture = Texture;
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 float *Parameter::GetScalar(void)
 {
  return m_ValueFloat;
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 void Parameter::SetScalar(int Count, _In_count_(Count) float *Values)
 {
  if (Values)
  {
   memcpy(m_ValueFloat, Values, min(16, Count) * sizeof(float));
  }
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 ShaderRef Parameter::GetShader(void)
 {
  return m_Parent;
 }

 /**
  ===================================================================================================================
  *
  ===================================================================================================================
  */
 CGparameter Parameter::GetCgParameter(void)
 {
  return this->m_Param;
 }
}
