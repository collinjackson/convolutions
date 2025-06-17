// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __I_SCENARIO_H_INCLUDED__
#define __I_SCENARIO_H_INCLUDED__

// Doesn't seem to be necessary.  -- cej9
// #include <windows.h>
#include "irrlicht.h"
#include "IPerson.h"
#include "CTangibleRegistry.h"
#include "CAnimatableRegistry.h"
#include "CDamagableRegistry.h"

#include <audiere.h> // your compiler throws an error here? get audiere from 
                     // http://audiere.sourceforge.net/ or uncomment 
                     // the define USE_AUDIERE above.
using namespace audiere;
#pragma comment (lib, "audiere.lib")

using namespace irr;
class CMainMenu;
class CStartupMenu;
class stereogram::Stereogram;

class IScenario : public IEventReceiver
{
public:

	IScenario(CStartupMenu* startupMenu);

	~IScenario();

	// Returns true if the user wants to go back to the startup menu
	virtual bool run();

	virtual bool OnEvent(SEvent event);

	enum scenario_t
	{
		SCENARIO_CITY_INSERTION,// see CCityInsertionScenario.h
//		SCENARIO_EMPTY,			// this file
//		SCENARIO_CASTLE,		// see CCastleScenario.h
		SCENARIO_MAZE,
		SCENARIO_CITY_ESCAPE,   // see CCityEscapeScenario.h
		SCENARIO_COUNT  // keep this at the end
	};

	enum scene_t
	{
		SCENE_START,
		SCENE_LOADING_SCREEN,
		SCENE_LOAD,
		SCENE_INTRO,
		SCENE_MAIN,
		SCENE_VICTORY,
		SCENE_VICTORY_END,
		SCENE_DEFEAT,
		SCENE_DEFEAT_END,
		SCENE_COUNT
	};

	virtual scenario_t getScenarioID() = 0;

	static wchar_t* getScenarioName(scenario_t which);
	static IScenario* createScenario(scenario_t which, CStartupMenu *startupMenu);

	// Create a bullet somewhere in the scene along the specified path
	bool fireBullet(core::line3d<f32> path, IMetaTriangleSelector* world);

protected:

	IrrlichtDevice *device;
	video::SColor backColor;
	IPerson* operative;
	core::vector3df introAnimStartPos, introAnimEndPos;
	u32 introAnimationTime;

	// Representative selectors that will record registry entries
	scene::IMetaTriangleSelector* movableTangibleSelector;
	scene::IMetaTriangleSelector* stationaryTangibleSelector;
	CTangibleRegistry movableTangibleRegistry;
	CTangibleRegistry stationaryTangibleRegistry;
	CAnimatableRegistry animatableRegistry;
	CDamagableRegistry damagableRegistry;
	
	virtual void loadSceneData();
	virtual void addMap(char *name, core::vector3df position, bool fog = false);

	void makeHRotX(irr::core::matrix4 m, double thetaX );
	void makeHRotY(irr::core::matrix4 *m, double thetaY );

private:

	char* operativeModelFilename;
	char* operativeTextureFilename;

	f32 savedSpeed;

	void createLoadingScreen();
	void createParticleImpacts();
	void doPhysicsStuff();
	
	CStartupMenu* startupMenu;
	video::EDriverType driverType;
	
	f32 jumping;
	f32 jumpingDown;

	f32 oldGrav;
//	scene::ISceneNodeAnimatorCollisionResponse* modelCollider;
	scene::ISceneNodeAnimatorCollisionResponse* firstCollider;

	f32 freeOverRadius;

	//double oldx;
	//double oldy;

public:
	
	// To move along the game
	void switchToNextScene();
	void switchToScene(scene_t scene);
	scene_t getScene() { return currentScene; }

	// Add a static image to the screen at end of mission
	void addResultImage(char* filename, s32 time = -1);
	video::ITexture* resultImage;
	s32 resultImageTime;

	// Ideally these would have their own class
	void startAudiere();
	AudioDevicePtr audiereDevice;
	OutputStreamPtr stream;
	OutputStreamPtr ballSound;
	OutputStreamPtr impactSound;
	OutputStreamPtr damageSound;

private:
	scene_t currentScene;

	CMainMenu* mainMenu;

	gui::IGUIStaticText* statusText;
	gui::IGUIInOutFader* inOutFader;


#define MAX_KEY_MAPPINGS 10
	SKeyMap keyMap[MAX_KEY_MAPPINGS];

	scene::ICameraSceneNode* chaseCamera;
	scene::ICameraSceneNode* overCamera;
	scene::ICameraSceneNode* freeOverCamera;
	scene::ICameraSceneNode* firstCamera;

	// for bullet impacts
	scene::IParticleSystemSceneNode* smokeParticleSystem;

	u32 sceneStartTime;
	u32 timeForThisScene;

	// Fader for the fly-in animation
	scene::ISkyBoxFaderSceneNode* skyboxFader;

	stereogram::Stereogram *stereogram;

	bool isBulletTime;

public:

	// A few accessors for the main menu to use
	OutputStreamPtr getStream() { return stream; }
	IrrlichtDevice *getDevice() { return device; }
	stereogram::Stereogram *getStereogram() { return stereogram; }

	// And a few for the scene members to use
	char* getOperativeModelFilename();
	char* getOperativeTextureFilename();
	CTangibleRegistry* getStationaryTangibleRegistry() { return &stationaryTangibleRegistry; }
	CTangibleRegistry* getMovableTangibleRegistry() { return &movableTangibleRegistry; }
	CDamagableRegistry* getDamagableRegistry() { return &damagableRegistry; }
	CAnimatableRegistry* getAnimatableRegistry() { return &animatableRegistry; }
	CMainMenu* getMainMenu() { return mainMenu; }
	ITriangleSelector* getStationaryTangibleSelector() { return stationaryTangibleSelector; }
	ITriangleSelector* getMovableTangibleSelector() { return movableTangibleSelector; }
	scene::IParticleSystemSceneNode* getSmokeParticleSystem() { return smokeParticleSystem; }

	void setBulletTime(bool value) { isBulletTime = value; }
	bool getBulletTime() { return isBulletTime; }
};

#endif