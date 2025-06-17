// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_VIDEO_NULL_H_INCLUDED__
#define __C_VIDEO_NULL_H_INCLUDED__

#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "IImagePresenter.h"
#include "array.h"
#include "irrString.h"
#include "IImageLoader.h"
#include "CFPSCounter.h"
#include "S3DVertex.h"
#include "stereogram.h"

namespace irr
{
namespace video
{
	class CVideoNull : public IVideoDriver
	{
	public:

		//! constructor
		CVideoNull(io::IFileSystem* io, const core::dimension2d<s32>& screenSize);

		//! destructor
		virtual ~CVideoNull();

		virtual bool beginScene(bool backBuffer, bool zBuffer, SColor color);

		virtual bool endScene();

		//! queries the features of the driver, returns true if feature is available
		virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature);

		//! sets transformation
		virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

		//! sets a material
		virtual void setMaterial(const SMaterial& material);

		//! loads a Texture
		virtual ITexture* getTexture(const c8* filename);

		//! loads a Texture
		virtual ITexture* getTexture(io::IReadFile* file);

		//! creates a Texture
		virtual ITexture* addTexture(const core::dimension2d<s32>& size, const c8* name, ECOLOR_FORMAT format);

		//! sets a render target
		virtual void setRenderTarget(video::ITexture* texture);

		//! sets a viewport
		virtual void setViewPort(const core::rect<s32>& area);

		//! gets the area of the current viewport
		virtual const core::rect<s32>& getViewPort() const;

		//! draws an indexed triangle list
		virtual void drawIndexedTriangleList(const S3DVertex* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount);

		//! draws an indexed triangle list
		virtual void drawIndexedTriangleList(const S3DVertex2TCoords* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount);

		//! Draws an indexed triangle fan.
		virtual void drawIndexedTriangleFan(const S3DVertex* vertices,
			s32 vertexCount, const u16* indexList, s32 triangleCount);

		//! Draws an indexed triangle list.
		virtual void drawIndexedTriangleFan(const S3DVertex2TCoords* vertices,
			s32 vertexCount, const u16* indexList, s32 triangleCount);

		//! Draws a 3d line.
		virtual void draw3DLine(const core::vector3df& start,
			const core::vector3df& end, SColor color = SColor(255,255,255,255));

		//! Draws a 3d triangle.
		virtual void draw3DTriangle(const core::triangle3df& triangle,
			SColor color = SColor(255,255,255,255));

		//! Draws a 3d axis aligned box.
		virtual void draw3DBox(const core::aabbox3d<f32> box,
			SColor color = SColor(255,255,255,255));

		//! draws an 2d image
		virtual void draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos);

		//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
		virtual void draw2DImage(video::ITexture* texture, const core::position2d<s32>& destPos, 
			const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
			SColor color=SColor(255,255,255,255), bool useAlphaChannelOfTexture=false);

		//! draw an 2d rectangle
		virtual void draw2DRectangle(SColor color, const core::rect<s32>& pos, const core::rect<s32>* clip = 0);

		//! Draws a 2d line. 
		virtual void draw2DLine(const core::position2d<s32>& start,
								const core::position2d<s32>& end, 
								SColor color=SColor(255,255,255,255));

		virtual void setFog(SColor color=SColor(0,255,255,255), bool linearFog=true,
			f32 start=50.0f, f32 end=100.0f, 
			f32 density=0.01f, bool pixelFog=false, bool rangeFog=false);

		//! returns screen size
		virtual core::dimension2d<s32> getScreenSize();

		// returns current frames per second value
		virtual s32 getFPS();

		//! returns amount of primitives (mostly triangles) were drawn in the last frame.
		//! very useful method for statistics.
		virtual u32 getPrimitiveCountDrawn();

		//! deletes all dynamic lights there are
		virtual void deleteAllDynamicLights();

		//! adds a dynamic light
		virtual void addDynamicLight(const SLight& light);

		//! returns the maximal amount of dynamic lights the device can handle
		virtual s32 getMaximalDynamicLightAmount();

		//! \return Returns the name of the video driver. Example: In case of the DirectX8
		//! driver, it would return "Direct3D8.1".
		virtual const wchar_t* getName();

		//! Sets the dynamic ambient light color. The default color is
		//! (0,0,0,0) which means it is dark.
		//! \param color: New color of the ambient light.
		virtual void setAmbientLight(const SColorf& color);

		//! Adds an external image loader to the engine.
		virtual void addExternalImageLoader(IImageLoader* loader);

		//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
		//! this: Frist, draw all geometry. Then use this method, to draw the shadow
		//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
		virtual void drawStencilShadowVolume(const core::vector3df* triangles, s32 count, bool zfail=true);

		//! Fills the stencil shadow with color. After the shadow volume has been drawn
		//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
		//! to draw the color of the shadow. 
		virtual void drawStencilShadow(bool clearStencilBuffer=false, 
			video::SColor leftUpEdge = video::SColor(0,0,0,0), 
			video::SColor rightUpEdge = video::SColor(0,0,0,0),
			video::SColor leftDownEdge = video::SColor(0,0,0,0),
			video::SColor rightDownEdge = video::SColor(0,0,0,0));

		//! Returns current amount of dynamic lights set
		//! \return Current amount of dynamic lights set
		virtual s32 getDynamicLightCount();

		//! Returns light data which was previously set with IVideDriver::addDynamicLight().
		//! \param idx: Zero based index of the light. Must be greater than 0 and smaller
		//! than IVideoDriver()::getDynamicLightCount.
		//! \return Light data.
		virtual const SLight& getDynamicLight(s32 idx);

		//! Removes a texture from the texture cache and deletes it, freeing lot of
		//! memory. 
		virtual void removeTexture(ITexture* texture);

		//! Removes all texture from the texture cache and deletes them, freeing lot of
		//! memory. 
		virtual void removeAllTextures();

		//! Creates an 1bit alpha channel of the texture based of an color key.
		virtual void makeColorKeyTexture(video::ITexture* texture, video::SColor color);

		//! Creates an 1bit alpha channel of the texture based of an color key position.
		virtual void makeColorKeyTexture(video::ITexture* texture, core::position2d<s32> colorKeyPixelPos);

		//! Returns the maximum amount of primitives (mostly vertices) which
		//! the device is able to render with one drawIndexedTriangleList
		//! call.
		virtual s32 getMaximalPrimitiveCount();

		//! Enables or disables a texture creation flag.
		virtual void setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled);

		//! Returns if a texture creation flag is enabled or disabled.
		virtual bool getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag);

		//! Creates a software image from a file. 
		virtual IImage* createImageFromFile(const char* filename);

		//! Creates a software image from a file. 
		virtual IImage* createImageFromFile(io::IReadFile* file);

		//! Draws a mesh buffer
		virtual void drawMeshBuffer(scene::IMeshBuffer* mb);

		//! Only used by the internal engine. Used to notify the driver that
		//! the window was resized.
		virtual void OnResize(const core::dimension2d<s32>& size);

		virtual stereogram::Stereogram* createStereogram();
		virtual void readDepth(f32* buffer);
		virtual void writeColor(u8* buffer);
	
	protected:

		//! deletes all textures
		void deleteAllTextures();

		//! looks if the image is already loaded
		video::ITexture* findTexture(const c8* filename);

		//! opens the file and loads it into the surface
		video::ITexture* loadTextureFromFile(io::IReadFile* file);

		//! adds a surface, not loaded or created by the Irrlicht Engine
		void addTexture(video::ITexture* surface, const c8* filename);

		//! Creates a texture from a loaded IImage.
		virtual ITexture* addTexture(const c8* name, IImage* image);

		//! returns a device dependent texture from a software surface (IImage)
		//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
		virtual video::ITexture* createDeviceDependentTexture(IImage* surface);

		//! checks triangle count and print warning if wrong
		bool checkPrimitiveCount(s32 vtxCount);

		struct SSurface
		{
			core::stringc Filename;
			video::ITexture* Surface;

			bool operator < (const SSurface& other) const
			{
				return (Filename < other.Filename);
			}
		};

		core::array<SSurface> Textures;
		core::array<video::IImageLoader*> SurfaceLoader;
		core::array<SLight> Lights;

		io::IFileSystem* FileSystem;

		core::rect<s32> ViewPort;
		core::dimension2d<s32> ScreenSize;
	
		CFPSCounter FPSCounter;

		u32 PrimitivesDrawn;

		u32 TextureCreationFlags;

		bool LinearFog;
		f32 FogStart;
		f32 FogEnd;
		f32 FogDensity;
		bool PixelFog;
		bool RangeFog;
		SColor FogColor;
	};

} // end namespace video
} // end namespace irr


#endif

