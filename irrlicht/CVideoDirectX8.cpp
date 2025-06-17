
#include "CVideoDirectX8.h"
#include "os.h"
#include "S3DVertex.h"
#include "CDirectX8Texture.h"
#include <stdio.h>

#ifdef WIN32

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECTX_8_

namespace irr
{
namespace video
{


//! constructor
CVideoDirectX8::CVideoDirectX8(const core::dimension2d<s32>& screenSize, HWND window, 
								bool fullscreen, bool stencilbuffer,
								io::IFileSystem* io, bool pureSoftware)
: CVideoNull(io, screenSize), D3DLibrary(0), CurrentRenderMode(ERM_NONE), pID3DDevice(0),
 LastVertexType((video::E_VERTEX_TYPE)-1), ResetRenderStates(true), pID3D(0),
 LastSetLight(-1), Transformation3DChanged(0), StencilBuffer(stencilbuffer),
 DeviceLost(false)
{

	#ifdef _DEBUG
	setDebugName("CVideoDirectX8");
	#endif

	os::Printer::log("DirectX8 Renderer", ELL_INFORMATION);

	CurrentTexture[0] = 0;
	CurrentTexture[1] = 0;

	// create sphere map matrix

	SphereMapMatrix._11 = 0.5f; SphereMapMatrix._12 = 0.0f; 
	SphereMapMatrix._13 = 0.0f; SphereMapMatrix._14 = 0.0f; 
	SphereMapMatrix._21 = 0.0f; SphereMapMatrix._22 =-0.5f;
	SphereMapMatrix._23 = 0.0f; SphereMapMatrix._24 = 0.0f; 
	SphereMapMatrix._31 = 0.0f; SphereMapMatrix._32 = 0.0f;
	SphereMapMatrix._33 = 1.0f; SphereMapMatrix._34 = 0.0f; 
	SphereMapMatrix._41 = 0.5f; SphereMapMatrix._42 = 0.5f; 
	SphereMapMatrix._43 = 0.0f; SphereMapMatrix._44 = 1.0f; 

	core::matrix4 mat;
	UnitMatrix = *(D3DMATRIX*)((void*)&mat);

	// init direct 3d is done in the factory function

	MaxLightDistance = sqrtf(FLT_MAX);
}




//! destructor
CVideoDirectX8::~CVideoDirectX8()
{

	if (CurrentTexture[0])
		CurrentTexture[0]->drop();

	if (CurrentTexture[1])
		CurrentTexture[1]->drop();

	// drop d3d8

	if (pID3DDevice)	
		pID3DDevice->Release();

	if (pID3D)		
		pID3D->Release();
}




//! initialises the Direct3D API
bool CVideoDirectX8::initDriver(const core::dimension2d<s32>& screenSize, HWND hwnd,
								u32 bits, bool fullScreen, bool pureSoftware)
{
	HRESULT hr;
	D3DLibrary = LoadLibrary( "d3d8.dll" );

	if (!D3DLibrary)
	{
		os::Printer::log("Error, could not load d3d8.dll.", ELL_ERROR);
		return false;
	}

	typedef IDirect3D8 * (__stdcall *D3DCREATETYPE)(UINT);
	D3DCREATETYPE d3dCreate = (D3DCREATETYPE) GetProcAddress(D3DLibrary, "Direct3DCreate8");

	if (!d3dCreate)
	{
		os::Printer::log("Error, could not get proc adress of Direct3DCreate8.", ELL_ERROR);
		return false;
	}

	//just like pID3D = Direct3DCreate8(D3D_SDK_VERSION);
	pID3D = (*d3dCreate)(D3D_SDK_VERSION);
	

	if (!pID3D)
	{
		os::Printer::log("Error initializing D3D.", ELL_ERROR);
		return false;
	}

	// print device information
	D3DADAPTER_IDENTIFIER8 dai;
	if (!FAILED(pID3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &dai)))
	{
		char tmp[512];

		s32 Product = HIWORD(dai.DriverVersion.HighPart);
		s32 Version = LOWORD(dai.DriverVersion.HighPart);
		s32 SubVersion = HIWORD(dai.DriverVersion.LowPart);
		s32 Build = LOWORD(dai.DriverVersion.LowPart);

		sprintf(tmp, "%s %s %d.%d.%d.%d", dai.Description, dai.Driver, Product, Version,
			SubVersion, Build);
		os::Printer::log(tmp, ELL_INFORMATION);		
	}



	D3DDISPLAYMODE d3ddm;
	hr = pID3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(hr))
	{
		os::Printer::log("Error: Could not get Adapter Display mode.", ELL_ERROR);
		return false;
	}


	ZeroMemory(&present, sizeof(present));

	present.SwapEffect					= D3DSWAPEFFECT_FLIP; //D3DSWAPEFFECT_COPY;
	present.Windowed					= fullScreen ? FALSE : TRUE;
	present.BackBufferFormat			= d3ddm.Format;
	present.EnableAutoDepthStencil		= TRUE;

	if (fullScreen)
	{
		present.BackBufferWidth = screenSize.Width;
		present.BackBufferHeight = screenSize.Height;
		present.BackBufferFormat = D3DFMT_R5G6B5;
		present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		present.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		// present.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

		// request 32bit mode if user specified 32 bit, added by Thomas St�fe
		if ( bits == 32 ) 
			present.BackBufferFormat = D3DFMT_A8R8G8B8;
	}

	// check stencil buffer compatibility
	if (StencilBuffer)
	{
		present.AutoDepthStencilFormat = D3DFMT_D24S8;
		if(FAILED(pID3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			present.BackBufferFormat, D3DUSAGE_DEPTHSTENCIL, 
			D3DRTYPE_SURFACE, D3DFMT_D24S8)))
		{
			os::Printer::log("Device does not support stencilbuffer, disabling stencil buffer.", ELL_WARNING);
			StencilBuffer = false;
		}
		else
		if(FAILED(pID3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			present.BackBufferFormat, present.BackBufferFormat, D3DFMT_D24S8)))
		{
			os::Printer::log("Depth-stencil format is not compatible with display format, disabling stencil buffer.", ELL_WARNING);
			StencilBuffer = false;
		} 		
	}

	if (!StencilBuffer)
		present.AutoDepthStencilFormat = D3DFMT_D16;


	// create device

	if (pureSoftware)
	{
		hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_REF,	hwnd,
									D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present, &pID3DDevice);

		if (FAILED(hr))
			os::Printer::log("Was not able to create Direct3D8 software device.", ELL_ERROR);
	}
	else
	{
		hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	hwnd,
									D3DCREATE_HARDWARE_VERTEXPROCESSING, &present, &pID3DDevice);

		if(FAILED(hr))
		{
			hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,	hwnd,
										D3DCREATE_MIXED_VERTEXPROCESSING , &present, &pID3DDevice);

			if(FAILED(hr))
			{
				hr = pID3D->CreateDevice(	D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
											D3DCREATE_SOFTWARE_VERTEXPROCESSING, &present, &pID3DDevice);

				if (FAILED(hr))
					os::Printer::log("Was not able to create Direct3D8 device.", ELL_ERROR);
			}
		}
	}


	if (!pID3DDevice)
	{
		os::Printer::log("Was not able to create DirectX8 device.", ELL_ERROR);
		return false;
	}

	// get caps
	pID3DDevice->GetDeviceCaps(&Caps);

	if (StencilBuffer && 
		(!(Caps.StencilCaps & D3DSTENCILCAPS_DECR) ||
		!(Caps.StencilCaps & D3DSTENCILCAPS_INCR) ||
		!(Caps.StencilCaps & D3DSTENCILCAPS_KEEP)))
	{
		os::Printer::log("Device not able to use stencil buffer, disabling stencil buffer.", ELL_WARNING);
		StencilBuffer = false;
	}

	// set default vertex shader
	setVertexShader(EVT_STANDARD);

	ResetRenderStates = true;

	// set the renderstates
	setRenderStates3DMode();

	// set fog mode
	setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// so far so good.
	return true;
}




//! applications must call this method before performing any rendering. returns false if failed.
bool CVideoDirectX8::beginScene(bool backBuffer, bool zBuffer, SColor color)
{
	CVideoNull::beginScene(backBuffer, zBuffer, color);
	HRESULT hr;

	if (!pID3DDevice)
		return false;

	if (DeviceLost)
	{
		if(FAILED(hr = pID3DDevice->TestCooperativeLevel()))
        {
            if (hr == D3DERR_DEVICELOST)
                return false;

            if (hr == D3DERR_DEVICENOTRESET)
				reset();
            return false;
        }
	}

	DWORD flags = 0;

	if (backBuffer)
		flags |= D3DCLEAR_TARGET;

	if (zBuffer)
		flags |= D3DCLEAR_ZBUFFER;

	if (StencilBuffer)
		flags |= D3DCLEAR_STENCIL;

	hr = pID3DDevice->Clear( 0, NULL, flags, color.color, 1.0, 0);
	if (FAILED(hr))
		os::Printer::log("DirectX8 clear failed.", ELL_WARNING);

	hr = pID3DDevice->BeginScene();
	if (FAILED(hr))
	{
		os::Printer::log("DirectX8 begin scene failed.", ELL_WARNING);
		return false;
	}

	return true;
}



//! resets the device
bool CVideoDirectX8::reset()
{
    // reset
	os::Printer::log("Resetting D3D8 device.", ELL_INFORMATION);
	if (FAILED(pID3DDevice->Reset(&present)))
	{
		os::Printer::log("Resetting failed.", ELL_WARNING);
		return false;
	}

	DeviceLost = false;
	ResetRenderStates = true;
	LastVertexType = (E_VERTEX_TYPE)-1;

	if (CurrentTexture[0]) CurrentTexture[0]->drop();
	CurrentTexture[0] = 0;
	if (CurrentTexture[1]) CurrentTexture[1]->drop();
	CurrentTexture[1] = 0;

	setVertexShader(EVT_STANDARD);
	setRenderStates3DMode();
	setFog(FogColor, LinearFog, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	return true;
}



//! applications must call this method after performing any rendering. returns false if failed.
bool CVideoDirectX8::endScene()
{
	CVideoNull::endScene();

	HRESULT hr = pID3DDevice->EndScene();
	if (FAILED(hr))
	{
		os::Printer::log("DirectX8 end scene failed.", ELL_WARNING);
		return false;
	}

	hr = pID3DDevice->Present(NULL, NULL, NULL, NULL);
	if (hr == D3DERR_DEVICELOST)
	{
		DeviceLost = true;
		os::Printer::log("DirectX8 device lost.", ELL_WARNING);
	}
	else
	if (FAILED(hr) && hr != D3DERR_INVALIDCALL)
	{
		os::Printer::log("DirectX8 present failed.", ELL_WARNING);
		return false;
	}

	return true;
}



//! queries the features of the driver, returns true if feature is available
bool CVideoDirectX8::queryFeature(E_VIDEO_DRIVER_FEATURE feature)
{
	switch (feature)
	{
	case EVDF_BILINEAR_FILER:
		return true;
	case EVDF_RENDER_TO_TARGET:
		return false;
	case EVDF_HARDWARE_TL:
		return (Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
	case EVDF_MIP_MAP:
		return (Caps.TextureCaps & D3DPTEXTURECAPS_MIPMAP) != 0;
	case EVDF_STENCIL_BUFFER:
		return StencilBuffer &&  Caps.StencilCaps;
	};

	return false;
}



//! sets transformation
void CVideoDirectX8::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
	Transformation3DChanged = true;

	switch(state)
	{
	case ETS_VIEW:
		pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)&mat));
		break;
	case ETS_WORLD:
		pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)&mat));
		break;
	case ETS_PROJECTION:
		pID3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)((void*)&mat));
		break;
	}

	Matrices[state] = mat;
}




//! sets the current Texture
void CVideoDirectX8::setTexture(s32 stage, video::ITexture* texture)
{
	if (CurrentTexture[stage] == texture)
		return;

	if (texture && texture->getDriverType() != EDT_DIRECTX8)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return;
	}

	if (CurrentTexture[stage])
		CurrentTexture[stage]->drop();

	CurrentTexture[stage] = texture;	

	if (!texture)
		pID3DDevice->SetTexture(stage, 0);
	else
	{
		pID3DDevice->SetTexture(stage, ((CDirectX8Texture*)texture)->getDX8Texture());
		texture->grab();
	}
}



//! sets a material
void CVideoDirectX8::setMaterial(const SMaterial& material)
{
	Material = material;

	setTexture(0, Material.Texture1);
	setTexture(1, Material.Texture2);
}


//! returns a device dependent texture from a software surface (IImage)
//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
video::ITexture* CVideoDirectX8::createDeviceDependentTexture(IImage* surface)
{
	bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);

	return new CDirectX8Texture(surface, pID3DDevice,
		TextureCreationFlags);
}


//! Enables or disables a texture creation flag.
void CVideoDirectX8::setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled)
{
	if (flag == video::ETCF_CREATE_MIP_MAPS && !queryFeature(EVDF_MIP_MAP))
		enabled = false;

	CVideoNull::setTextureCreationFlag(flag, enabled);
}


//! sets a render target
void CVideoDirectX8::setRenderTarget(video::ITexture* texture)
{
	if (texture->getDriverType() != EDT_DIRECTX8)
	{
		os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
		return;
	}
}



//! sets a viewport
void CVideoDirectX8::setViewPort(const core::rect<s32>& area)
{
	core::rect<s32> vp = area;
	core::rect<s32> rendert(0,0, ScreenSize.Width, ScreenSize.Height);
	vp.clipAgainst(rendert);

	D3DVIEWPORT8 viewPort;
	viewPort.X = vp.UpperLeftCorner.X;
	viewPort.Y = vp.UpperLeftCorner.Y;
	viewPort.Width = vp.getWidth();
	viewPort.Height = vp.getHeight();
	viewPort.MinZ = 0.0f;
	viewPort.MaxZ = 1.0f;

	HRESULT hr = D3DERR_INVALIDCALL;
	if (vp.getHeight()>0 && vp.getWidth()>0)
		hr = pID3DDevice->SetViewport(&viewPort);

	if (FAILED(hr))
		os::Printer::log("Failed setting the viewport.", ELL_WARNING);

	ViewPort = vp;
}



//! gets the area of the current viewport
const core::rect<s32>& CVideoDirectX8::getViewPort() const
{
	return ViewPort;
}


//! draws an indexed triangle list
void CVideoDirectX8::drawIndexedTriangleList(const S3DVertex* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	if (!checkPrimitiveCount(vertexCount))
		return;

	CVideoNull::drawIndexedTriangleList(vertices, vertexCount, indexList, triangleCount);

	if (!vertexCount || !triangleCount)
		return;

	setVertexShader(EVT_STANDARD);
	setRenderStates3DMode();

	pID3DDevice->DrawIndexedPrimitiveUP(	D3DPT_TRIANGLELIST, 0,
											vertexCount, 
											triangleCount,
											indexList,
											D3DFMT_INDEX16,
											vertices,
											sizeof(S3DVertex));
}



//! draws an indexed triangle list
void CVideoDirectX8::drawIndexedTriangleList(const S3DVertex2TCoords* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	if (!checkPrimitiveCount(vertexCount))
		return;

	CVideoNull::drawIndexedTriangleList(vertices, vertexCount, indexList, triangleCount);

	if (!vertexCount || !triangleCount)
		return;

	setVertexShader(EVT_2TCOORDS);
	setRenderStates3DMode();

	pID3DDevice->DrawIndexedPrimitiveUP(	D3DPT_TRIANGLELIST, 0,
											vertexCount, 
											triangleCount,
											indexList,
											D3DFMT_INDEX16,
											vertices,
											sizeof(S3DVertex2TCoords));
}



//! Draws an indexed triangle list.
void CVideoDirectX8::drawIndexedTriangleFan(const S3DVertex2TCoords* vertices,
	s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	if (!checkPrimitiveCount(vertexCount))
		return;

	CVideoNull::drawIndexedTriangleFan(vertices, vertexCount, indexList, triangleCount);

	if (!vertexCount || !triangleCount)
		return;

	setVertexShader(EVT_2TCOORDS);
	setRenderStates3DMode();

	pID3DDevice->DrawIndexedPrimitiveUP(	D3DPT_TRIANGLEFAN, 0,
											vertexCount, 
											triangleCount,
											indexList,
											D3DFMT_INDEX16,
											vertices,
											sizeof(S3DVertex2TCoords));
}



//! Draws an indexed triangle fan.
void CVideoDirectX8::drawIndexedTriangleFan(const S3DVertex* vertices,
	s32 vertexCount, const u16* indexList, s32 triangleCount)
{
	if (!checkPrimitiveCount(vertexCount))
		return;

	CVideoNull::drawIndexedTriangleFan(vertices, vertexCount, indexList, triangleCount);

	if (!vertexCount || !triangleCount)
		return;

	setVertexShader(EVT_STANDARD);
	setRenderStates3DMode();

	pID3DDevice->DrawIndexedPrimitiveUP(	D3DPT_TRIANGLEFAN, 0,
											vertexCount, 
											triangleCount,
											indexList,
											D3DFMT_INDEX16,
											vertices,
											sizeof(S3DVertex));
}



//! draws an 2d image
void CVideoDirectX8::draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos)
{
	if (!texture)
		return;

	draw2DImage(texture,destPos, core::rect<s32>(core::position2d<s32>(0,0), texture->getOriginalSize()));
}



//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void CVideoDirectX8::draw2DImage(video::ITexture* texture, const core::position2d<s32>& pos, 
								 const core::rect<s32>& sourceRect, 
								 const core::rect<s32>* clipRect, SColor color, 
								 bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	if (texture)
	{
		if (texture->getDriverType() != EDT_DIRECTX8)
		{
			os::Printer::log("Fatal Error: Tried to copy from a surface not owned by this driver.", ELL_ERROR);
			return;
		}
	}

	if (sourceRect.UpperLeftCorner.X >= sourceRect.LowerRightCorner.X ||
		sourceRect.UpperLeftCorner.Y >= sourceRect.LowerRightCorner.Y)
		return;

	core::position2d<s32> targetPos = pos;
	core::position2d<s32> sourcePos = sourceRect.UpperLeftCorner;
	core::dimension2d<s32> sourceSize(sourceRect.getWidth(), sourceRect.getHeight());
	const core::dimension2d<s32> targetSurfaceSize = ScreenSize;
	
	if (clipRect)
	{
		if (targetPos.X < clipRect->UpperLeftCorner.X)
		{
			sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
			if (sourceSize.Width <= 0)
				return;

			sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
			targetPos.X = clipRect->UpperLeftCorner.X;
		}
		
		if (targetPos.X + sourceSize.Width > clipRect->LowerRightCorner.X)
		{
			sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
			if (sourceSize.Width <= 0)
				return;
		}

		if (targetPos.Y < clipRect->UpperLeftCorner.Y)
		{
			sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
			if (sourceSize.Height <= 0)
				return;

			sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
			targetPos.Y = clipRect->UpperLeftCorner.Y;
		}
		
		if (targetPos.Y + sourceSize.Height > clipRect->LowerRightCorner.Y)
		{
			sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
			if (sourceSize.Height <= 0)
				return;
		}
	}

	// clip these coordinates

	if (targetPos.X<0)
	{
		sourceSize.Width += targetPos.X;
		if (sourceSize.Width <= 0)
			return;

		sourcePos.X -= targetPos.X;
		targetPos.X = 0;
	}
	
	if (targetPos.X + sourceSize.Width > targetSurfaceSize.Width)
	{
		sourceSize.Width -= (targetPos.X + sourceSize.Width) - targetSurfaceSize.Width;
		if (sourceSize.Width <= 0)
			return;
	}

	if (targetPos.Y<0)
	{
		sourceSize.Height += targetPos.Y;
		if (sourceSize.Height <= 0)
			return;

		sourcePos.Y -= targetPos.Y;
		targetPos.Y = 0;
	}
	
	if (targetPos.Y + sourceSize.Height > targetSurfaceSize.Height)
	{
		sourceSize.Height -= (targetPos.Y + sourceSize.Height) - targetSurfaceSize.Height;
		if (sourceSize.Height <= 0)
			return;
	}

	// ok, we've clipped everything.
	// now draw it.

	if (useAlphaChannelOfTexture)
		setRenderStates2DMode(false, true, true);
	else
		setRenderStates2DMode(false, true, false);
	
	setTexture(0, texture);
	
	core::rect<s32> poss(targetPos, sourceSize);

	s32 xPlus = -(ScreenSize.Width>>1);
	f32 xFact = 1.0f / (ScreenSize.Width>>1);

	s32 yPlus = ScreenSize.Height-(ScreenSize.Height>>1);
	f32 yFact = 1.0f / (ScreenSize.Height>>1);

	const core::dimension2d<s32> sourceSurfaceSize = texture->getOriginalSize();
	core::rect<f32> tcoords;
	tcoords.UpperLeftCorner.X = (((f32)sourcePos.X)+0.5f) / texture->getOriginalSize().Width ;
	tcoords.UpperLeftCorner.Y = (((f32)sourcePos.Y)+0.5f) / texture->getOriginalSize().Height;
	tcoords.LowerRightCorner.X = (((f32)sourcePos.X +0.5f + (f32)sourceSize.Width)) / texture->getOriginalSize().Width;
	tcoords.LowerRightCorner.Y = (((f32)sourcePos.Y +0.5f + (f32)sourceSize.Height)) / texture->getOriginalSize().Height;

    S3DVertex vtx[4];
	vtx[0] = S3DVertex((f32)(poss.UpperLeftCorner.X+xPlus) * xFact, (f32)(yPlus-poss.UpperLeftCorner.Y ) * yFact , 0.0f, 0.0f, 0.0f, 0.0f, color, tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	vtx[1] = S3DVertex((f32)(poss.LowerRightCorner.X+xPlus) * xFact, (f32)(yPlus- poss.UpperLeftCorner.Y) * yFact, 0.0f, 0.0f, 0.0f, 0.0f, color, tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	vtx[2] = S3DVertex((f32)(poss.LowerRightCorner.X+xPlus) * xFact, (f32)(yPlus-poss.LowerRightCorner.Y) * yFact, 0.0f, 0.0f, 0.0f, 0.0f, color, tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	vtx[3] = S3DVertex((f32)(poss.UpperLeftCorner.X+xPlus) * xFact, (f32)(yPlus-poss.LowerRightCorner.Y) * yFact, 0.0f, 0.0f, 0.0f, 0.0f, color, tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);	

	s16 indices[6] = {0,1,2,0,2,3};

	setVertexShader(EVT_STANDARD);
	
	pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, &indices[0],
										D3DFMT_INDEX16,&vtx[0],	sizeof(S3DVertex));
}



//! draw an 2d rectangle
void CVideoDirectX8::draw2DRectangle(SColor color, const core::rect<s32>& position,
									 const core::rect<s32>* clip)
{
	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	s32 xPlus = -(ScreenSize.Width>>1);
	f32 xFact = 1.0f / (ScreenSize.Width>>1);

	s32 yPlus = ScreenSize.Height-(ScreenSize.Height>>1);
	f32 yFact = 1.0f / (ScreenSize.Height>>1);

    S3DVertex vtx[4];
	vtx[0] = S3DVertex((f32)(pos.UpperLeftCorner.X+xPlus) * xFact, (f32)(yPlus-pos.UpperLeftCorner.Y) * yFact , 0.0f, 0.0f, 0.0f, 0.0f, color, 0.0f, 0.0f);
	vtx[1] = S3DVertex((f32)(pos.LowerRightCorner.X+xPlus) * xFact, (f32)(yPlus- pos.UpperLeftCorner.Y) * yFact, 0.0f, 0.0f, 0.0f, 0.0f, color, 0.0f, 1.0f);
	vtx[2] = S3DVertex((f32)(pos.LowerRightCorner.X+xPlus) * xFact, (f32)(yPlus-pos.LowerRightCorner.Y) * yFact, 0.0f, 0.0f, 0.0f, 0.0f, color, 1.0f, 0.0f);
	vtx[3] = S3DVertex((f32)(pos.UpperLeftCorner.X+xPlus) * xFact, (f32)(yPlus-pos.LowerRightCorner.Y) * yFact, 0.0f, 0.0f, 0.0f, 0.0f, color, 1.0f, 1.0f);	

	s16 indices[6] = {0,1,2,0,2,3};

	setRenderStates2DMode(color.getAlpha() < 255, false, false);
	setTexture(0,0);

	setVertexShader(EVT_STANDARD);

	pID3DDevice->DrawIndexedPrimitiveUP(	D3DPT_TRIANGLELIST, 0, 4, 2, &indices[0],
											D3DFMT_INDEX16, &vtx[0], sizeof(S3DVertex));
}


//! sets right vertex shader
void CVideoDirectX8::setVertexShader(E_VERTEX_TYPE newType)
{
	if (newType != LastVertexType)
	{
		LastVertexType = newType;
		HRESULT hr = 0;

		switch(newType)
		{
		case EVT_STANDARD:
			hr = pID3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1);
			break;
		case EVT_2TCOORDS:
			hr = pID3DDevice->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2);
			break;
		}

		if (FAILED(hr))
		{
			os::Printer::log("Could not set vertex Shader.", ELL_ERROR);
			return;
		}
	}
}


//! sets the needed renderstates
void CVideoDirectX8::setRenderStates3DMode()
{
	if (!pID3DDevice)
		return;

	if (CurrentRenderMode != ERM_3D)
	{
		// switch back the matrices
		pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)&Matrices[ETS_VIEW]));
		pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)&Matrices[ETS_WORLD]));
		pID3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)((void*)&Matrices[ETS_PROJECTION]));

		pID3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		ResetRenderStates = true;
	}

	if (ResetRenderStates ||
		LastMaterial.AmbientColor != Material.AmbientColor ||
		LastMaterial.DiffuseColor != Material.DiffuseColor ||
		LastMaterial.SpecularColor != Material.SpecularColor ||
		LastMaterial.EmissiveColor != Material.EmissiveColor ||
		LastMaterial.Shininess != Material.Shininess)
	{
		D3DMATERIAL8 mat;
		mat.Diffuse = colorToD3D(Material.DiffuseColor);
		mat.Ambient = colorToD3D(Material.AmbientColor);
		mat.Specular = colorToD3D(Material.SpecularColor);
		mat.Emissive = colorToD3D(Material.EmissiveColor);
		mat.Power = Material.Shininess;
		pID3DDevice->SetMaterial(&mat);
	}

	// blendmode

	if (ResetRenderStates || LastMaterial.MaterialType != Material.MaterialType)
	{
		switch(Material.MaterialType)
		{
		case EMT_SOLID:

			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
			pID3DDevice->SetTransform( D3DTS_TEXTURE0, &UnitMatrix );

			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);

			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			break;

		case EMT_SOLID_2_LAYER:

			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
			pID3DDevice->SetTransform( D3DTS_TEXTURE0, &UnitMatrix );

			pID3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);

			pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA);

			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);		
			pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			break;

		case EMT_TRANSPARENT_ADD_COLOR:

			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
			pID3DDevice->SetTransform( D3DTS_TEXTURE0, &UnitMatrix );

			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);

			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
			pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			break;

		case EMT_TRANSPARENT_VERTEX_ALPHA:
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
			pID3DDevice->SetTransform( D3DTS_TEXTURE0, &UnitMatrix );

			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1,  D3DTA_DIFFUSE);

			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			break;

		case EMT_TRANSPARENT_ALPHA_CHANNEL:
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE ); 
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0); 
			pID3DDevice->SetTransform( D3DTS_TEXTURE0, &UnitMatrix ); 

			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_BLENDTEXTUREALPHA ); 
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE ); 
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT ); 
			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE ); 
			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 ); 
			pID3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE ); 

			pID3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE ); 
			pID3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE ); 

			pID3DDevice->SetRenderState(D3DRS_ALPHAREF, 1); 
			pID3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
			pID3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL); 

			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE); 
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA); 
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA ); 
			pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE); 
			break;

		case EMT_LIGHTMAP_ADD:
		case EMT_LIGHTMAP:
		case EMT_LIGHTMAP_M2:
		case EMT_LIGHTMAP_M4:
		case EMT_LIGHTMAP_LIGHTING:
		case EMT_LIGHTMAP_LIGHTING_M2:
		case EMT_LIGHTMAP_LIGHTING_M4:

			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
			pID3DDevice->SetTransform( D3DTS_TEXTURE0, &UnitMatrix );

			if (Material.MaterialType >= EMT_LIGHTMAP_LIGHTING)
			{
				// with lighting
				pID3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
				pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			}
			else
			{
				pID3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				pID3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			}

			pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
			pID3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);

			if (Material.MaterialType == EMT_LIGHTMAP_ADD)
				pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_ADD);
			else
			if (Material.MaterialType == EMT_LIGHTMAP_M4)
				pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE4X);
			else
			if (Material.MaterialType == EMT_LIGHTMAP_M2)
				pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
			else
				pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);

			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			pID3DDevice->SetTextureStageState (1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);

			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);		
			pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			break;

		case EMT_SPHERE_MAP:

			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);

			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

			pID3DDevice->SetTransform( D3DTS_TEXTURE0, &SphereMapMatrix );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
			break;

		case EMT_REFLECTION_2_LAYER:

			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);

			pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

			pID3DDevice->SetTransform( D3DTS_TEXTURE0, &SphereMapMatrix );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
			break;

		case EMT_TRANSPARENT_REFLECTION_2_LAYER:

			pID3DDevice->SetTextureStageState (0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1,  D3DTA_DIFFUSE);

			pID3DDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pID3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);

			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			pID3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

			pID3DDevice->SetTransform( D3DTS_TEXTURE0, &SphereMapMatrix );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
			break;
		}
	}

    // Bilinear and/or trilinear	
	if ((ResetRenderStates || LastMaterial.BilinearFilter != Material.BilinearFilter
		|| LastMaterial.TrilinearFilter != Material.TrilinearFilter))
	{
		if (Material.BilinearFilter || Material.TrilinearFilter)
		{
			pID3DDevice->SetTextureStageState (0, D3DTSS_MAGFILTER,  D3DTEXF_LINEAR);
			pID3DDevice->SetTextureStageState (0, D3DTSS_MINFILTER,  D3DTEXF_LINEAR);
			pID3DDevice->SetTextureStageState (0, D3DTSS_MIPFILTER, Material.TrilinearFilter ? D3DTEXF_LINEAR : D3DTEXF_POINT);
			
			pID3DDevice->SetTextureStageState (1, D3DTSS_MAGFILTER,  D3DTEXF_LINEAR);
			pID3DDevice->SetTextureStageState (1, D3DTSS_MINFILTER,  D3DTEXF_LINEAR);
			pID3DDevice->SetTextureStageState (1, D3DTSS_MIPFILTER, Material.TrilinearFilter ? D3DTEXF_LINEAR : D3DTEXF_POINT);
		}
		else
		{
			pID3DDevice->SetTextureStageState (0, D3DTSS_MINFILTER,  D3DTEXF_POINT);
			pID3DDevice->SetTextureStageState (0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
			pID3DDevice->SetTextureStageState (0, D3DTSS_MAGFILTER,  D3DTEXF_POINT);

			pID3DDevice->SetTextureStageState (1, D3DTSS_MINFILTER,  D3DTEXF_POINT);
			pID3DDevice->SetTextureStageState (1, D3DTSS_MIPFILTER, D3DTEXF_NONE);
			pID3DDevice->SetTextureStageState (1, D3DTSS_MAGFILTER,  D3DTEXF_POINT);
		}
	}

	// check mipmap levels
	if (ResetRenderStates || LastMaterial.BilinearFilter != Material.BilinearFilter
		|| LastMaterial.TrilinearFilter != Material.TrilinearFilter ||
		Material.Texture1 != LastMaterial.Texture1 || 
		Material.Texture2 != LastMaterial.Texture2)
		disableMipMappingIfNescessary();

	// fillmode

	if (ResetRenderStates || LastMaterial.Wireframe != Material.Wireframe)
	{
		if (!Material.Wireframe)
			pID3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		else
			pID3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	// shademode

	if (ResetRenderStates || LastMaterial.GouraudShading != Material.GouraudShading)
	{
		if (Material.GouraudShading)
			pID3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		else
			pID3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	}

	// lighting

	if (ResetRenderStates || LastMaterial.Lighting != Material.Lighting)
	{
		if (Material.Lighting)
			pID3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		else
			pID3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	}

	// zbuffer

	if (ResetRenderStates || LastMaterial.ZBuffer != Material.ZBuffer)
	{
		if (Material.ZBuffer)
			pID3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		else
			pID3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	}

	// zwrite

	if (!Material.isTransparent())
	{
		if (ResetRenderStates || LastMaterial.ZWriteEnable != Material.ZWriteEnable)
		{
			if (Material.ZWriteEnable)
				pID3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
			else
				pID3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);
		}
	}

	// back face culling

	if (ResetRenderStates || LastMaterial.BackfaceCulling != Material.BackfaceCulling)
	{
		if (Material.BackfaceCulling)
			pID3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
		else
			pID3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE);
	}

	// fog
	if (ResetRenderStates || LastMaterial.FogEnable != Material.FogEnable)
	{
		pID3DDevice->SetRenderState(D3DRS_FOGENABLE, Material.FogEnable); 
	}

	LastMaterial = Material;

	ResetRenderStates = false;

	CurrentRenderMode = ERM_3D;
}


//! enables or disables mipmapping
void CVideoDirectX8::disableMipMappingIfNescessary()
{
	for (s32 i=0 ;i<MATERIAL_MAX_TEXTURES; ++i)
		if (CurrentTexture[i] && !((CDirectX8Texture*)CurrentTexture[i])->hasMipMaps())
			pID3DDevice->SetTextureStageState (i, D3DTSS_MIPFILTER, D3DTEXF_NONE);
}


//! sets the needed renderstates
void CVideoDirectX8::setRenderStatesStencilShadowMode(bool zfail)
{
	if ((CurrentRenderMode != ERM_SHADOW_VOLUME_ZFAIL &&
		CurrentRenderMode != ERM_SHADOW_VOLUME_ZPASS) ||
		Transformation3DChanged)
	{
		// switch back the matrices
		pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)&Matrices[ETS_VIEW]));
		pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)&Matrices[ETS_WORLD]));
		pID3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)((void*)&Matrices[ETS_PROJECTION]));

		Transformation3DChanged = false;

		setTexture(0,0);
		setTexture(1,0);

		pID3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(1,  D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(2,  D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		pID3DDevice->SetVertexShader(D3DFVF_XYZ);
		LastVertexType = (video::E_VERTEX_TYPE)(-1);

		pID3DDevice->SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
		pID3DDevice->SetRenderState( D3DRS_STENCILENABLE, TRUE );
		pID3DDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT);

		//pID3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}

	if (CurrentRenderMode != ERM_SHADOW_VOLUME_ZPASS && !zfail)
	{
		// USE THE ZPASS METHOD

		pID3DDevice->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
		pID3DDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		pID3DDevice->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );

		pID3DDevice->SetRenderState( D3DRS_STENCILREF,       0x1 );
		pID3DDevice->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
		pID3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );

		pID3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE);
		pID3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		pID3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}
	else
	if (CurrentRenderMode != ERM_SHADOW_VOLUME_ZFAIL && zfail)
	{
		// USE THE ZFAIL METHOD

		pID3DDevice->SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
		pID3DDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		pID3DDevice->SetRenderState( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );
		pID3DDevice->SetRenderState( D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP );

		pID3DDevice->SetRenderState( D3DRS_STENCILREF,       0x0 );
		pID3DDevice->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
		pID3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
	    
		pID3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pID3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		pID3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}

	CurrentRenderMode = zfail ? ERM_SHADOW_VOLUME_ZFAIL : ERM_SHADOW_VOLUME_ZPASS;
}



//! sets the needed renderstates
void CVideoDirectX8::setRenderStatesStencilFillMode(bool alpha)
{
	if (CurrentRenderMode != ERM_STENCIL_FILL || Transformation3DChanged)
	{
		core::matrix4 mat;
		pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)&mat));
		pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)&mat));
		pID3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)((void*)&mat));

		pID3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		pID3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		pID3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

		pID3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE );

		pID3DDevice->SetRenderState( D3DRS_STENCILREF,       0x1 );
		pID3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL);
		//pID3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_GREATEREQUAL);
		pID3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
		pID3DDevice->SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		pID3DDevice->SetRenderState( D3DRS_STENCILMASK,      0xffffffff );
		pID3DDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 0xffffffff );
		
		pID3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);

		Transformation3DChanged = false;

		if (alpha)
		{
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		else
		{
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
	}

	CurrentRenderMode = ERM_STENCIL_FILL;
}



//! sets the needed renderstates
void CVideoDirectX8::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
{
	if (!pID3DDevice)
		return;

	if (CurrentRenderMode != ERM_2D || Transformation3DChanged)
	{
		core::matrix4 mat;
		pID3DDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)((void*)&mat));
		pID3DDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)((void*)&mat));
		pID3DDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)((void*)&mat));

		pID3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		//pID3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
		pID3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		pID3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		pID3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

		pID3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE);
		pID3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP,  D3DTOP_DISABLE );

		pID3DDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );

		pID3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);

		pID3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP ); 
		pID3DDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );
		pID3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP ); 
		pID3DDevice->SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP );

		pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		pID3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
		pID3DDevice->SetTransform( D3DTS_TEXTURE0, &UnitMatrix );

		Transformation3DChanged = false;
	}

	if (texture)
	{
		pID3DDevice->SetTextureStageState (0, D3DTSS_MINFILTER,  D3DTEXF_POINT);
		pID3DDevice->SetTextureStageState (0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
		pID3DDevice->SetTextureStageState (0, D3DTSS_MAGFILTER,  D3DTEXF_POINT);

		if (alphaChannel)
		{
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1 );
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
		else
		{
			if (alpha)
			{
				pID3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
				pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
			}
			else
			{
				pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
				pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			}
		}

	}
	else
	{
		if (alpha)
		{
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_SELECTARG1);
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			pID3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pID3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		}
		else
		{
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			pID3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			pID3DDevice->SetTextureStageState (0, D3DTSS_ALPHAOP,  D3DTOP_DISABLE);
			pID3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
	}

	CurrentRenderMode = ERM_2D;
}


//! deletes all dynamic lights there are
void CVideoDirectX8::deleteAllDynamicLights()
{
	for (s32 i=0; i<LastSetLight+1; ++i)
		pID3DDevice->LightEnable(i, false);

	LastSetLight = -1;

	CVideoNull::deleteAllDynamicLights();
}



//! adds a dynamic light
void CVideoDirectX8::addDynamicLight(const SLight& dl)
{
	if ((u32)LastSetLight == Caps.MaxActiveLights-1)
		return;

	CVideoNull::addDynamicLight(dl);

	D3DLIGHT8 light;

	light.Type = D3DLIGHT_POINT;
	light.Diffuse = *(D3DCOLORVALUE*)((void*)(&dl.DiffuseColor));
	light.Specular = *(D3DCOLORVALUE*)((void*)(&dl.SpecularColor));
	light.Ambient = *(D3DCOLORVALUE*)((void*)(&dl.AmbientColor));
	light.Position = *(D3DVECTOR*)((void*)(&dl.Position));
	light.Range = MaxLightDistance;

	light.Attenuation0 = 0.0f;
	light.Attenuation1 = 1.0f / dl.Radius;
	light.Attenuation2 = 0.0f;

	++LastSetLight;
	pID3DDevice->SetLight(LastSetLight, &light);
	pID3DDevice->LightEnable(LastSetLight, true);
}



//! returns the maximal amount of dynamic lights the device can handle
s32 CVideoDirectX8::getMaximalDynamicLightAmount()
{
	return Caps.MaxActiveLights;
}



//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void CVideoDirectX8::setAmbientLight(const SColorf& color)
{
	if (!pID3DDevice)
		return;

	D3DCOLOR col = color.toSColor().color;
	pID3DDevice->SetRenderState(D3DRS_AMBIENT, col);
}



//! \return Returns the name of the video driver. Example: In case of the DirectX8
//! driver, it would return "Direct3D8.1".
const wchar_t* CVideoDirectX8::getName()
{
	return L"Direct3D8.1";
}



//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: Frist, draw all geometry. Then use this method, to draw the shadow
//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
void CVideoDirectX8::drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail)
{
	if (!StencilBuffer || !count)
		return;

	setRenderStatesStencilShadowMode(zfail);

	if (!zfail)
	{ 
		// ZPASS Method

		// Draw front-side of shadow volume in stencil/z only
		pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );
		pID3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
		pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, triangles, sizeof(core::vector3df));

		// Now reverse cull order so front sides of shadow volume are written.
		pID3DDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
		pID3DDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
		pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, triangles, sizeof(core::vector3df));
	}
	else
	{
		// ZFAIL Method

		// Draw front-side of shadow volume in stencil/z only
		pID3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );
		pID3DDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR );
		pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, triangles, sizeof(core::vector3df));

		// Now reverse cull order so front sides of shadow volume are written.
		pID3DDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
		pID3DDevice->SetRenderState( D3DRS_STENCILZFAIL,  D3DSTENCILOP_DECR );
		pID3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, count / 3, triangles, sizeof(core::vector3df));
	}
}



//! Fills the stencil shadow with color. After the shadow volume has been drawn
//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
//! to draw the color of the shadow. 
void CVideoDirectX8::drawStencilShadow(bool clearStencilBuffer, video::SColor leftUpEdge,
			video::SColor rightUpEdge, video::SColor leftDownEdge, video::SColor rightDownEdge)
{
	if (!StencilBuffer)
		return;

    S3DVertex vtx[4];
	vtx[0] = S3DVertex(1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, leftUpEdge, 0.0f, 0.0f);
	vtx[1] = S3DVertex(1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, rightUpEdge, 0.0f, 1.0f);
	vtx[2] = S3DVertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, leftDownEdge, 1.0f, 0.0f);
	vtx[3] = S3DVertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, rightDownEdge, 1.0f, 1.0f);	

	s16 indices[6] = {0,1,2,1,3,2};

	setRenderStatesStencilFillMode(
		leftUpEdge.getAlpha() < 255 || 
		rightUpEdge.getAlpha() < 255 ||
		leftDownEdge.getAlpha() < 255 ||
		rightDownEdge.getAlpha() < 255);

	setTexture(0,0);

	setVertexShader(EVT_STANDARD);

	pID3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, &indices[0],
										D3DFMT_INDEX16, &vtx[0], sizeof(S3DVertex));

	if (clearStencilBuffer)
		pID3DDevice->Clear( 0, NULL, D3DCLEAR_STENCIL,0, 1.0, 0);
}



//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
s32 CVideoDirectX8::getMaximalPrimitiveCount()
{
	return Caps.MaxPrimitiveCount;
}


//! Sets the fog mode.
void CVideoDirectX8::setFog(SColor color, bool linearFog, f32 start, 
	f32 end, f32 density, bool pixelFog, bool rangeFog)
{
	CVideoNull::setFog(color, linearFog, start, end, density, pixelFog, rangeFog);

	if (!pID3DDevice)
		return;

	pID3DDevice->SetRenderState(D3DRS_FOGCOLOR,	color.color);	

	pID3DDevice->SetRenderState(
		pixelFog  ? D3DRS_FOGTABLEMODE : D3DRS_FOGVERTEXMODE,
		linearFog ? D3DFOG_LINEAR : D3DFOG_EXP); 

	if(linearFog) 
	{
		pID3DDevice->SetRenderState(D3DRS_FOGSTART,	*(DWORD*)(&start)); 
		pID3DDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD*)(&end)); 
	}
	else
		pID3DDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)(&density)); 

	if(!pixelFog)
		pID3DDevice->SetRenderState	(D3DRS_RANGEFOGENABLE, rangeFog);	
}

//! Draws a 3d line.
void CVideoDirectX8::draw3DLine(const core::vector3df& start,
	const core::vector3df& end, SColor color)
{
	setVertexShader(EVT_STANDARD);
	setRenderStates3DMode();
	video::S3DVertex v[2];
	v[0].Color = color;
	v[1].Color = color;
	v[0].Pos = start;
	v[1].Pos = end;

	pID3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, v, sizeof(S3DVertex));
}


void CVideoDirectX8::OnResize(const core::dimension2d<s32>& size)
{
	if (!pID3DDevice)
		return;

	CVideoNull::OnResize(size);
	reset();
}


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_DIRECTX_8_
#endif // WIN32




namespace irr
{
namespace video
{

#ifdef _WIN32
//! creates a video driver
IVideoDriver* createDirectX8Driver(const core::dimension2d<s32>& screenSize, HWND window,
								   u32 bits, bool fullscreen, bool stencilbuffer, io::IFileSystem* io, bool pureSoftware)
{
    #ifdef _IRR_COMPILE_WITH_DIRECTX_8_
	CVideoDirectX8* dx8 =  new CVideoDirectX8(screenSize, window, fullscreen, stencilbuffer, io, pureSoftware);
	if (!dx8->initDriver(screenSize, window, bits, fullscreen, pureSoftware))
	{
		dx8->drop();
		dx8 = 0;
	}

	return dx8;
	
	#else
	
	return 0;
	
	#endif // _IRR_COMPILE_WITH_DIRECTX_8_
}
#endif

} // end namespace video
} // end namespace irr




