#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECTX_9_

#include "CDirectX9Texture.h"
#include "os.h"

#include <stdio.h>
#include <d3dx9tex.h>
#pragma comment(lib, "d3dx9.lib")

namespace irr
{
namespace video  
{

//! constructor
CDirectX9Texture::CDirectX9Texture(IImage* image, IDirect3DDevice9* device,
								   u32 flags)
: Image(image), Device(device), TextureSize(0,0), 
Texture(0), Pitch(0), ImageSize(0,0), HasMipMaps(false)
{
	#ifdef _DEBUG
    setDebugName("CDirectX9Texture");
	#endif

	bool generateMipLevels = (flags & video::ETCF_CREATE_MIP_MAPS) != 0;

	if (Device)
		Device->AddRef();

	if (Image)
	{
		Image->grab();

		createTexture(flags);

		if (Texture)
		{
			if (copyTexture() && generateMipLevels)
			{
				// create mip maps.
				HRESULT hr  = D3DXFilterTexture(Texture, NULL, D3DX_DEFAULT, D3DX_DEFAULT);
				if (FAILED(hr))
				{
					os::Printer::log("Could not create direct3d mip map levels.", ELL_WARNING);
				}
				else
					HasMipMaps = true;
			}				
		}
		else
			os::Printer::log("Could not create DirectX9 Texture.", ELL_WARNING);
	}
}



//! creates the hardware texture
void CDirectX9Texture::createTexture(u32 flags)
{
	core::dimension2d<s32> optSize;
	ImageSize = Image->getDimension();

	optSize.Width = getTextureSizeFromImageSize(ImageSize.Width);
	optSize.Height = getTextureSizeFromImageSize(ImageSize.Height);

	HRESULT hr;
	D3DFORMAT format = D3DFMT_A1R5G5B5;
	
	switch(getTextureFormatFromFlags(flags))
	{
	case ETCF_ALWAYS_16_BIT:
		format = D3DFMT_A1R5G5B5; break;
	case ETCF_ALWAYS_32_BIT:
		format = D3DFMT_A8R8G8B8; break;
	case ETCF_OPTIMIZED_FOR_QUALITY:
		{
			switch(Image->getColorFormat())
			{
			case ECF_R8G8B8:
			case ECF_A8R8G8B8:
				format = D3DFMT_A8R8G8B8; break;
			case ECF_A1R5G5B5:
			case ECF_R5G6B5:
				format = D3DFMT_A1R5G5B5; break;
			}
		}
		break;
	case ETCF_OPTIMIZED_FOR_SPEED:
		format = D3DFMT_A1R5G5B5; break;
	}

	bool mipmaps = (flags & video::ETCF_CREATE_MIP_MAPS) != 0;

	hr = Device->CreateTexture(optSize.Width, optSize.Height,
		mipmaps ? 0 : 1, // number of mipmaplevels (0 = automatic all)
		0, // usage
		format,  D3DPOOL_MANAGED , &Texture, NULL);

	if (FAILED(hr) && format == D3DFMT_A8R8G8B8)
	{
		// try brute force 16 bit

		format = D3DFMT_A1R5G5B5;

		hr = Device->CreateTexture(optSize.Width, optSize.Height,
			(flags & ETCF_CREATE_MIP_MAPS) ? 0 : 1, // number of mipmaplevels (0 = automatic all)
			0, D3DFMT_A1R5G5B5, D3DPOOL_MANAGED, &Texture, NULL);
	}

	ColorFormat = (format == D3DFMT_A1R5G5B5) ? ECF_A1R5G5B5 : ECF_A8R8G8B8;
}



//! copies the image to the texture
bool CDirectX9Texture::copyTexture()
{
	if (Texture && Image)
	{
		D3DSURFACE_DESC desc;
		Texture->GetLevelDesc(0, &desc);

		TextureSize.Width = desc.Width;
		TextureSize.Height = desc.Height;

		SufaceHasSameSize = (TextureSize == ImageSize);

		if (desc.Format == D3DFMT_A1R5G5B5)
			return copyTo16BitTexture();
		else
		if (desc.Format == D3DFMT_A8R8G8B8)
			return copyTo32BitTexture();
		else
			os::Printer::log("CDirectX9Texture: Unsupported D3D9 hardware texture format", ELL_ERROR);
	}

	return true;
}


//! copies texture to 32 bit hardware texture
bool CDirectX9Texture::copyTo32BitTexture()
{
	D3DLOCKED_RECT rect;
	HRESULT hr = Texture->LockRect(0, &rect, 0, 0);
	if (FAILED(hr))
	{
		os::Printer::log("Could not lock DirectX9 32 bit Texture.", ELL_ERROR);
		return false;
	}

	s32* dest = (s32*)rect.pBits;
	s32* source = (s32*)Image->lock();
	Pitch = rect.Pitch;
	s32 pitch = rect.Pitch / 4;

	if (SufaceHasSameSize)
	{
		if (Image->getColorFormat() == ECF_A8R8G8B8)
		{
			// direct copy, fast

			for (s32 x=0; x<ImageSize.Width; ++x)
				for (s32 y=0; y<ImageSize.Height; ++y)
					dest[x + y*pitch] = source[x + y * ImageSize.Width];
		}
		else
		{
			// slow convert

			for (s32 x=0; x<ImageSize.Width; ++x)
				for (s32 y=0; y<ImageSize.Height; ++y)
					dest[x + y*pitch] = Image->getPixel(x,y).color;
		}
	}
	else
	{
		// scale texture

		f32 sourceXStep = (f32)ImageSize.Width / (f32)TextureSize.Width;
		f32 sourceYStep = (f32)ImageSize.Height / (f32)TextureSize.Height;
		f32 sy;

		if (Image->getColorFormat() == ECF_A8R8G8B8)
		{
			// copy texture scaling

			for (s32 x=0; x<TextureSize.Width; ++x)
			{
				sy = 0.0f;

				for (s32 y=0; y<TextureSize.Height; ++y)
				{
					dest[(s32)(y*pitch + x)] = source[(s32)(((s32)sy)*ImageSize.Width + x*sourceXStep)];
					sy+=sourceYStep;
				}
			}
		}
		else
		{
			// convert texture scaling, slow
			for (s32 x=0; x<TextureSize.Width; ++x)
			{
				sy = 0.0f;

				for (s32 y=0; y<TextureSize.Height; ++y)
				{
					dest[(s32)(y*pitch + x)] = 
						Image->getPixel((s32)(x*sourceXStep), (s32)sy).color;

					sy+=sourceYStep;
				}
			}
		}
	}		

	Image->unlock();

	hr = Texture->UnlockRect(0);
	if (FAILED(hr))
	{
		os::Printer::log("Could not unlock DirectX9 Texture.", ELL_ERROR);
		return false;
	}

	return true;

}


//! optimized for 16 bit to 16 copy. This is in here because before 
//! version 0.4.2, the engine only used A1R5G5B5 hardware textures,
bool CDirectX9Texture::copyTo16BitTexture()
{
	D3DLOCKED_RECT rect;
	HRESULT hr = Texture->LockRect(0, &rect, 0, 0);
	if (FAILED(hr))
	{
		os::Printer::log("Could not lock DirectX9 16 bit Texture.", ELL_ERROR);
		return false;
	}

	s16* dest = (s16*)rect.pBits;
	s16* source = (s16*)Image->lock();
	Pitch = rect.Pitch;
	s32 pitch = rect.Pitch/2;

	if (SufaceHasSameSize)
	{
		// copy texture

		if (Image->getColorFormat() == ECF_A1R5G5B5)
		{
			// direct copy, fast

			for (s32 x=0; x<ImageSize.Width; ++x)
				for (s32 y=0; y<ImageSize.Height; ++y)
					dest[x + y*pitch] = source[x + y * ImageSize.Width];
		}
		else
		{
			// slow convert

			for (s32 x=0; x<ImageSize.Width; ++x)
				for (s32 y=0; y<ImageSize.Height; ++y)
					dest[x + y*pitch] = Image->getPixel(x,y).toA1R5G5B5();
		}
	}
	else
	{
		// scale texture

		f32 sourceXStep = (f32)ImageSize.Width / (f32)TextureSize.Width;
		f32 sourceYStep = (f32)ImageSize.Height / (f32)TextureSize.Height;
		f32 sy;

		if (Image->getColorFormat() == ECF_A1R5G5B5)
		{
			// copy texture scaling

			for (s32 x=0; x<TextureSize.Width; ++x)
			{
				sy = 0.0f;

				for (s32 y=0; y<TextureSize.Height; ++y)
				{
					dest[(s32)(y*pitch + x)] = source[(s32)(((s32)sy)*ImageSize.Width + x*sourceXStep)];
					sy+=sourceYStep;
				}
			}
		}
		else
		{
			// convert texture scaling, slow
			for (s32 x=0; x<TextureSize.Width; ++x)
			{
				sy = 0.0f;

				for (s32 y=0; y<TextureSize.Height; ++y)
				{
					dest[(s32)(y*pitch + x)] = 
						Image->getPixel((s32)(x*sourceXStep), (s32)sy).toA1R5G5B5();

					sy+=sourceYStep;
				}
			}
		}
	}		

	Image->unlock();

	hr = Texture->UnlockRect(0);
	if (FAILED(hr))
	{
		os::Printer::log("Could not unlock DirectX9 16 bit Texture.", ELL_ERROR);
		return false;
	}

	return true;
}



//! destructor
CDirectX9Texture::~CDirectX9Texture()
{
	if (Device)
		Device->Release();

	if (Image)
		Image->drop();

	if (Texture)
		Texture->Release();
}



//! lock function
void* CDirectX9Texture::lock()
{
	if (!Texture)
		return 0;

	D3DLOCKED_RECT rect;
	HRESULT hr = Texture->LockRect(0, &rect, 0, 0);
	if (FAILED(hr))
	{
		os::Printer::log("Could not lock DirectX9 Texture.", ELL_ERROR);
		return 0;
	}

	return rect.pBits; 
}



//! unlock function
void CDirectX9Texture::unlock()
{
	if (!Texture)
		return;

	Texture->UnlockRect(0);
}


//! Returns original size of the texture.
const core::dimension2d<s32>& CDirectX9Texture::getOriginalSize()
{
	return ImageSize;
}


//! Returns (=size) of the texture.
const core::dimension2d<s32>& CDirectX9Texture::getSize()
{
	return TextureSize;
}


//! returns the size of a texture which would be the optimize size for rendering it
inline s32 CDirectX9Texture::getTextureSizeFromImageSize(s32 size)
{
	s32 ts = 0x01;

	while(ts < size)
		ts <<= 1;

	if (ts > size && ts > 64)
		ts >>= 1;

	return ts;
}



//! returns driver type of texture (=the driver, who created the texture)
EDriverType CDirectX9Texture::getDriverType()
{
	return EDT_DIRECTX9;
}



//! returns color format of texture
ECOLOR_FORMAT CDirectX9Texture::getColorFormat()
{
	return ColorFormat;
}



//! returns pitch of texture (in bytes)
s32 CDirectX9Texture::getPitch()
{
	return Pitch;
}



//! returns the DirectX9 Texture
IDirect3DTexture9* CDirectX9Texture::getDX9Texture()
{
	return Texture;
}


//! returns if texture has mipmap levels
bool CDirectX9Texture::hasMipMaps()
{
	return HasMipMaps;
}



} // end namespace video
} // end namespace irr

#endif // _IRR_COMPILE_WITH_DIRECTX_9_
