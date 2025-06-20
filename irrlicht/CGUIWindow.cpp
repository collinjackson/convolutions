// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CGUIWindow.h"
#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIButton.h"
#include "IGUIFont.h"
#include "GUIIcons.h"

namespace irr
{
namespace gui
{

//! constructor
CGUIWindow::CGUIWindow(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
: IGUIWindow(environment, parent, id, rectangle), Dragging(false)
{
	#ifdef _DEBUG
	setDebugName("CGUIWindow");
	#endif

	s32 buttonw = environment->getSkin()->getSize(EGDS_WINDOW_BUTTON_WIDTH);
	s32 posx = RelativeRect.getWidth() - buttonw - 4;

	CloseButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1, GUI_ICON_WINDOW_CLOSE);
	CloseButton->setOverrideFont(Environment->getBuiltInFont());
	posx -= buttonw + 2;

	RestoreButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1, GUI_ICON_WINDOW_RESTORE);
	RestoreButton->setOverrideFont(Environment->getBuiltInFont());
	RestoreButton->setVisible(false);
	posx -= buttonw + 2;

	MinButton = Environment->addButton(core::rect<s32>(posx, 3, posx + buttonw, 3 + buttonw), this, -1, GUI_ICON_WINDOW_MINIMIZE);
	MinButton->setOverrideFont(Environment->getBuiltInFont());
	MinButton->setVisible(false);

	MinButton->grab();
	RestoreButton->grab();
	CloseButton->grab();
}



//! destructor
CGUIWindow::~CGUIWindow()
{
	if (MinButton)
		MinButton->drop();

	if (RestoreButton)
		RestoreButton->drop();

	if (CloseButton)
		CloseButton->drop();
}



//! called if an event happened.
bool CGUIWindow::OnEvent(SEvent event)
{
	switch(event.EventType)
	{
	case EET_GUI_EVENT:
		if (event.GUIEvent.EventType == EGET_ELEMENT_FOCUS_LOST)
		{
			Dragging = false;
			return true;
		}
		else
		if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED)
		{
			if (event.GUIEvent.Caller == CloseButton)
			{
				remove();
				return true;
			}
		}
		break;
	case EET_MOUSE_INPUT_EVENT:
		switch(event.MouseInput.Event)
		{
		case EMIE_LMOUSE_PRESSED_DOWN:
			DragStart.X = event.MouseInput.X;
			DragStart.Y = event.MouseInput.Y;
			if (!Environment->hasFocus(this))
			{
				Dragging = true;
				Environment->setFocus(this);
				if (Parent)
					Parent->bringToFront(this);
			}
			return true;
		case EMIE_LMOUSE_LEFT_UP:
			Dragging = false;
			Environment->removeFocus(this);
			return true;
		case EMIE_MOUSE_MOVED:
			if (Dragging)
			{
				move(core::position2d<s32>(event.MouseInput.X - DragStart.X, event.MouseInput.Y - DragStart.Y));
				DragStart.X = event.MouseInput.X;
				DragStart.Y = event.MouseInput.Y;
				return true;
			}
			break;
		}
	}

	return Parent ? Parent->OnEvent(event) : false;
}



//! draws the element and its children
void CGUIWindow::draw()
{
	if (!IsVisible)
		return;

	IGUISkin* skin = Environment->getSkin();
	video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32> rect = AbsoluteRect;
	core::rect<s32> *cl = &AbsoluteClippingRect;

	// draw body fast
	
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + 1;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_HIGH_LIGHT), rect, cl);

	rect.LowerRightCorner.Y = AbsoluteRect.LowerRightCorner.Y;
	rect.LowerRightCorner.X = rect.UpperLeftCorner.X + 1;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_HIGH_LIGHT), rect, cl);

	rect.UpperLeftCorner.X = AbsoluteRect.LowerRightCorner.X - 1;
	rect.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X;
	rect.UpperLeftCorner.Y = AbsoluteRect.UpperLeftCorner.Y;
    rect.LowerRightCorner.Y = AbsoluteRect.LowerRightCorner.Y;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_DARK_SHADOW), rect, cl);

	rect.UpperLeftCorner.X -= 1;
	rect.LowerRightCorner.X -= 1;
	rect.UpperLeftCorner.Y += 1;
	rect.LowerRightCorner.Y -= 1;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_SHADOW), rect, cl);

	rect.UpperLeftCorner.X = AbsoluteRect.UpperLeftCorner.X;
	rect.UpperLeftCorner.Y = AbsoluteRect.LowerRightCorner.Y - 1;
	rect.LowerRightCorner.Y = AbsoluteRect.LowerRightCorner.Y;
	rect.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_DARK_SHADOW), rect, cl);

	rect.UpperLeftCorner.X += 1;
	rect.LowerRightCorner.X -= 1;
	rect.UpperLeftCorner.Y -= 1;
	rect.LowerRightCorner.Y -= 1;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_SHADOW), rect, cl);

	rect = AbsoluteRect;
	rect.UpperLeftCorner.X +=1;
	rect.UpperLeftCorner.Y +=1;
	rect.LowerRightCorner.X -= 2;
	rect.LowerRightCorner.Y -= 2;
	driver->draw2DRectangle(skin->getColor(EGDC_3D_FACE), rect, cl);

	// draw title bar

	rect = AbsoluteRect;
	rect.UpperLeftCorner.X += 2;
	rect.UpperLeftCorner.Y += 2;
	rect.LowerRightCorner.X -= 2;
	rect.LowerRightCorner.Y = rect.UpperLeftCorner.Y + skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 2;
	driver->draw2DRectangle(skin->getColor(EGDC_ACTIVE_BORDER), rect, cl);

	if (Text.size())
	{
		rect.UpperLeftCorner.X += 2;
		rect.LowerRightCorner.X -= skin->getSize(EGDS_WINDOW_BUTTON_WIDTH) + 5;

		IGUIFont* font = skin->getFont();
		if (font)
			font->draw(Text.c_str(), rect, skin->getColor(EGDC_ACTIVE_CAPTION), false, true, cl);
	}

	IGUIElement::draw();
}



//! Returns pointer to the close button
IGUIButton* CGUIWindow::getCloseButton()
{
	return CloseButton;
}



//! Returns pointer to the minimize button
IGUIButton* CGUIWindow::getMinimizeButton()
{
	return MinButton;
}



//! Returns pointer to the maximize button
IGUIButton* CGUIWindow::getMaximizeButton()
{
	return RestoreButton;
}


} // end namespace gui
} // end namespace irr
