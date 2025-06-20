// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __C_MAIN_MENU_H_INCLUDED__
#define __C_MAIN_MENU_H_INCLUDED__

#include <irrlicht.h>

using namespace irr;

class CMainMenu : public IEventReceiver
{
public:

	CMainMenu();

	bool run(bool& outFullscreen, bool& outMusic, bool& outShadows,
		bool& outAdditive, video::EDriverType& outDriver);

	virtual bool OnEvent(SEvent event);

private:

	void setTransparency();

	gui::IGUIButton* startButton;
	IrrlichtDevice *device;
	s32 selected;
	bool start;
	bool fullscreen;
	bool music;
	bool shadows;
	bool additive;
	bool transparent;

	scene::IAnimatedMesh* quakeLevel;
	scene::ISceneNode* lightMapNode;
	scene::ISceneNode* dynamicNode;
};

#endif