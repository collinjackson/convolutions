// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#include "ICityScenario.h"
#include "stereogram.h"
#include "CDronePerson.h"


#include <stdio.h>
#include <matrix4.h> 
#include "CStartupMenu.h"
#include "CMainMenu.h"
#include "ISkyBoxFaderSceneNode.h"
#include "CElevator.h"

ICityScenario::ICityScenario(CStartupMenu* startupMenu)
	: IScenario(startupMenu),
	  model1(0), skyboxNode(0)
{
  introAnimStartPos = core::vector3df(-480,20000,-60); 
  introAnimEndPos = core::vector3df(-2880,1800,160);  // core::vector3df(-480,-24,-60);
  introAnimationTime = 2000;

  backColor.set(255,255,255,255);

}


void ICityScenario::loadSceneData()
{
	// Create octtree, set up blending, skybox fader, etc.
	IScenario::loadSceneData();
	addMap("city.bsp", core::vector3df(-1300,-2000,-1249), true);

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* sm = device->getSceneManager();

	scene::IAnimatedMesh* mesh = 0;
	
	// mesh = sm->getMesh("media\\models\\hobgoblin.md2");
	//mesh = sm->getMesh(startupMenu->getModelFilename(startupMenu->getCurrentOperatorModel()));
	if (mesh)
	{
		int j = 0, i = 0; 
		//for(j = 0; j<10; j++){for(i = 0; i<10; i++){
		
		model1 = sm->addAnimatedMeshSceneNode(mesh);
		if (model1 )
			//&& i==0 && j==0)
		{
			model1->setMaterialTexture(0, driver->getTexture("media\\models\\hobgoblin.jpg"));
	//		model1->setMaterialTexture(0, driver->getTexture(startupMenu->getModelTexture(startupMenu->getCurrentOperatorModel()));
			model1->setScale(core::vector3df(2,2,2));
			model1->setMD2Animation(scene::EMAT_SALUTE);
			model1->setMaterialFlag(video::EMF_LIGHTING, false);
			model1->setMaterialType(video::EMT_SPHERE_MAP);
			model1->addShadowVolumeSceneNode();
		}
		model1->setPosition(core::vector3df(-1300, 1000, 1000));
		//}}

		//sm->getMesh("media\\sydney.md2");
	}

	scene::ISceneNodeAnimator* anim = 0;

	// create sky box
	
	ILightSceneNode* sun = sm->addLightSceneNode(0, core::vector3df(-1300,200000.0f,-1249), video::SColorf(1.0f, 1.0f, 1.0f), 300000.0f);
	sun = sm->addLightSceneNode(0, core::vector3df(-1300,100000.0f,-1249), video::SColorf(1.0f, 1.0f, 1.0f), 300000.0f);
	sun = sm->addLightSceneNode(0, core::vector3df(-1300,50000.0f,-1249), video::SColorf(1.0f, 1.0f, 1.0f), 300000.0f);
	sun = sm->addLightSceneNode(0, core::vector3df(-1000,50000.0f,-1249), video::SColorf(1.0f, 1.0f, 1.0f), 300000.0f);
	sun = sm->addLightSceneNode(0, core::vector3df(-1000,50000.0f,-2000), video::SColorf(1.0f, 1.0f, 1.0f), 300000.0f);
	sun = sm->addLightSceneNode(0, core::vector3df(-1000,50000.0f,2000), video::SColorf(1.0f, 1.0f, 1.0f), 300000.0f);

	/*
	scene::ISceneNode* skyboxNode = sm->addSkyBoxSceneNode(
		driver->getTexture("media\\skybox\\citysunset_up.bmp"),
		driver->getTexture("media\\skybox\\citysunset_dn.bmp"),
		driver->getTexture("media\\skybox\\citysunset_lf.bmp"),
		driver->getTexture("media\\skybox\\citysunset_rt.bmp"),
		driver->getTexture("media\\skybox\\citysunset_ft.bmp"),
		driver->getTexture("media\\skybox\\citysunset_bk.bmp"));
	*/
	// create walk-between-portals animation

	core::vector3df waypoint[2];
	waypoint[0].set(-150,40,100);
	waypoint[1].set(350,40,100);

	/*if (playerModel)
	{
		anim = device->getSceneManager()->createFlyStraightAnimator(waypoint[0], 
			waypoint[1], 10000, true);
		playerModel->addAnimator(anim);
		anim->drop();
	}
	*/

	// create animation for portals;

	core::array<video::ITexture*> textures;
	for (s32 g=1; g<8; ++g)
	{
		char tmp[64];
		sprintf(tmp, "media\\portal%d.bmp", g);
		video::ITexture* t = driver->getTexture(tmp);
		textures.push_back(t);
	}

	anim = sm->createTextureAnimator(textures, 100);

	// create portals

	scene::IBillboardSceneNode* bill = 0;

	for (int r=0; r<2; ++r)
	{
		bill = sm->addBillboardSceneNode(0, core::dimension2d<f32>(100,100),
			waypoint[r]+ core::vector3df(0,20,0));
		bill->setMaterialFlag(video::EMF_LIGHTING, false);
		bill->setMaterialTexture(0, driver->getTexture("media\\portal1.bmp"));
		bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		bill->addAnimator(anim);
	}

	anim->drop();

	// create cirlce flying dynamic light with transparent billboard attached

	scene::ILightSceneNode* light = 0;

	light = device->getSceneManager()->addLightSceneNode(0,
		core::vector3df(0,0,0),	video::SColorf(1.0f, 0.5f, 0.5f, 1.0f), 1500);

	anim = device->getSceneManager()->createFlyCircleAnimator(
		core::vector3df(100,150,80), 80.0f, 0.0005f);

	light->addAnimator(anim);
	anim->drop();

	bill = device->getSceneManager()->addBillboardSceneNode(
		light, core::dimension2d<f32>(40,40));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialTexture(0, driver->getTexture("media\\particlewhite.bmp"));
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

	// create camp fire

	scene::IParticleEmitter* em = getSmokeParticleSystem()->createBoxEmitter(
		core::aabbox3d<f32>(-7,0,-7,7,1,7), 
		core::vector3df(0.0f,0.03f,0.0f),
		80,100, video::SColor(0,255,255,255),video::SColor(0,255,255,255), 800,2000);

	getSmokeParticleSystem()->setEmitter(em);
	em->drop();


	// Some drones to run after us
	IPerson* drone;
	// drone = new CDronePerson(this, operative, "media/models/sydney/sydney.md2", "media/models/sydney/sydney_sunglasses.bmp");
	// drone->setPosition(core::vector3df(-2880,1000,160));
	drone = new CDronePerson(this, operative, "media/models/civi/tris.md2", "media/models/civi/civi_agent.bmp");
	drone->setPosition(core::vector3df(-2880,1000,260));
	// drone = new CDronePerson(this, operative, "media/models/sas/tris.md2", "media/models/sas/sas.pcx");
	// drone->setPosition(core::vector3df(-2880,1000,360));
	drone = new CDronePerson(this, operative, "media/models/sas/tris.md2", "media/models/sas/sas.pcx");
	drone->setPosition(core::vector3df(-2880,1000,460));
	

	//TODO: texture this
	CElevator* elevator = new CElevator(this, operative->getModel(), core::aabbox3df(999,999,999,999,999,999));
	elevator->addElevator(core::vector3df(-1969.6,-1300.25,-644.9), 1.0f, 900);
}