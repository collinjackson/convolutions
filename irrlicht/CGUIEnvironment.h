// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_GUI_ENVIRNMENT_H_INCLUDED__
#define __C_GUI_ENVIRNMENT_H_INCLUDED__

#include "IGUIEnvironment.h"
#include "IGUIElement.h"
#include "array.h"
#include "IFileSystem.h"
#include "IOSOperator.h"

namespace irr
{
namespace gui
{

class CGUIEnvironment : public IGUIEnvironment, IGUIElement
{
public:

	//! constructor
	CGUIEnvironment(io::IFileSystem* fs, video::IVideoDriver* driver, IOSOperator* op);

	//! destructor
	virtual ~CGUIEnvironment();

	//! draws all gui elements
	virtual void drawAll();

	//! returns the current video driver
	virtual video::IVideoDriver* getVideoDriver();

	//! posts an input event to the environment
	virtual bool postEventFromUser(SEvent event);

	//! This sets a new event receiver for gui events. Usually you do not have to
	//! use this method, it is used by the internal engine.
	virtual void setUserEventReceiver(IEventReceiver* evr);

	//! called if an event happened.
	virtual bool OnEvent(SEvent event);

	//! returns the current gui skin
	virtual IGUISkin* getSkin();

	//! returns the font
	virtual IGUIFont* getFont(const c8* filename);

	//! adds an button. The returned pointer must not be dropped.
	virtual IGUIButton* addButton(const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0);

	//! adds a window. The returned pointer must not be dropped.
	virtual IGUIWindow* addWindow(const core::rect<s32>& rectangle, bool modal = false, 
		const wchar_t* text=0, IGUIElement* parent=0, s32 id=-1);

	//! Adds a message box.
	virtual IGUIWindow* addMessageBox(const wchar_t* caption, const wchar_t* text=0,
		bool modal = true, s32 flag = EMBF_OK, IGUIElement* parent=0, s32 id=-1);

	//! adds a scrollbar. The returned pointer must not be dropped.
	virtual IGUIScrollBar* addScrollBar(bool horizontal, const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1);

	//! adds an image. The returned pointer must not be dropped.
	virtual IGUIImage* addImage(const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0);

	//! adds a checkbox
	virtual IGUICheckBox* addCheckBox(bool checked, const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0);

	//! adds a list box
	virtual IGUIListBox* addListBox(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1, bool drawBackground=false);

	//! adds an mesh viewer. The returned pointer must not be dropped.
	virtual IGUIMeshViewer* addMeshViewer(const core::rect<s32>& rectangle, IGUIElement* parent=0, s32 id=-1, const wchar_t* text=0);

	//! Adds a file open dialog.
	virtual IGUIFileOpenDialog* addFileOpenDialog(const wchar_t* title = 0, bool modal=true, IGUIElement* parent=0, s32 id=-1);

	//! adds a static text. The returned pointer must not be dropped.
	virtual IGUIStaticText* addStaticText(const wchar_t* text, const core::rect<s32>& rectangle,
		bool border=false, bool wordWrap=true, IGUIElement* parent=0, s32 id=-1);

	//! Adds an edit box. The returned pointer must not be dropped.
	virtual IGUIEditBox* addEditBox(const wchar_t* text, const core::rect<s32>& rectangle, 
		bool border=false, IGUIElement* parent=0, s32 id=-1);

	//! Adds a tab control to the environment.
	virtual IGUITabControl* addTabControl(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, bool fillbackground=false, bool border=true, s32 id=-1);

	//! Adds tab to the environment. 
	virtual IGUITab* addTab(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1);

	//! Adds a context menu to the environment.
	virtual IGUIContextMenu* addContextMenu(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1);

	//! Adds a menu to the environment.
	virtual IGUIContextMenu* addMenu(IGUIElement* parent=0, s32 id=-1);

	//! Adds a toolbar to the environment. It is like a menu is always placed on top
	//! in its parent, and contains buttons.
	virtual IGUIToolBar* addToolBar(IGUIElement* parent=0, s32 id=-1);

	//! Adds a combo box to the environment.
	virtual IGUIComboBox* addComboBox(const core::rect<s32>& rectangle,
		IGUIElement* parent=0, s32 id=-1);

	//! sets the focus to an element
	virtual void setFocus(IGUIElement* element);

	//! removes the focus from an element
	virtual void removeFocus(IGUIElement* element);

	//! Returns if the element has focus
	virtual bool hasFocus(IGUIElement* element);

	//! returns default font
	virtual IGUIFont* getBuiltInFont();

	//! Adds an element for fading in or out.
	virtual IGUIInOutFader* addInOutFader(const core::rect<s32>* rectangle=0, IGUIElement* parent=0, s32 id=-1);

	//! Returns the root gui element. 
    virtual IGUIElement* getRootGUIElement();

private:

	struct SFont
	{
		core::stringc Filename;
		IGUIFont* Font;

		bool operator < (const SFont& other) const
		{
			return (Filename < other.Filename);
		}
	};

	void updateHoveredElement(core::position2d<s32> mousePos);

	void loadBuidInFont();

	core::array<SFont> Fonts;
	video::IVideoDriver* Driver;
	IGUIElement* Hovered;
	IGUIElement* Focus;
	IGUISkin* CurrentSkin;
	io::IFileSystem* FileSystem;
	IEventReceiver* UserReceiver;
	IOSOperator* Operator;
};

} // end namespace gui
} // end namespace irr

#endif

