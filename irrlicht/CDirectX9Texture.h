// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_DIRECTX9_TEXTURE_H_INCLUDED__
#define __C_DIRECTX9_TEXTURE_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECTX_9_

#include "ITexture.h"
#include "IImage.h"
#include <d3d9.h>

namespace irr
{
namespace video  
{

/*!
	interface for a Video Driver dependent Texture.
*/
class CDirectX9Texture : public ITexture
{
public:

	//! constructor
	CDirectX9Texture(IImage* image, IDirect3DDevice9* device,
		u32 flags);

	//! destructor
	virtual ~CDirectX9Texture();

	//! lock function
	virtual void* lock();

	//! unlock function
	virtual void unlock();

	//! Returns original size of the texture.
	virtual const core::dimension2d<s32>& getOriginalSize();

	//! Returns (=size) of the texture.
	virtual const core::dimension2d<s32>& getSize();	

	//! returns driver type of texture (=the driver, who created the texture)
	virtual EDriverType getDriverType();

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat();

	//! returns pitch of texture (in bytes)
	virtual s32 getPitch();

	//! returns the DirectX9 Texture
	IDirect3DTexture9* getDX9Texture();

	//! returns if texture has mipmap levels
	bool hasMipMaps();

private:

	//! returns the size of a texture which would be the optimize size for rendering it
	inline s32 getTextureSizeFromImageSize(s32 size);

	//! creates the hardware texture
	void createTexture(u32 flags);

	//! copies the image to the texture
	bool copyTexture();

	//! optimized for 16 bit to 16 copy. This is in here because before 
	//! version 0.4.2, the engine only used A1R5G5B5 hardware textures,
	bool copyTo16BitTexture();

	//! copies texture to 32 bit hardware texture
	bool copyTo32BitTexture();

	IImage* Image;
	IDirect3DDevice9* Device;
	IDirect3DTexture9* Texture;
	core::dimension2d<s32> TextureSize;
	core::dimension2d<s32> ImageSize;
	s32 Pitch;
	ECOLOR_FORMAT ColorFormat;
	bool SufaceHasSameSize; // true if image has the same dimension as texture.
	bool HasMipMaps;
};


} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_DIRECTX_9_

#endif // __C_DIRECTX9_TEXTURE_H_INCLUDED__

