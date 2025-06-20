// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_GUI_TOOL_BAR_H_INCLUDED__
#define __C_GUI_TOOL_BAR_H_INCLUDED__

#include "IGUIToolBar.h"

namespace irr
{
namespace gui
{

	//! Stays at the top of its parent like the menu bar and contains tool buttons
	class CGUIToolBar : public IGUIToolBar
	{
	public:

		//! constructor
		CGUIToolBar(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! destructor
		~CGUIToolBar();

		//! draws the element and its children
		virtual void draw();

		//! Updates the absolute position.
		virtual void updateAbsolutePosition();

		//! Adds a button to the tool bar
		virtual IGUIButton* addButton(s32 id=-1, const wchar_t* text=0,
			video::ITexture* img=0, video::ITexture* pressed=0,
			bool isPushButton=false);

	private:

		s32 ButtonX;
	};


} // end namespace gui
} // end namespace irr

#endif

