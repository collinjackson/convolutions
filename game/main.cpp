// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#include <windows.h>
#include <irrlicht.h>
#include <stdio.h>

#include "CStartupMenu.h"
#include "IScenario.h"

using namespace irr;

#pragma comment(lib, "Irrlicht.lib")


int main( )
{

	// Feel free to change these booleans to customize the startup menu
	CStartupMenu menu;
	menu.skip = false;
	menu.startupMenuFullscreen = false;

	// If the startup menu is skipped, we'll use these values
	menu.gameFullscreen = false;
	menu.music = true;
	menu.shadows = false;
	menu.fog = true;
	menu.additive = false;
	menu.currentScenario = IScenario::SCENARIO_CITY_INSERTION;

	while (menu.begin() && menu.scenario->run());
	return 0;
}

