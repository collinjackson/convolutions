// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#include "CStartupMenu.h"
#include "IScenario.h"
#include "stereogram.h"


CStartupMenu::CStartupMenu()
: startButton(0), device(0), start(false), scenario(0),
	startupMenuFullscreen(false), gameFullscreen(false),
	music(true), shadows(false), additive(false), fog(true),
	currentScenario(IScenario::SCENARIO_CITY_INSERTION),
	transparent(true), buttonPressed(false),
	currentOperatorModel(MODEL_ENSIGN), currentOperativeModel(MODEL_RAMPAGE),
	operatorNode(0), operativeNode(0)
{
}

char *CStartupMenu::getModelFilename(s32 index)
{
	if(index >= MODEL_CIPHER) return "media\\models\\male\\tris.md2";
	else return "media\\models\\female\\tris.md2";
}

char *CStartupMenu::getTextureFilename(s32 index)
{
	switch(index)
	{
	// Women
	case MODEL_ATHENA:		return "media\\models\\female\\athena.pcx";
	case MODEL_BRIANNA:		return "media\\models\\female\\brianna.pcx";
	case MODEL_COBALT:		return "media\\models\\female\\cobalt.pcx";
	case MODEL_DOOMGAL:		return "media\\models\\female\\doomgal.pcx";
	case MODEL_ENSIGN:		return "media\\models\\female\\ensign.pcx";
	case MODEL_JEZEBEL:		return "media\\models\\female\\jezebel.pcx";
	case MODEL_JUNGLE:		return "media\\models\\female\\jungle.pcx";
	case MODEL_LOTUS:		return "media\\models\\female\\lotus.pcx";
	case MODEL_STILETTO:	return "media\\models\\female\\stiletto.pcx";
	case MODEL_VENUS:		return "media\\models\\female\\venus.pcx";
	case MODEL_VOODOO:		return "media\\models\\female\\voodoo.pcx";
	// Men
	case MODEL_CIPHER:		return "media\\models\\male\\cipher.pcx";
	case MODEL_CLAYMORE:	return "media\\models\\male\\claymore.pcx";
	case MODEL_FLAK:		return "media\\models\\male\\flak.pcx";
	case MODEL_GRUNT:		return "media\\models\\male\\grunt.pcx";
	case MODEL_HOWITZER:	return "media\\models\\male\\howitzer.pcx";
	case MODEL_MAJOR:		return "media\\models\\male\\major.pcx";
	case MODEL_NIGHTOPS:	return "media\\models\\male\\nightops.pcx";
	case MODEL_POINTMAN:	return "media\\models\\male\\pointman.pcx";
	case MODEL_PSYCHO:		return "media\\models\\male\\psycho.pcx";
	case MODEL_RAMPAGE:		return "media\\models\\male\\rampage.pcx";
	case MODEL_RAZOR:		return "media\\models\\male\\razor.pcx";
	case MODEL_RECON:		return "media\\models\\male\\recon.pcx";		
	case MODEL_SCOUT:		return "media\\models\\male\\scout.pcx";
	case MODEL_SNIPER:		return "media\\models\\male\\sniper.pcx";
	case MODEL_VIPER:		return "media\\models\\male\\viper.pcx";
	// Something else
	default:				return 0;
	}
}




wchar_t *CStartupMenu::getModelName(s32 index)
{
	switch(index)
	{
	// Women
	case MODEL_ATHENA:		return L"Athena";
	case MODEL_BRIANNA:		return L"Brianna";
	case MODEL_COBALT:		return L"Cobalt";
	case MODEL_DOOMGAL:		return L"Doomgal";
	case MODEL_ENSIGN:		return L"Ensign";
	case MODEL_JEZEBEL:		return L"Jezebel";
	case MODEL_JUNGLE:		return L"Jungle";
	case MODEL_LOTUS:		return L"Lotus";
	case MODEL_STILETTO:	return L"Stiletto";
	case MODEL_VENUS:		return L"Venus";
	case MODEL_VOODOO:		return L"Voodoo";
	// Men
	case MODEL_CIPHER:		return L"Cipher";
	case MODEL_CLAYMORE:	return L"Claymore";
	case MODEL_FLAK:		return L"Flak";
	case MODEL_GRUNT:		return L"Grunt";
	case MODEL_HOWITZER:	return L"Howitzer";
	case MODEL_MAJOR:		return L"Major";
	case MODEL_NIGHTOPS:	return L"Nightops";
	case MODEL_POINTMAN:	return L"Pointman";
	case MODEL_PSYCHO:		return L"Psycho";
	case MODEL_RAMPAGE:		return L"Rampage";
	case MODEL_RAZOR:		return L"Razor";
	case MODEL_RECON:		return L"Recon";		
	case MODEL_SCOUT:		return L"Scout";
	case MODEL_SNIPER:		return L"Sniper";
	case MODEL_VIPER:		return L"Viper";
	// Something else
	default:				return 0;
	}
}

bool CStartupMenu::begin()
{
	start = false;
	bool continueToGame = skip ? true : run();
	skip = false;			// Skipping startup menu is a one-time thing
	if(scenario) delete scenario;
	scenario = IScenario::createScenario(currentScenario, this);
	return scenario && continueToGame;
}

bool CStartupMenu::run()
{
	
	device = createDevice(video::EDT_SOFTWARE,
		core::dimension2d<s32>(640, 480), 16, startupMenuFullscreen, false, this);

	device->getFileSystem()->addZipFileArchive("irrlicht.dat");
	device->getFileSystem()->addZipFileArchive("media\\irrlicht.dat");

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	wchar_t tmp[255];
	swprintf(tmp, 255, L"Zion Operator Training Simulation v%s", device->getVersion());
	device->setWindowCaption(tmp);

	// load font

	gui::IGUIFont* font = guienv->getFont("media/fonthaettenschweiler.bmp");
	if (font)
		guienv->getSkin()->setFont(font);


	// add images

	video::ITexture* desktop = driver->getTexture("media/operator_desktop.bmp");
	driver->makeColorKeyTexture(desktop, core::position2d<s32>(100,100));

	video::ITexture* codeImage = driver->getTexture("media/codegridfalling.bmp");
	video::ITexture* buttonPress = driver->getTexture("media/operator_desktop_buttonpress.bmp");
	video::ITexture* operativeChair = driver->getTexture("media/operator_desktop_chair.bmp");

    const s32 leftX = 5, topY = 330, rightX = 220, bottomY = 475;

	// add tab control
	gui::IGUITabControl* tabctrl = guienv->addTabControl(core::rect<int>(leftX,topY,rightX,bottomY),
		0, true, true);
	gui::IGUITab* crewTab = tabctrl->addTab(L"Crew");
	gui::IGUITab* constructTab = tabctrl->addTab(L"Construct");
	gui::IGUITab* optionTab = tabctrl->addTab(L"Options");

	/*
	// Random text
	gui::IGUITab* aboutTab = tabctrl->addTab(L"About");
	wchar_t* text2 = 
		L"If you are selected to join the crew of a Zion hovercraft, you must possess skills as an operator, regardless of your primary assignment on the ship. Every crew member should be able to assume this role if the circumstances require it. This training program will teach you the basics of reading Matrix code, tapping into cellular networks, and hacking land lines for crew member insertion and extraction.";
	guienv->addStaticText(text2, core::rect<int>(5, 5, rightX-leftX-5, bottomY-topY-5),
		false, true, aboutTab);
	
	*/

	// add checkbox

	int checkY = 10;
	guienv->addCheckBox(gameFullscreen, core::rect<int>(20,checkY,150,checkY+20),
		optionTab, STARTUPMENU_FULLSCREEN_CHECK, L"Fullscreen");
	checkY += 25;

//	Causes bugs if you don't use music, so just use it by default for now	
//	guienv->addCheckBox(music, core::rect<int>(20,checkY,150,checkY+20), 
//		optionTab, STARTUPMENU_MUSIC_CHECK, L"Music & Sfx");
//	checkY += 25;
	guienv->addCheckBox(shadows, core::rect<int>(20,checkY,150,checkY+20), 
		optionTab, STARTUPMENU_SHADOWS_CHECK, L"Shadows");
	checkY += 25;
	guienv->addCheckBox(fog, core::rect<int>(20,checkY,150,checkY+20), 
		optionTab, STARTUPMENU_FOG_CHECK, L"Fog");
	checkY += 25;
	guienv->addCheckBox(additive, core::rect<int>(20,checkY,150,checkY+20), 
		optionTab, STARTUPMENU_ADDITIVE_CHECK, L"Additive Blending");

	wchar_t* operatortext = L"Operator";
	guienv->addStaticText(operatortext, core::rect<int>(20, 10, 80, 25),
		false, true, crewTab);

	wchar_t* operativetext = L"Operative";
	guienv->addStaticText(operativetext, core::rect<int>(120, 10, rightX-leftX, 25),
		false, true, crewTab);

	gui::IGUIListBox* operatorbox = guienv->addListBox(core::rect<int>(10,30,100,100), crewTab, STARTUPMENU_OPERATOR_SELECT);
	for(int i = 0; i < MODEL_COUNT; i++) operatorbox->addItem(getModelName(i));
	operatorbox->setSelected(currentOperatorModel);

	gui::IGUIListBox* operativebox = guienv->addListBox(core::rect<int>(110,30,200,100), crewTab, STARTUPMENU_OPERATIVE_SELECT);
	for(i = 0; i < MODEL_COUNT; i++) operativebox->addItem(getModelName(i));
	operativebox->setSelected(currentOperativeModel);

	// add list box

	gui::IGUIListBox* box = guienv->addListBox(core::rect<int>(20,20,200,75), constructTab, STARTUPMENU_SCENARIO_SELECT);
	for(i = 0; i < IScenario::SCENARIO_COUNT; i++) box->addItem(IScenario::getScenarioName((IScenario::scenario_t)i));
	box->setSelected(currentScenario);


	// add button

//	guienv->getSkin()->setColor(gui::EGDC_BUTTON_TEXT,video::SColor(255,255,0,0));
//	startButton = guienv->addButton(core::rect<int>(446,292,495,338), 0, 2, L"Start");
//	guienv->getSkin()->setColor(gui::EGDC_BUTTON_TEXT,video::SColor(101,210,210,210));

	// add text

	// add md2 model
	
	resetOperator();
	resetOperative();

	f32 operativeY = 1000;
	scene::ICameraSceneNode* operatorCam = smgr->addCameraSceneNode(0, core::vector3df(-10,25,20), core::vector3df(0,20,0));
	smgr->addLightSceneNode(0, core::vector3df(15,25+operativeY+5,-10),video::SColorf(1.0f, 1.0f, 1.0f),100.0f,-1);
	scene::ICameraSceneNode* operativeCam = smgr->addCameraSceneNode(0, core::vector3df(19,23+operativeY,-19), core::vector3df(0,20+operativeY+10,0));
	operativeCam->setFOV(1.7f);
	operativeCam->setUpVector(core::vector3df(0,10,-10));
//	stereogram::Stereogram *stereogram = driver->createStereogram();
//	GlyphSet *glyphSet = stereogram->createGlyphSet("media/codegrid5x1.bmp", core::dimension2d<s32>(5, 5));
//	stereogram->setGlyphSet(glyphSet);

	// irrlicht logo
	video::ITexture* backgroundImage = 0; // driver->getTexture("media/operator_desktop_hole.bmp");

	// set transparency
	setTransparency();

	// draw all

	while(device->run())
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, video::SColor(0,0,0,0));


			// Draw operative's chair
			if (operativeChair)
			{
				driver->draw2DImage(operativeChair, core::position2d<s32>(445, 70),
									core::rect<s32>(0,0,157,148), 0, 
									video::SColor(255,255,255,255), false);
			}

			// Draw operative
			driver->setViewPort(core::rect<s32>(445, 70, 595, 226));
			smgr->setActiveCamera(operativeCam);
			smgr->drawAll();	
			driver->setViewPort(core::rect<s32>(0, 0, 640, 480));

			// Draw some code
			if (codeImage)
			{
				static int codePos = -476;
				driver->draw2DImage(codeImage, core::position2d<s32>(90, 50 - 238 + codePos),
									core::rect<s32>(0,0,340,713), 0, 
									video::SColor(255,255,255,255), false);
				codePos += 3;
				if(codePos > 0) codePos = codePos % 238; // 476;
			}

			// Draw desktop
			if (desktop)
			{
				driver->draw2DImage(desktop, core::position2d<s32>(0,0),
									core::rect<s32>(0,0,640,480), 0, 
									video::SColor(255,255,255,255), true);
			}

			if(buttonPress && buttonPressed)
			{
				driver->draw2DImage(buttonPress, core::position2d<s32>(515,262),
						core::rect<s32>(0,0,68,71), 0, video::SColor(255,255,255,255), false);
			}

			// Draw operator guy
			smgr->setActiveCamera(operatorCam);
			smgr->drawAll();	


			guienv->drawAll();


			driver->endScene();
		}
	}

	device->drop();
	operatorNode = operativeNode = 0;
	return start;
}



bool CStartupMenu::OnEvent(SEvent event)
{
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN )
	{
		s32 x = event.MouseInput.X, y = event.MouseInput.Y; 
		if(x >= 515 && x <= 583 && y > 262 && y <= 333)
		{
			buttonPressed = true;
		}
	}
	else

	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP )
	{
		s32 x = event.MouseInput.X, y = event.MouseInput.Y; 
		if(buttonPressed && x >= 515 && x <= 583 && y > 262 && y <= 333)
		{
			// user clicked start button to start scenario
			device->closeDevice();
			start = true;
		}
		buttonPressed = false;
	}
	else
	if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_RETURN &&
		event.KeyInput.PressedDown == false)
	{
		// user pushed enter to start scenario
		device->closeDevice();
		start = true;
	}
	else
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP )
	{
		core::rect<s32> r(event.MouseInput.X, event.MouseInput.Y, 0, 0);
		gui::IGUIContextMenu* menu = device->getGUIEnvironment()->addContextMenu(r, 0, STARTUPMENU_CONTEXT_MENU);
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
		case STARTUPMENU_CONTEXT_MENU: 
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
		case STARTUPMENU_OPERATOR_SELECT:
			if (event.GUIEvent.EventType == gui::EGET_LISTBOX_CHANGED)
			{
				currentOperatorModel = ((gui::IGUIListBox*)event.GUIEvent.Caller)->getSelected();
				resetOperator();
			}
			break;
		case STARTUPMENU_OPERATIVE_SELECT:
			if (event.GUIEvent.EventType == gui::EGET_LISTBOX_CHANGED)
			{
				currentOperativeModel = ((gui::IGUIListBox*)event.GUIEvent.Caller)->getSelected();
				resetOperative();
			}
			break;
		case STARTUPMENU_SCENARIO_SELECT:
			if (event.GUIEvent.EventType == gui::EGET_LISTBOX_CHANGED)
				currentScenario = (IScenario::scenario_t)((gui::IGUIListBox*)event.GUIEvent.Caller)->getSelected();
			break;
		case STARTUPMENU_FULLSCREEN_CHECK:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				gameFullscreen = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case STARTUPMENU_MUSIC_CHECK:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				music = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case STARTUPMENU_SHADOWS_CHECK:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				shadows = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case STARTUPMENU_FOG_CHECK:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				fog = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		case STARTUPMENU_ADDITIVE_CHECK:
			if (event.GUIEvent.EventType == gui::EGET_CHECKBOX_CHANGED )
				additive = ((gui::IGUICheckBox*)event.GUIEvent.Caller)->isChecked();
			break;
		}
	}

	return false;
}


void CStartupMenu::setTransparency()
{
	for (s32 i=0; i<gui::EGDC_COUNT ; ++i)
	{
		video::SColor col = device->getGUIEnvironment()->getSkin()->
			getColor((gui::EGUI_DEFAULT_COLOR)i);
		col.setAlpha(transparent ? 101 : 255);
		device->getGUIEnvironment()->getSkin()->setColor((gui::EGUI_DEFAULT_COLOR)i, col);
	}
}


void CStartupMenu::resetOperator()
{
	if(operatorNode) operatorNode->remove();

	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver* driver = device->getVideoDriver();

	scene::IAnimatedMesh* operatorMesh = smgr->getMesh(getModelFilename(currentOperatorModel));
	operatorNode = smgr->addAnimatedMeshSceneNode(operatorMesh);
	if (operatorNode)
	{
		operatorNode->setPosition(core::vector3df(0,10,0));
		operatorNode->setMaterialTexture(0, driver->getTexture(getTextureFilename(currentOperatorModel)));
	 	operatorNode->setMaterialFlag(video::EMF_LIGHTING, false);
		operatorNode->setAnimationSpeed(40);
		operatorNode->setMD2Animation(scene::EMAT_STAND);
		// Here's an equivalent statement that doesn't use MD2 models...
		// operatorNode->setFrameLoop(0, 320); 
	}
}

void CStartupMenu::resetOperative()
{
	if(operativeNode) operativeNode->remove();

	scene::ISceneManager* smgr = device->getSceneManager();
	video::IVideoDriver* driver = device->getVideoDriver();

	f32 operativeY = 1000;
	scene::IAnimatedMesh* operativeMesh = smgr->getMesh(getModelFilename(currentOperativeModel));
	operativeNode = smgr->addAnimatedMeshSceneNode(operativeMesh);
	if (operativeNode)
	{
		operativeNode->setMaterialTexture(0, driver->getTexture(getTextureFilename(currentOperativeModel)));
	 	operativeNode->setMaterialFlag(video::EMF_LIGHTING, false);
		if(currentOperativeModel >= MODEL_CIPHER)
		{
			// it's a man; animate it
			operativeNode->setPosition(core::vector3df(0,13+operativeY,0));
			operativeNode->setAnimationSpeed(10);
			operativeNode->setFrameLoop(0, 80); 
		} 
		else
		{
			// women are too shifty... freeze her
			operativeNode->setPosition(core::vector3df(-5,12+operativeY,5));
			operativeNode->setRotation(core::vector3df(10,50,0));
			//	operativeNode->setRotation(core::vector3df(-30,60,-30));
			operativeNode->setAnimationSpeed(8);
			operativeNode->setScale(core::vector3df(1.2f, 1.2f, 1.2f));
			operativeNode->setFrameLoop(60,80);
		}
	}


}