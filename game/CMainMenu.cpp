// Main Menu for Matrix Convolutions game

#include "CMainMenu.h"
#include "IScenario.h"
#include "stereogram.h"
#include <assert.h>

CMainMenu::CMainMenu(IScenario* scenario)
	:	device(scenario->getDevice()), 
		stereogram(scenario->getStereogram()), 
		stream(scenario->getStream()),
		visible(false), tabctrl(0), transparent(true), returnToStartupMenu(false),
		gravity((int)(255.0f / 2.0f)),
		musicVolume(1.0f / 3.0f * 255.0f)
{
	if(stereogram)
	{
		glyphSet = GLYPH_SIZE_7;
		glyphSets[GLYPH_SIZE_5] = stereogram->createGlyphSet("media/codegrid5x1.bmp", core::dimension2d<s32>(5, 5));
		glyphSets[GLYPH_SIZE_7] = stereogram->createGlyphSet("media/codegrid7x1.bmp", core::dimension2d<s32>(7, 7));
		glyphSets[GLYPH_SIZE_9] = stereogram->createGlyphSet("media/codegrid9x1.bmp", core::dimension2d<s32>(9, 9));
		glyphSets[GLYPH_SIZE_14] = stereogram->createGlyphSet("media/codegrid14x1.bmp", core::dimension2d<s32>(14, 14));
		stereogram->setGlyphSet(glyphSets[glyphSet]);

		showGlyphs = stereogram->getShowGlyphs();
		streamDown = stereogram->getStreamDown();
		handicapMode = stereogram->getHandicapMode();
		flipDepth = stereogram->getFlipDepth();
		autoFocus = stereogram->getAutoFocus();
		focusDepth = (int)(stereogram->getFocusDepth() * 255.0f);
		brightness = (int)(stereogram->getBrightness() * 255.0f);
		brightnessSmoothing = (int)(stereogram->getBrightnessSmoothing() / 2.0f * 255.0f);
	}
}

bool CMainMenu::OnEvent(SEvent event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_ESCAPE &&
		event.KeyInput.PressedDown == false)
	{
		hide();
	}
	else
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP )
	{
		core::rect<s32> r(event.MouseInput.X, event.MouseInput.Y, 0, 0);
		gui::IGUIContextMenu* menu = device->getGUIEnvironment()->addContextMenu(r, 0, MAINMENU_CONTEXT_MENU);
		menu->addItem(L"transparent menus", 666, transparent == false);
		menu->addItem(L"solid menus", 666, transparent == true);
		menu->addSeparator();
		menu->addItem(L"Cancel");
	}
	else
	if (event.EventType == EET_GUI_EVENT)
	{
		s32 id = event.GUIEvent.Caller->getID();
		switch(id)
		{
		case MAINMENU_CONTEXT_MENU: // context menu
			if (event.GUIEvent.EventType == gui::EGET_MENU_ITEM_SELECTED)
			{
				s32 s = ((gui::IGUIContextMenu*)event.GUIEvent.Caller)->getSelectedItem();
				if (s == 0 || s == 1)
				{
					transparent = !transparent;
					setTransparency();
				}
			}
			break;
		case MAINMENU_RESUME_BUTTON:
			if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED )
			{
				hide();					// Return to game
			} 
			break;
		case MAINMENU_EXIT_BUTTON:
			if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED )
			{
				exit();
			} 
			break;
		case MAINMENU_RESIGN_BUTTON:
			if (event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED )
			{
				resign();
			} 
			break;
		case MAINMENU_GRAVITY:
			if (event.GUIEvent.EventType == gui::EGET_SCROLL_BAR_CHANGED )
			{
				gravity = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
			} 
			break;
		case MAINMENU_FLIP_DEPTH:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
			{
				flipDepth = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
				if(stereogram) stereogram->setFlipDepth(flipDepth);
			} 
			break;
		case MAINMENU_SHOW_GLYPHS:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
			{
				showGlyphs = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
				if(stereogram) stereogram->setShowGlyphs(showGlyphs);
			} 
			break;
		case MAINMENU_STREAM_DOWN:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
			{
				streamDown = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
				if(stereogram) stereogram->setStreamDown(streamDown);
			} 
			break;
		case MAINMENU_AUTO_FOCUS:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
			{
				autoFocus = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
				if(stereogram) stereogram->setAutoFocus(autoFocus);
			} 
			break;
		case MAINMENU_HANDICAP_MODE:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
			{
				handicapMode = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
				if(stereogram) stereogram->setHandicapMode(handicapMode);
			} 
			break;
		case MAINMENU_FOCUS_DEPTH:
			if (event.GUIEvent.EventType == gui::EGET_SCROLL_BAR_CHANGED )
			{
				focusDepth = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				if(stereogram) stereogram->setFocusDepth((f32)focusDepth / 255.0f);
			} 
			break;
		case MAINMENU_BRIGHTNESS:
			if (event.GUIEvent.EventType == gui::EGET_SCROLL_BAR_CHANGED )
			{
				brightness = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				if(stereogram) stereogram->setBrightness((f32)brightness / 255.0f);
			} 
			break;
		case MAINMENU_BRIGHTNESS_SMOOTHING:
			if (event.GUIEvent.EventType == gui::EGET_SCROLL_BAR_CHANGED )
			{
				brightnessSmoothing = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				if(stereogram) stereogram->setBrightnessSmoothing((float)brightnessSmoothing / 255.0f * 2.0f);
			} 
			break;
		case MAINMENU_GLYPH_SET:
			if (event.GUIEvent.EventType == gui::EGET_SCROLL_BAR_CHANGED )
			{
				glyphSet = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				if(stereogram) stereogram->setGlyphSet(glyphSets[glyphSet]);
			} 
			break;
		case MAINMENU_MUSIC_VOLUME:
			if (event.GUIEvent.EventType == gui::EGET_SCROLL_BAR_CHANGED )
			{
				musicVolume = ((gui::IGUIScrollBar*)event.GUIEvent.Caller)->getPos();
				if(stream) stream->setVolume((f32)musicVolume * 3.0f / 255.0f);
			} 
			break;
		}
	}

	return false;
}


void CMainMenu::setTransparency()
{
	for (s32 i=0; i<gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = device->getGUIEnvironment()->getSkin()->
			getColor((gui::EGUI_DEFAULT_COLOR)i);
		col.setAlpha(transparent ? 180 : 255);
		device->getGUIEnvironment()->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
	}
}

void CMainMenu::show()
{
	// Save settings
	savedReceiver = device->getEventReceiver();
	savedCamera = device->getSceneManager()->getActiveCamera();
	savedCursorVisibility = device->getCursorControl()->isVisible();
	savedCamera->setMouseInputReceiverEnabled(false);

	// Update camera before switching 

	menuCamera = device->getSceneManager()->addCameraSceneNode(0, savedCamera->getPosition(), savedCamera->getTarget());
	
	// Synch menuCamera up with savedCamera; doesn't seem to be necessary
	// menuCamera->setPosition(savedCamera->getPosition());
	// menuCamera->setProjectionMatrix(savedCamera->getProjectionMatrix());
	// menuCamera->setTarget(savedCamera->getTarget());
	// menuCamera->setUpVector(savedCamera->getUpVector());
	// menuCamera->setNearValue(savedCamera->getNearValue());
	// menuCamera->setFarValue(savedCamera->getFarValue());
	// menuCamera->setAspectRatio(savedCamera->getAspectRatio());
	// menuCamera->setFOV(savedCamera->getFOV());

	// Switch to menu settings
	device->setEventReceiver(this);
	device->getSceneManager()->setActiveCamera(menuCamera);
	device->getCursorControl()->setVisible(true);

	if(!tabctrl)
	{
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();
	// load font

	gui::IGUIFont* font = guienv->getFont("media/fonthaettenschweiler.bmp");
	if (font)
		guienv->getSkin()->setFont(font);

	// add images

//	gui::IGUIImage* img = guienv->addImage(core::rect<int>(0,0,512,384));
//	img->setImage(driver->getTexture("media/techdemoback.bmp"));

    const s32 leftX = 230;

	// add tab control
	tabctrl = guienv->addTabControl(core::rect<int>(leftX,50,leftX + 230,384),
		0, true, true);
	tabctrl->setVisible(visible);
	gui::IGUITab* gameTab = tabctrl->addTab(L"Game");
	gui::IGUITab* constructTab = tabctrl->addTab(L"Construct");
	gui::IGUITab* operatorTab = tabctrl->addTab(L"Operator");
	

	// Could potentially make the background green or something cute like that
	//	gameTab->setDrawBackground(true);
	//	gameTab->setBackgroundColor(video::SColor(180, 0, 180, 0));	


	// add list box
	// gui::IGUIListBox* box = guienv->addListBox(core::rect<int>(10,10,220,75), gameTab, 1);
	// box->addItem(L"OpenGL 1.2");
	// box->addItem(L"Direct3D 8.1");
	// box->addItem(L"Direct3D 9.0b");	
	// box->addItem(L"Irrlicht Software Renderer 1.0");
	// box->setSelected(selected);


	{
	// add buttons
	s32 ymin = 15, ypos = ymin, xmin = 20, xpos = xmin;

	guienv->addStaticText(L"Music Volume", core::rect<int>(20, ypos, 220, ypos+20), false, false, gameTab);
	ypos +=20;
	gui::IGUIScrollBar* musicVolumeScrollBar = guienv->addScrollBar(true, core::rect<int>(20, ypos, 220, ypos+20), gameTab, MAINMENU_MUSIC_VOLUME);
	musicVolumeScrollBar->setMax(255);
	musicVolumeScrollBar->setPos(musicVolume);

	ypos = 160;
	guienv->addButton(core::rect<int>(30,ypos,200,ypos+25), gameTab, MAINMENU_RESUME_BUTTON, L"Resume");
	ypos+=30;
	guienv->addButton(core::rect<int>(30,ypos,200,ypos+25), gameTab, MAINMENU_RESIGN_BUTTON, L"Restart");
	ypos+=30;
	guienv->addButton(core::rect<int>(30,ypos,200,ypos+25), gameTab, MAINMENU_EXIT_BUTTON, L"Exit");
	ypos+=30;
	}



	// Construct setup
	{
	s32 ymin = 15, ypos = ymin, xmin = 20, xpos = xmin;

	guienv->addStaticText(L"Gravity", core::rect<int>(20, ypos, 220, ypos+20), false, false, constructTab);
	ypos +=20;
	gui::IGUIScrollBar* focusDepthScrollBar = guienv->addScrollBar(true, core::rect<int>(20, ypos, 220, ypos+20), constructTab, MAINMENU_GRAVITY);
	focusDepthScrollBar->setMax(255);
	focusDepthScrollBar->setPos(gravity);
	}

	// add checkbox

	// guienv->addCheckBox(fullscreen, core::rect<int>(20,105,130,130), 
	// 	gameTab, 3, L"Fullscreen");
	// guienv->addCheckBox(music, core::rect<int>(20,130,130,155), 
	// 	gameTab, 4, L"Music & Sfx");
	// guienv->addCheckBox(shadows, core::rect<int>(20,160,130,175), 
	// 	gameTab, 5, L"Shadows");
	// guienv->addCheckBox(additive, core::rect<int>(20,180,230,205),
	// 	gameTab, 6, L"Old HW compatible blending");

	// add text

	wchar_t* text = L"Welcome to the Irrlicht Engine. Please select "\
		L"the settings you prefer and press 'Start Demo'. "\
		L"Right click for changing menu style.";

	//	guienv->addStaticText(text, core::rect<int>(10, 220, 220, 280),
	//		true, true, gameTab);

	// add about text

	//	wchar_t* text2 = L"";

	{
	s32 ymin = 15, ypos = ymin, xmin = 20, xpos = xmin;

	/*
	// Scroll bar template
	gui::IGUIListBox* glyphSizeBox = guienv->addListBox(core::rect<int>(10,ypos,220,ypos+65), operatorTab, MAINMENU_GLYPH_SET);
	glyphSizeBox->addItem(L"5");
	glyphSizeBox->addItem(L"7");
	glyphSizeBox->addItem(L"9");
	glyphSizeBox->addItem(L"14");
	glyphSizeBox->setSelected(glyphSet);
	ypos += 75;
	*/

	guienv->addCheckBox(showGlyphs, core::rect<int>(xpos,ypos,xpos+100,ypos+20), operatorTab, MAINMENU_SHOW_GLYPHS, L"Show glyphs");

	ypos +=25;  // separator
	xpos = xmin;

	guienv->addCheckBox(streamDown, core::rect<int>(xpos,ypos,xpos+100,ypos+20), operatorTab, MAINMENU_STREAM_DOWN, L"Stream down");
	xpos += 100;
	guienv->addCheckBox(handicapMode, core::rect<int>(xpos,ypos,xpos+100,ypos+20), operatorTab, MAINMENU_HANDICAP_MODE, L"Handicap Mode");

	ypos +=25;  // separator
	xpos = xmin;

	guienv->addCheckBox(flipDepth, core::rect<int>(xpos,ypos,xpos+100,ypos+20), operatorTab, MAINMENU_FLIP_DEPTH, L"Flip Depth");
	xpos += 100;
	guienv->addCheckBox(autoFocus, core::rect<int>(xpos,ypos,xpos+100,ypos+20), operatorTab, MAINMENU_AUTO_FOCUS, L"Auto Focus");

	ypos +=25;  // separator
	xpos = xmin;

	guienv->addStaticText(L"Focus Depth", core::rect<int>(20, ypos, 220, ypos+20), false, false, operatorTab);
	ypos +=20;
	gui::IGUIScrollBar* focusDepthScrollBar = guienv->addScrollBar(true, core::rect<int>(20, ypos, 220, ypos+20), operatorTab, MAINMENU_FOCUS_DEPTH);
	focusDepthScrollBar->setMax(255);
	focusDepthScrollBar->setPos(focusDepth);

	ypos +=25;	// separator

	guienv->addStaticText(L"Glyph Size", core::rect<int>(20, ypos, 220, ypos+20), false, false, operatorTab);
	ypos +=20;
	gui::IGUIScrollBar* glyphSetScrollBar = guienv->addScrollBar(true, core::rect<int>(20, ypos, 220, ypos+20), operatorTab, MAINMENU_GLYPH_SET);
	glyphSetScrollBar->setMax(GLYPH_SIZE_NUM-1);
	glyphSetScrollBar->setPos(glyphSet);

	ypos +=25;	// separator

	guienv->addStaticText(L"Brightness", core::rect<int>(20, ypos, 220, ypos+20), false, false, operatorTab);
	ypos +=20;
	gui::IGUIScrollBar* brightnessScrollBar = guienv->addScrollBar(true, core::rect<int>(20, ypos, 220, ypos+20), operatorTab, MAINMENU_BRIGHTNESS);
	brightnessScrollBar->setMax(255);
	brightnessScrollBar->setPos(brightness);

	ypos +=25;	// separator

	guienv->addStaticText(L"Brightness Smoothing", core::rect<int>(20, ypos, 220, ypos+20),	false, false, operatorTab);
	ypos +=20;
	gui::IGUIScrollBar* brightnessSmoothingScrollBar = guienv->addScrollBar(true, core::rect<int>(20, ypos, 220, ypos+20), operatorTab, MAINMENU_BRIGHTNESS_SMOOTHING);
	brightnessSmoothingScrollBar->setMax(255);
	brightnessSmoothingScrollBar->setPos(brightnessSmoothing);
	}


	// set transparency
	setTransparency();
	}

	// Tab is ready to display	
	visible = true;
	tabctrl->setVisible(visible);

}

void CMainMenu::hide()
{

	// Restore settings
	device->setEventReceiver(savedReceiver);
	device->getSceneManager()->setActiveCamera(savedCamera);
	savedCamera->setMouseInputReceiverEnabled(true);
	device->getCursorControl()->setVisible(savedCursorVisibility);
	menuCamera->remove();

	// Hide menu
	visible = false;
	tabctrl->setVisible(visible);
}

void CMainMenu::resign()
{
	returnToStartupMenu = true;
	stream->stop();
	device->closeDevice();
}

void CMainMenu::exit()
{
	returnToStartupMenu = false;
	stream->stop();
	device->closeDevice();
}