// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#include <windows.h>
#include <irrlicht.h>
#include <stdio.h>

#include "CMainMenu.h"
#include "CDemo.h"

using namespace irr;

#pragma comment(lib, "Irrlicht.lib")


INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
	bool fullscreen = false;
	bool music = true;
	bool shadows = false;
	bool additive = false;

	video::EDriverType driverType = video::EDT_DIRECTX8;

	CMainMenu menu;

	if (menu.run(fullscreen, music, shadows, additive, driverType))
	{
		CDemo demo(fullscreen, music, shadows, additive, driverType);
		demo.run();		
	}	

	return 0;
}

