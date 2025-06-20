// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_GUI_MENU_H_INCLUDED__
#define __C_GUI_MENU_H_INCLUDED__

#include "CGUIContextMenu.h"

namespace irr
{
namespace gui
{

	//! GUI menu interface.
	class CGUIMenu : public CGUIContextMenu
	{
	public:

		//! constructor
		CGUIMenu(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle);

		//! destructor
		~CGUIMenu();

		//! draws the element and its children
		virtual void draw();

		//! called if an event happened.
		virtual bool OnEvent(SEvent event);

		//! Updates the absolute position.
		virtual void updateAbsolutePosition();

	protected:

		virtual void recalculateSize();

		//! returns the item highlight-area
		virtual core::rect<s32> getHRect(SItem& i, core::rect<s32>& absolute);

		//! Gets drawing rect of Item
		virtual core::rect<s32> getRect(SItem& i, core::rect<s32>& absolute);

		void closeAllSubMenus();

	};

} // end namespace gui
} // end namespace irr

#endif

