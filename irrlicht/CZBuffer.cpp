// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CZBuffer.h"

namespace irr
{
namespace video
{



//! constructor
CZBuffer::CZBuffer(const core::dimension2d<s32>& size)
: Buffer(0), Size(0,0), TotalSize(0), BufferEnd(0)
{
	#ifdef _DEBUG
	setDebugName("CZBuffer");
	#endif

	setSize(size);
}



//! destructor
CZBuffer::~CZBuffer()
{
	if (Buffer)
		delete [] Buffer;
}



//! clears the zbuffer
void CZBuffer::clear()
{
	TZBufferType* p = Buffer;
	while(p != BufferEnd)
	{
		*p = 0; ++p;
	}
}



//! sets the new size of the zbuffer
void CZBuffer::setSize(const core::dimension2d<s32>& size)
{
	if (size == Size)
		return;

	Size = size;

	if (Buffer)
		delete [] Buffer;

	TotalSize = size.Width * size.Height;
	Buffer = new TZBufferType[TotalSize];
	BufferEnd = Buffer + TotalSize;
}



//! returns the size of the zbuffer
const core::dimension2d<s32>& CZBuffer::getSize()
{
	return Size;
}



//! locks the zbuffer
TZBufferType* CZBuffer::lock()
{
	return Buffer;
}



//! unlocks the zbuffer
void CZBuffer::unlock()
{
}



//! creates a ZBuffer
IZBuffer* createZBuffer(const core::dimension2d<s32>& size)
{
	return new CZBuffer(size);
}

	
} // end namespace video
} // end namespace irr


