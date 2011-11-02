

#include "DX89_Adapter.hpp"
#include "DX89_Converter.hpp"
#include "DX89_Version.hpp"

namespace VoodooShader
{
 namespace DirectX89
{
 LPDIRECT3DVERTEXBUFFER9 FSQuadVerts = nullptr;

 /**
  *
  */
 Adapter::Adapter(ICore *core, IDirect3DDevice9 *device) :
  mCore(core),
  mDevice(device)
 {
  assert(device);

  if (!core)
  {
   core = VoodooShader::ICore::Create();
  }

  core->Log(LL_Info, VOODOO_DX89_NAME, "Starting adapter...");
  core->LogModule(this->GetVersion());

  try
  {
   core->SetAdapter(reinterpret_cast < VoodooShader::Adapter * > (this));
   core->Log(LL_Info, VOODOO_DX89_NAME, "ICore adapter set to this.");
  }
  catch(std::exception & exc)
  {
   core->Log(LL_Error, VOODOO_DX89_NAME, "Error setting adapter on core: %s.", exc.what());
  }

  HRESULT hr = cgD3D9SetDevice(device);

  if (!SUCCEEDED(hr))
  {
   Throw(VOODOO_DX89_NAME, "Could not set Cg device.", core);
  }
  else
  {
   core->Log(LL_Debug, VOODOO_DX89_NAME, "Set Cg device to %p.", device);
  }

  // ifdef _DEBUG
  cgD3D9EnableDebugTracing(CG_TRUE);

  // endif
  cgD3D9SetManageTextureParameters(core->GetCGContext(), CG_TRUE);

  cgD3D9RegisterStates(core->GetCGContext());

  HRESULT errors = cgD3D9GetLastError();

  if (!SUCCEEDED(errors))
  {
   core->Log(LL_Errors, VOODOO_DX89_NAME, "Errors setting Cg states: %s", cgD3D9TranslateHRESULT(errors));
  }
  else
  {
   core->Log(LL_Debug, VOODOO_DX89_NAME, "Cg states set successfully.");
  }


  // Setup profiles
  CGprofile bestFrag = cgD3D9GetLatestPixelProfile();
  CGprofile bestVert = cgD3D9GetLatestVertexProfile();

  core->Log
   (
    LL_Info,
    VOODOO_DX89_NAME,
    "Detected the following profiles: %s (vertex); %s (fragment)",
    cgGetProfileString(bestVert),
    cgGetProfileString(bestFrag)
   );


  // Get params
  D3DVIEWPORT9 viewport;

  device->GetViewport(&viewport);

  float fx = ((float) viewport.Width / 2) + 0.5f; // 2;
                 ///
  float fy = ((float) viewport.Height / 2) + 0.5f; // 2;
                 ///

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  mCore->Log(LL_Info, VOODOO_DX89_NAME, "Prepping for %d by %d target.", fx, fy);

  hr = this->mDevice->CreateVertexBuffer
   (
    6 * sizeof(FSVert),
    0,
    D3DFVF_CUSTOMVERTEX,
    D3DPOOL_DEFAULT,
    &FSQuadVerts,
    nullptr
   );

  if (FAILED(hr))
  {
   mCore->Log(LL_Error, VOODOO_DX89_NAME, "Failed to create vertex buffer for fullscreen quad.");
  }

  FSVert g_Vertices[4];

  memset(g_Vertices, 0, sizeof(FSVert) * 4);

  g_Vertices[0].x = -0.5f;
  g_Vertices[0].y = -0.5f;
  g_Vertices[0].z = 0.5f;
  g_Vertices[1].x = fx;
  g_Vertices[1].y = -0.5f;
  g_Vertices[1].z = 0.5f;
  g_Vertices[2].x = -0.5f;
  g_Vertices[2].y = fy;
  g_Vertices[2].z = 0.5f;
  g_Vertices[3].x = fx;
  g_Vertices[3].y = fy;
  g_Vertices[3].z = 0.5f;

  g_Vertices[0].rhw = g_Vertices[1].rhw = g_Vertices[2].rhw = g_Vertices[3].rhw = 1.0f;

  g_Vertices[0].tu = 0.0f;
  g_Vertices[0].tv = 0.0f;
  g_Vertices[1].tu = 1.0f;
  g_Vertices[1].tv = 0.0f;
  g_Vertices[2].tu = 0.0f;
  g_Vertices[2].tv = 1.0f;
  g_Vertices[3].tu = 1.0f;
  g_Vertices[3].tv = 1.0f;

  void *pVertices;

  FSQuadVerts->Lock(0, sizeof(FSVert) * 4, &pVertices, 0);

  memcpy(pVertices, g_Vertices, sizeof(FSVert) * 4);

  FSQuadVerts->Unlock();
 }

 /**
  *
  */
 Version Adapter::GetVersion(void)
 {

  Version version = { VOODOO_META_VERSION_CHAIN(DX89) };

  return version;
 }

 /**
  *
  */
 bool Adapter::LoadPass(IPass *pass)
 {
  assert(pass);

  CGprogram vertProg = pass->GetProgram(PS_Vertex);
  CGprogram fragProg = pass->GetProgram(PS_Fragment);
  HRESULT hr = S_OK;

  if (cgIsProgram(vertProg))
  {
   hr = cgD3D9LoadProgram(vertProg, CG_TRUE, 0);
   if (!SUCCEEDED(hr))
   {
    this->mCore->Log
     (
      LL_Error,
      VOODOO_DX89_NAME,
      "Error loading vertex program from '%s': %s",
      pass->ToString().GetData(),
      cgD3D9TranslateHRESULT(hr)
     );

    return false;
   }
  }

  if (cgIsProgram(fragProg))
  {
   hr = cgD3D9LoadProgram(fragProg, CG_TRUE, 0);
   if (!SUCCEEDED(hr))
   {
    this->mCore->Log
     (
      LL_Error,
      VOODOO_DX89_NAME,
      "Error loading fragment program from '%s': %s",
      pass->ToString().GetData(),
      cgD3D9TranslateHRESULT(hr)
     );

    return false;
   }
  }

  this->mCore->Log(LL_Info, VOODOO_DX89_NAME, "Successfully loaded programs from '%s'.", pass->ToString().GetData());

  return true;
 }

 /**
  *
  */
 void Adapter::BindPass(IPass* pass)
 {


  // Both should be loaded and valid (if they exist and prepare was called)
  CGprogram vertProg = pass->GetProgram(PS_Vertex);
  CGprogram fragProg = pass->GetProgram(PS_Fragment);

  if (cgIsProgram(vertProg))
  {

   HRESULT hr = cgD3D9BindProgram(vertProg);

   if (!SUCCEEDED(hr))
   {
    this->mCore->Log
     (
      LL_Error,
      VOODOO_DX89_NAME,
      "Error binding vertex program from '%s': %s.",
      pass->Name().GetData(),
      cgD3D9TranslateHRESULT(hr)
     );

    return;
   }
   else
   {
    mBoundVP = vertProg;
   }
  }
  else
  {
   mDevice->SetVertexShader(nullptr);
  }

  if (cgIsProgram(fragProg))
  {

   HRESULT hr = cgD3D9BindProgram(fragProg);

   if (!SUCCEEDED(hr))
   {
    this->mCore->Log
     (
      LL_Info,
      VOODOO_DX89_NAME,
      "Error binding fragment program from '%s': %s.",
      pass->Name().GetData(),
      cgD3D9TranslateHRESULT(hr)
     );

    if (cgIsProgram(vertProg))
    {
     cgD3D9UnbindProgram(vertProg);
     mBoundVP = nullptr;
    }

    return;
   }
   else
   {
    mBoundFP = fragProg;
   }
  }
  else
  {
   mDevice->SetPixelShader(nullptr);
  }
 }

 /**
  *
  */
 void Adapter::UnbindPass(void)
 {
  if (cgIsProgram(mBoundVP))
  {
   cgD3D9UnbindProgram(mBoundVP);
  }

  if (cgIsProgram(mBoundFP))
  {
   cgD3D9UnbindProgram(mBoundFP);
  }

  mDevice->SetVertexShader(nullptr);
  mDevice->SetPixelShader(nullptr);
 }

 /**
  *
  */
 void Adapter::DrawQuad(Vertex *vertexData)
 {

  IDirect3DVertexBuffer9 *sourceBuffer;
  UINT sourceOffset, sourceStride;
  DWORD sourceFVF, zEnabled, aEnabled, cullMode;

  this->mDevice->GetStreamSource(0, &sourceBuffer, &sourceOffset, &sourceStride);
  this->mDevice->GetFVF(&sourceFVF);
  this->mDevice->GetRenderState(D3DRS_ZENABLE, &zEnabled);
  this->mDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &aEnabled);
  this->mDevice->GetRenderState(D3DRS_CULLMODE, &cullMode);

  this->mDevice->SetStreamSource(0, FSQuadVerts, 0, sizeof(FSVert));
  this->mDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
  this->mDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
  this->mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
  this->mDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

  if (!vertexData)
  {

   HRESULT hr = this->mDevice->BeginScene();

   if (SUCCEEDED(hr))
   {
    this->mDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    this->mDevice->EndScene();
   }
   else
   {
    mCore->Log(LL_Info, VOODOO_DX89_NAME, "Failed to draw quad.");
   }
  }
  else
  {

   HRESULT hr = this->mDevice->BeginScene();

   if (SUCCEEDED(hr))
   {
    hr = this->mDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertexData, sizeof(FSVert));
    if (!SUCCEEDED(hr))
    {
     this->mCore->Log(LL_Info, VOODOO_DX89_NAME, "Error drawing user quad.");
    }

    this->mDevice->EndScene();
   }
  }

  this->mDevice->SetStreamSource(0, sourceBuffer, sourceOffset, sourceStride);
  this->mDevice->SetFVF(sourceFVF);
  this->mDevice->SetRenderState(D3DRS_ZENABLE, zEnabled);
  this->mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, aEnabled);
  this->mDevice->SetRenderState(D3DRS_CULLMODE, cullMode);
 }

 /**
  *
  */
 void Adapter::DrawShader(IShader* shader)
 { }

 /**
  *
  */
 void Adapter::ApplyParameter(IParameter* param)
 {
  switch (Converter::ToParameterCategory(param->GetType()))
  {
  case PC_Float:
   cgD3D9SetUniform(param->GetParameter(), param->Getfloat());
   break;

  case PC_Sampler:
   cgD3D9SetTextureParameter(param->GetParameter(), (IDirect3DTexture9 *) param->GetTexture()->GetData());
   break;

  case PC_Unknown:
  default:
   this->mCore->Log
    (
     LL_Info,
     VOODOO_DX89_NAME,
     "Unable to bind parameter %s of unknown type.",
     param->Name().GetData()
    );
  }
 }

 /**
  *
  */
 bool Adapter::ConnectTexture(IParameter* param, ITexture* texture)
 {
  if (Converter::ToParameterCategory(param->GetType()) == PC_Sampler)
  {
   param->Set(texture);

   IDirect3DTexture9 *texObj = (IDirect3DTexture9 *) texture->GetTexture();
   CGparameter texParam = param->GetParameter();

   cgD3D9SetTextureParameter(texParam, texObj);
   mCore->Log
    (
     LL_Info,
     VOODOO_DX89_NAME,
     "Bound texture %s to parameter %s.",
     texture->Name().GetData(),
     param->Name().GetData()
    );
   return true;
  }
  else
  {
   Throw(VOODOO_DX89_NAME, "Invalid binding attempt, parameter is not a sampler.\n", this->mCore);
   return false;
  }
 }

 /**
  *
  */
 ITexture* Adapter::CreateTexture
(
 std::string name,
 size_t width,
 size_t height,
 size_t depth,
 bool mipmaps,
 TextureFormat format
)
 {

  IDirect3DTexture9 *tex = nullptr;
  D3DFORMAT fmt = DX89_Converter::ToD3DFormat(format);
  HRESULT hr = mDevice->CreateTexture
   (
    width,
    height,
    depth,
    D3DUSAGE_RENDERTARGET,
    fmt,
    D3DPOOL_DEFAULT,
    &tex,
    nullptr
   );

  if (SUCCEEDED(hr))
  {

   ITexture* texRef = mCore->CreateTexture(name, reinterpret_cast < void * > (tex));

   return texRef;
  }
  else
  {

   const char *error = cgD3D9TranslateHRESULT(hr);

   mCore->Log(LL_Error, VOODOO_DX89_NAME, "Error creating texture %s: %s", name, error);
   return ITexture*();
  }
 }

 /**
  *
  */
 void Adapter::HandleError(CGcontext context, CGerror error, void *core)
 {

  ICore *actualCore = reinterpret_cast < ICore * > (core);

  actualCore->Log("Voodoo DX89: Cg error: %s\n", cgD3D9TranslateCGerror(error));
 }
}
}