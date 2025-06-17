// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __C_MAIN_MENU_H_INCLUDED__
#define __C_MAIN_MENU_H_INCLUDED__

#include <irrlicht.h>
#include <audiere.h> 

using namespace irr;
using namespace audiere;

class GlyphSet;
class stereogram::Stereogram;
class IScenario;

class CMainMenu : public IEventReceiver
{
public:

	CMainMenu(IScenario* scenario);

	virtual bool OnEvent(SEvent event);

	// Main menu takes control over the device for a temporary time
	virtual void show();

	// Restore settings, allowing the game to continue
	virtual void hide();

	// Go back to the startup menu
	virtual void resign();

	// Quit game
	virtual void exit();

private:

	// IDs of the various menu elements
	enum
	{
		MAINMENU_CONTEXT_MENU = 0,
		MAINMENU_MUSIC_VOLUME,
		MAINMENU_EXIT_BUTTON,
		MAINMENU_RESIGN_BUTTON,
		MAINMENU_RESUME_BUTTON,
		MAINMENU_GRAVITY,
		MAINMENU_GLYPH_SET,
		MAINMENU_SHOW_GLYPHS,
		MAINMENU_STREAM_DOWN,
		MAINMENU_HANDICAP_MODE,
		MAINMENU_FLIP_DEPTH,
		MAINMENU_AUTO_FOCUS,
		MAINMENU_FOCUS_DEPTH,
		MAINMENU_BRIGHTNESS,
		MAINMENU_BRIGHTNESS_SMOOTHING,
	};

	// IDs of the various glyphs that the menu loads into the stereogram
	enum
	{
		GLYPH_SIZE_5,
		GLYPH_SIZE_7,
		GLYPH_SIZE_9,
		GLYPH_SIZE_14,
		GLYPH_SIZE_NUM		// Make sure this stays at the end
	};

	void setTransparency();

	IrrlichtDevice *device;
	stereogram::Stereogram* stereogram;
	GlyphSet* glyphSets[GLYPH_SIZE_NUM];
	s32 selected;
//	bool start;
//	bool fullscreen;
//	bool music;
//	bool shadows;
//	bool additive;
	bool transparent;

	// If the device suddenly closes we will check this to decide whether to continue
	bool returnToStartupMenu;

	// Operator parameters
	int glyphSet;
	bool showGlyphs;
	bool handicapMode;
	bool flipDepth;
	bool streamDown;
	bool autoFocus;
	int brightness;
	int brightnessSmoothing;
	int focusDepth;
	
	// Audiere sound pointer
	OutputStreamPtr stream;
	int musicVolume;

	// Construct parameters
	int gravity;

	// Menu objects
	gui::IGUITabControl* tabctrl;
	scene::ICameraSceneNode* menuCamera;
	bool visible;

	// Saved objects
	IEventReceiver *savedReceiver;
	scene::ICameraSceneNode* savedCamera;	
	bool savedCursorVisibility;


public:
	bool isVisible() { return visible; }
	bool getReturnToStartupMenu() { return returnToStartupMenu; }
	bool getShowGlyphs() { return showGlyphs; }
	bool getStreamDown() { return streamDown; }
	bool getHandicapMode() { return handicapMode; }
	bool getFlipDepth() { return flipDepth; }
	bool getAutoFocus() { return autoFocus; } 
	s32 getBrightness() { return brightness; }
	s32 getBrightnessSmoothing() { return brightnessSmoothing; }
	s32 getFocusDepth() { return focusDepth; }
	f32 getGravity() { return (f32)gravity / 255.0f ; }
};

#endif