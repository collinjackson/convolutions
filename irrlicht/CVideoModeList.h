// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __IRR_C_VIDEO_MODE_LIST_H_INCLUDED__
#define __IRR_C_VIDEO_MODE_LIST_H_INCLUDED__

#include "IVideoModeList.h"
#include "dimension2d.h"
#include "array.h"

namespace irr
{
namespace video
{

	class CVideoModeList : public IVideoModeList
	{
	public:

		//! constructor
		CVideoModeList();

		//! destructor
		virtual ~CVideoModeList();

		//! Gets amount of video modes in the list.
		virtual s32 getVideoModeCount() const;

		//! Returns the screen size of a video mode in pixels.
		virtual core::dimension2d<s32> getVideoModeResolution(s32 modeNumber) const;

		//! Returns the pixel depth of a video mode in bits.
		virtual s32 getVideoModeDepth(s32 modeNumber) const;

		//! Returns current desktop screen resolution.
		virtual core::dimension2d<s32> getDesktopResolution() const;

		//! Returns the pixel depth of a video mode in bits.
		virtual s32 getDesktopDepth() const;

		//! adds a new mode to the list
		void addMode(const core::dimension2d<s32>& size, s32 depth);

		void setDesktop(s32 desktopDepth, const core::dimension2d<s32>& desktopSize);

	private:

		struct SVideoMode
		{
			core::dimension2d<s32> size;
			s32 depth;

			bool operator==(const SVideoMode& other) const
			{
				return size == other.size && depth == other.depth;
			}

			bool operator <(const SVideoMode& other) const
			{
				return (size.Width < other.size.Width ||
					(size.Width == other.size.Width && 
					size.Height < other.size.Height) ||
					(size.Width == other.size.Width && 
					size.Height == other.size.Height &&
					depth < other.depth));
			}
		};

		core::array<SVideoMode> VideoModes;
		SVideoMode Desktop;
	};

} // end namespace video
} // end namespace irr


#endif

