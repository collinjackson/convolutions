// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __C_STARTUP_MENU_H_INCLUDED__
#define __C_STARTUP_MENU_H_INCLUDED__

#include <irrlicht.h>
#include "IScenario.h"

using namespace irr;

class CStartupMenu : public IEventReceiver
{
public:

	CStartupMenu();

	// Returns true if the user pressed the start button, false if they wanted to quit
	bool begin();

	virtual bool OnEvent(SEvent event);

	// These are publicly accessible intentionally because the main purpose of the startup menu
	// is to pass them on to the scenario
	bool skip;
	bool startupMenuFullscreen;
	bool gameFullscreen;
	bool music;
	bool shadows;
	bool fog;
	bool additive;
	char operativeModelPath;
	char operativeTexturePath;
	IScenario *scenario;
	int currentOperatorModel;
	int currentOperativeModel;
	IScenario::scenario_t currentScenario;

	char* getModelFilename(s32 index);
	char* getTextureFilename(s32 index);
	wchar_t* getModelName(s32 index);

private:

	void setTransparency();
	bool run();

	gui::IGUIButton* startButton;
	IrrlichtDevice *device;
	s32 selected;
	bool start;
	bool transparent;
	bool buttonPressed;

	enum
	{
		// Women
		MODEL_ATHENA = 0,
		MODEL_BRIANNA,
		MODEL_COBALT,
		MODEL_DOOMGAL,
		MODEL_ENSIGN,
		MODEL_JEZEBEL,
		MODEL_JUNGLE,
		MODEL_LOTUS,
		MODEL_STILETTO,
		MODEL_VENUS,
		MODEL_VOODOO,
		// Men
		MODEL_CIPHER,	
		MODEL_CLAYMORE,
		MODEL_FLAK,	
		MODEL_GRUNT,	
		MODEL_HOWITZER,
		MODEL_MAJOR,	
		MODEL_NIGHTOPS,
		MODEL_POINTMAN,	
		MODEL_PSYCHO,	
		MODEL_RAMPAGE,
		MODEL_RAZOR,	
		MODEL_RECON,	
		MODEL_SCOUT,
		MODEL_SNIPER,
		MODEL_VIPER,
		// Something else
		MODEL_COUNT
	};

	enum
	{
		STARTUPMENU_OPERATOR_SELECT,
		STARTUPMENU_OPERATIVE_SELECT,
		STARTUPMENU_SCENARIO_SELECT,
		STARTUPMENU_FULLSCREEN_CHECK,
		STARTUPMENU_MUSIC_CHECK,
		STARTUPMENU_SHADOWS_CHECK,
		STARTUPMENU_FOG_CHECK,
		STARTUPMENU_ADDITIVE_CHECK,
		STARTUPMENU_CONTEXT_MENU
	};

	scene::IAnimatedMeshSceneNode* operatorNode;
	scene::IAnimatedMeshSceneNode* operativeNode;

	// add the operator and the operative
	void resetOperator();
	void resetOperative();

};

#endif