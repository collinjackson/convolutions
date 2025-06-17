// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#include "CDemo.h"
#include <stdio.h>

CDemo::CDemo(bool f, bool m, bool s, bool a, video::EDriverType d)
: fullscreen(f), driverType(d), currentScene(-2),
 model1(0), model2(0), music(m), 
shadows(s), quakeLevelMesh(0), quakeLevelNode(0), timeForThisScene(0),
skyboxNode(0), mapSelector(0), metaSelector(0), campFire(0), device(0),
additive(a)
{
}


CDemo::~CDemo()
{
	if (mapSelector)
		mapSelector->drop();

	if (metaSelector)
		metaSelector->drop();
}


void CDemo::run()
{
	device = createDevice(driverType,
		core::dimension2d<s32>(640, 480), 16, fullscreen, shadows, this);
		//core::dimension2d<s32>(512, 350), 16, fullscreen, shadows, this);

	device->getFileSystem()->addZipFileArchive("irrlicht.dat");
	device->getFileSystem()->addZipFileArchive("..\\..\\media\\irrlicht.dat");
	device->getFileSystem()->addZipFileArchive("map-20kdm2.pk3");
	device->getFileSystem()->addZipFileArchive("..\\..\\media\\map-20kdm2.pk3");

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	device->setWindowCaption(L"Irrlicht Engine Techdemo");

	wchar_t tmp[255];

	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			// load next scene if necessary
			u32 now = device->getTimer()->getTime();
			if (now - sceneStartTime > timeForThisScene && timeForThisScene!=-1)
				switchToNextScene();

			createParticleImpacts();

			// draw everything

			driver->beginScene(true, true, backColor);

			smgr->drawAll();
			guienv->drawAll();

			driver->endScene();

			// write statistics

			swprintf(tmp, 255, L"%s fps:%d polys:%d", driver->getName(),
				driver->getFPS(), driver->getPrimitiveCountDrawn());

			statusText->setText(tmp);
		}
	}

	device->drop();
}


bool CDemo::OnEvent(SEvent event)
{
	if (!device)
		return false;

	if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_ESCAPE &&
		event.KeyInput.PressedDown == false)
	{
		// user wants to quit.
		if (currentScene < 3)
			timeForThisScene = 0;
		else
			device->closeDevice();
	}
	else
	if ((event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_SPACE &&
		event.KeyInput.PressedDown == false) ||
		(event.EventType == EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP) &&
		currentScene == 3)
	{
		// shoot 
		shoot();
	}
	else
#ifdef _DEBUG
	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
	{
		char tmp[255];
		core::vector3df pos = device->getSceneManager()->getActiveCamera()->getAbsolutePosition();
		sprintf(tmp, "points.push_back(core::vector3df(%ff, %ff, %ff)); // %d\n", pos.X, pos.Y, pos.Z,
			sceneStartTime - device->getTimer()->getTime());
		OutputDebugString(tmp);
		return true;
	}
	else
#endif
	if (device->getSceneManager()->getActiveCamera())
	{
		device->getSceneManager()->getActiveCamera()->OnEvent(event);
		return true;
	}

	return false;
}


void CDemo::switchToNextScene()
{
	currentScene++;
	if (currentScene > 3)
		currentScene = 1;

	scene::ISceneManager* sm = device->getSceneManager();
	scene::ISceneNodeAnimator* sa = 0;
	scene::ICameraSceneNode* camera = 0;

	camera = sm->getActiveCamera();
	if (camera)
	{
		sm->setActiveCamera(0);
		camera->remove();
	}

	switch(currentScene)
	{
	case -1: // loading screen
		timeForThisScene = 0;
		createLoadingScreen();
		break;

	case 0: // load scene
		timeForThisScene = 0;
		loadSceneData();
		//currentScene += 2;
		break;

	case 1: // panorama camera
		{
			#if 0
			camera = sm->addCameraSceneNodeFPS(0, 100, 500);
			timeForThisScene = 14000000;
			#else
			timeForThisScene = 39664;
			currentScene += 1;
			camera = sm->addCameraSceneNode(0, core::vector3df(0,0,0), core::vector3df(-586,708,52));
			camera->setTarget(core::vector3df(0,400,0));
			
			core::array<core::vector3df> points;
		
			points.push_back(core::vector3df(1114.336792f, 242.206009f, -56.976807f));
			points.push_back(core::vector3df(1116.545166f, 378.685272f, -56.083183f));
			points.push_back(core::vector3df(1118.940186f, 516.664307f, -56.338856f));
			points.push_back(core::vector3df(1130.007935f, 662.745605f, -56.157753f));
			points.push_back(core::vector3df(1154.303345f, 814.303101f, -57.683624f));
			points.push_back(core::vector3df(1007.227173f, 757.994019f, 226.902283f));
			points.push_back(core::vector3df(913.370850f, 667.597168f, 469.352020f));

			points.push_back(core::vector3df(804.647400f, 625.486145f, 637.515320f));
			points.push_back(core::vector3df(799.796814f, 625.486145f, 971.678223f));
			points.push_back(core::vector3df(576.089050f, 573.689880f, 1024.815430f));
			points.push_back(core::vector3df(492.829620f, 545.776917f, 940.990051f));
			points.push_back(core::vector3df(502.472015f, 563.155029f, 811.918396f));
			points.push_back(core::vector3df(456.350525f, 545.089233f, 587.798828f));
			points.push_back(core::vector3df(477.665680f, 545.019470f, 350.156158f));
			points.push_back(core::vector3df(462.408508f, 534.190247f, 55.671730f));
			points.push_back(core::vector3df(455.176117f, 548.591370f, -195.685745f));
			points.push_back(core::vector3df(445.013641f, 561.275391f, -481.564636f));
			points.push_back(core::vector3df(470.327087f, 554.699463f, -914.740601f));
			points.push_back(core::vector3df(215.299149f, 478.205139f, -890.992554f));
			points.push_back(core::vector3df(40.725445f, 406.419464f, -856.153564f));
			points.push_back(core::vector3df(-139.619537f, 367.475952f, -832.361145f));
			points.push_back(core::vector3df(-372.134460f, 353.153320f, -718.145630f));
			points.push_back(core::vector3df(-396.479126f, 399.665192f, -464.819946f));
			points.push_back(core::vector3df(-401.636963f, 400.320068f, -85.747185f));
			points.push_back(core::vector3df(-399.067566f, 405.603760f, 78.287315f));
			points.push_back(core::vector3df(-142.541534f, 390.756897f, 108.415192f));
			points.push_back(core::vector3df(110.264053f, 388.120453f, 91.955513f));
			points.push_back(core::vector3df(358.585236f, 392.355316f, 98.712135f));
			points.push_back(core::vector3df(573.278687f, 396.884125f, 109.258797f));
			points.push_back(core::vector3df(631.992310f, 419.328979f, 281.179108f));
			points.push_back(core::vector3df(607.874084f, 413.811523f, 535.857849f));
			points.push_back(core::vector3df(614.861389f, 400.083984f, 751.252869f));
			points.push_back(core::vector3df(557.560303f, 341.482758f, 924.299133f));
			points.push_back(core::vector3df(430.859222f, 331.369476f, 985.138367f));
			points.push_back(core::vector3df(263.945251f, 215.380280f, 968.150269f));
			points.push_back(core::vector3df(202.371033f, 213.545090f, 943.107300f));
			points.push_back(core::vector3df(114.756012f, 199.799759f, 805.014160f));
			points.push_back(core::vector3df(96.783348f, 181.639481f, 648.188110f));
			points.push_back(core::vector3df(97.865623f, 138.905975f, 484.812561f));
			points.push_back(core::vector3df(99.612457f, 102.463669f, 347.603210f));
			points.push_back(core::vector3df(99.612457f, 102.463669f, 347.603210f));
			points.push_back(core::vector3df(99.612457f, 102.463669f, 347.603210f));
			
			sa = sm->createFollowSplineAnimator(device->getTimer()->getTime(), 
				points);
			camera->addAnimator(sa);
			sa->drop();			
			#endif
		model1->setVisible(false);
		model2->setVisible(false);
		campFire->setVisible(false);
		inOutFader->fadeIn(7000);
	
		break;
		}
	case 2:	// down fly anim camera
		camera = sm->addCameraSceneNode(0, core::vector3df(100,40,-80), core::vector3df(844,670,-885));
		sa = sm->createFlyStraightAnimator(	core::vector3df(94, 1002, 127),
			core::vector3df(108, 15, -60), 10000, true);
		camera->addAnimator(sa);
		timeForThisScene = 9900;
		model1->setVisible(true);
		model2->setVisible(false);		
		campFire->setVisible(false);
		sa->drop();
		break;

	case 3: // interactive, go around
		{
			model1->setVisible(true);
			model2->setVisible(true);
			campFire->setVisible(true);
			timeForThisScene = -1;

			SKeyMap keyMap[8];
			keyMap[0].Action = EKA_MOVE_FORWARD;
			keyMap[0].KeyCode = KEY_UP;
			keyMap[1].Action = EKA_MOVE_FORWARD;
			keyMap[1].KeyCode = KEY_KEY_W;
	        
			keyMap[2].Action = EKA_MOVE_BACKWARD;
			keyMap[2].KeyCode = KEY_DOWN;
			keyMap[3].Action = EKA_MOVE_BACKWARD;
			keyMap[3].KeyCode = KEY_KEY_S;
	        
			keyMap[4].Action = EKA_STRAFE_LEFT;
			keyMap[4].KeyCode = KEY_LEFT;
			keyMap[5].Action = EKA_STRAFE_LEFT;
			keyMap[5].KeyCode = KEY_KEY_A;
	        
			keyMap[6].Action = EKA_STRAFE_RIGHT;
			keyMap[6].KeyCode = KEY_RIGHT;
			keyMap[7].Action = EKA_STRAFE_RIGHT;
			keyMap[7].KeyCode = KEY_KEY_D;

			camera = sm->addCameraSceneNodeFPS(0, 100.0f, 300.0f, -1, keyMap, 8);
			camera->setPosition(core::vector3df(108,140,-140));
			
			scene::ISceneNodeAnimatorCollisionResponse* collider = 
				sm->createCollisionResponseAnimator(
				metaSelector, camera, core::vector3df(30,50,30), 
				core::vector3df(0, quakeLevelMesh ? -100.0f : 0.0f,0), 100.0f,
					core::vector3df(0,40,0), 0.0005f);
		    
			camera->addAnimator(collider);
			collider->drop();
		}
		break;
	}

	sceneStartTime = device->getTimer()->getTime();

	// if we've got a new created camera, we call OnPostRender to let all animators
	// set the right position of the camera, otherwise the camera would
	// be at a wrong position in the first frame
	if (device->getSceneManager()->getActiveCamera())
		device->getSceneManager()->getActiveCamera()->OnPostRender(sceneStartTime);
}


void CDemo::loadSceneData()
{
	// load quake level

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* sm = device->getSceneManager();

	quakeLevelMesh = sm->getMesh("20kdm2.bsp");
	
	if (quakeLevelMesh)
	{
		quakeLevelNode = sm->addOctTreeSceneNode(quakeLevelMesh->getMesh(0));
		if (quakeLevelNode)
		{
			quakeLevelNode->setPosition(core::vector3df(-1300,-70,-1249));
			quakeLevelNode->setVisible(true);
			
			// create map triangle selector
			mapSelector = sm->createOctTreeTriangleSelector(quakeLevelMesh->getMesh(0),
				quakeLevelNode, 128);

			// set additive blending if wanted
			if (additive)
				quakeLevelNode->setMaterialType(video::EMT_LIGHTMAP_ADD);
		}
	}

	// load sydney model and create 2 instances

	scene::IAnimatedMesh* mesh = 0;
	mesh = sm->getMesh("..\\..\\media\\sydney.md2");
	if (mesh)
	{
		model1 = sm->addAnimatedMeshSceneNode(mesh);
		if (model1)
		{
			model1->setMaterialTexture(0, driver->getTexture("..\\..\\media\\spheremap.jpg"));
			model1->setPosition(core::vector3df(100,40,-80));
			model1->setScale(core::vector3df(2,2,2));
			model1->setMD2Animation(scene::EMAT_SALUTE);
			model1->setMaterialFlag(video::EMF_LIGHTING, false);
			model1->setMaterialType(video::EMT_SPHERE_MAP);
			model1->addShadowVolumeSceneNode();
		}

		model2 = sm->addAnimatedMeshSceneNode(mesh);
		if (model2)
		{
			model2->setPosition(core::vector3df(180,15,-60));
			model2->setScale(core::vector3df(2,2,2));
			model2->setMD2Animation(scene::EMAT_RUN);
			model2->setAnimationSpeed(20);
			model2->setMaterialTexture(0, device->getVideoDriver()->getTexture("../../media/sydney.BMP"));
			model2->setMaterialFlag(video::EMF_LIGHTING, true);
			model2->addShadowVolumeSceneNode();
		}
	}

	scene::ISceneNodeAnimator* anim = 0;

	// create sky box

	skyboxNode = sm->addSkyBoxSceneNode(
		driver->getTexture("..\\..\\media\\irrlicht2_up.bmp"),
		driver->getTexture("..\\..\\media\\irrlicht2_dn.bmp"),
		driver->getTexture("..\\..\\media\\irrlicht2_lf.bmp"),
		driver->getTexture("..\\..\\media\\irrlicht2_rt.bmp"),
		driver->getTexture("..\\..\\media\\irrlicht2_ft.bmp"),
		driver->getTexture("..\\..\\media\\irrlicht2_bk.bmp"));

	// create walk-between-portals animation

	core::vector3df waypoint[2];
	waypoint[0].set(-150,40,100);
	waypoint[1].set(350,40,100);

	if (model2)
	{
		anim = device->getSceneManager()->createFlyStraightAnimator(waypoint[0], 
			waypoint[1], 10000, true);
		model2->addAnimator(anim);
		anim->drop();
	}

	// create animation for portals;

	core::array<video::ITexture*> textures;
	for (s32 g=1; g<8; ++g)
	{
		char tmp[64];
		sprintf(tmp, "..\\..\\media\\portal%d.bmp", g);
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
		bill->setMaterialTexture(0, driver->getTexture("..\\..\\media\\portal1.bmp"));
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
	bill->setMaterialTexture(0, driver->getTexture("..\\..\\media\\particlewhite.bmp"));
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

	// create meta triangle selector with all triangles selectors in it.
	metaSelector = sm->createMetaTriangleSelector();
	metaSelector->addTriangleSelector(mapSelector);

	// create camp fire

	campFire = sm->addParticleSystemSceneNode(false, 0, -1, 
		core::vector3df(100,120,600), core::vector3df(0,0,0),
		core::vector3df(2,2,2));

	campFire->setParticleSize(core::dimension2d<f32>(20.0f, 10.0f));

	scene::IParticleEmitter* em = campFire->createBoxEmitter(
		core::aabbox3d<f32>(-7,0,-7,7,1,7), 
		core::vector3df(0.0f,0.03f,0.0f),
		80,100, video::SColor(0,255,255,255),video::SColor(0,255,255,255), 800,2000);

	campFire->setEmitter(em);
	em->drop();

	scene::IParticleAffector* paf = campFire->createFadeOutParticleAffector();
	campFire->addAffector(paf);
	paf->drop();

	campFire->setMaterialFlag(video::EMF_LIGHTING, false);
	campFire->setMaterialTexture(0, driver->getTexture("..\\..\\media\\fireball.bmp"));
	campFire->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

	// load music

	#ifdef USE_AUDIERE
	if (music)
		startAudiere();
	#endif

	// set background color

	backColor.set(0,0,0,0);
}



void CDemo::createLoadingScreen()
{
	core::dimension2d<int> size = device->getVideoDriver()->getScreenSize();

	device->getCursorControl()->setVisible(false);

	// setup loading screen

	backColor.set(255,90,90,156);

	// create in fader

	inOutFader = device->getGUIEnvironment()->addInOutFader();
	inOutFader->setColor(backColor);

	// irrlicht logo

	gui::IGUIImage* img = device->getGUIEnvironment()->addImage(
		core::rect<int>(10,10,98,41));

	img->setImage(
		device->getVideoDriver()->getTexture("..\\..\\media\\irrlichtlogoaligned.jpg"));

	// loading text

	const int lwidth = 170;
	const int lheight = 15;

	core::rect<int> pos(10, size.Height-lheight-10, 10+lwidth, size.Height-10);
	
	device->getGUIEnvironment()->addImage(pos);
	statusText = device->getGUIEnvironment()->addStaticText(L"Loading...",	pos, true);
	statusText->setOverrideColor(video::SColor(255,205,200,200));

	// load bigger font

	device->getGUIEnvironment()->getSkin()->setFont(
		device->getGUIEnvironment()->getFont("..\\..\\media\\fonthaettenschweiler.bmp"));

	// set new font color

	device->getGUIEnvironment()->getSkin()->setColor(gui::EGDC_BUTTON_TEXT,
		video::SColor(255,100,100,100));
}



void CDemo::shoot()
{
	scene::ISceneManager* sm = device->getSceneManager();
	scene::ICameraSceneNode* camera = sm->getActiveCamera();

	if (!camera || !mapSelector)
		return;

	SParticleImpact imp; 
	imp.when = 0;

	// get line of camera

	core::vector3df start = camera->getPosition();
	core::vector3df end = (camera->getTarget() - start);
	end.normalize();
	end = start + (end * camera->getFarValue());

	core::triangle3df triangle;

	core::line3d<f32> line(start, end);

	// get intersection point with map

	if (sm->getSceneCollisionManager()->getCollisionPoint(
		line, mapSelector, end, triangle))
	{
		// collides with wall
		core::vector3df out = triangle.getNormal();
		out.setLength(0.03f);

		imp.when = 1;
		imp.outVector = out;
		imp.pos = end;
	}
	else
	{
		// doesnt collide with wall
		end = (camera->getTarget() - start);
		end.normalize();
		end = start + (end * 1000);
	}

	// create fire ball
	scene::ISceneNode* node = 0;
	node = sm->addBillboardSceneNode(0,
		core::dimension2d<f32>(25,25), start);

	node->setMaterialFlag(video::EMF_LIGHTING, false);
	node->setMaterialTexture(0, device->getVideoDriver()->getTexture("..\\..\\media\\particle.bmp"));
	node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		
	f32 length = (f32)(end - start).getLength();
	const f32 speed = 0.3f;
	u32 time = (u32)(length / speed);

	scene::ISceneNodeAnimator* anim = 0;

	// set flight line

	anim = sm->createFlyStraightAnimator(start, end, time);
	node->addAnimator(anim);	
	anim->drop();

	anim = sm->createDeleteAnimator(time);
	node->addAnimator(anim);
	anim->drop();

	if (imp.when)
	{
		// create impact note
		imp.when = device->getTimer()->getTime() + (time - 100);
		Impacts.push_back(imp);
	}

	// play sound
	#ifdef USE_AUDIERE
	if (ballSound)
		ballSound->play();
	#endif
}



void CDemo::createParticleImpacts()
{
	u32 now = device->getTimer()->getTime();
	scene::ISceneManager* sm = device->getSceneManager();

	for (s32 i=0; i<(s32)Impacts.size(); ++i)
		if (now > Impacts[i].when)
		{
			// create smoke particle system
			scene::IParticleSystemSceneNode* pas = 0;

			pas = sm->addParticleSystemSceneNode(false, 0, -1, Impacts[i].pos);

			pas->setParticleSize(core::dimension2d<f32>(10.0f, 10.0f));

			scene::IParticleEmitter* em = pas->createBoxEmitter(
				core::aabbox3d<f32>(-5,-5,-5,5,5,5),
				Impacts[i].outVector, 20,40, video::SColor(0,255,255,255),video::SColor(0,255,255,255),
				1200,1600, 20);

			pas->setEmitter(em);
			em->drop();

			scene::IParticleAffector* paf = campFire->createFadeOutParticleAffector();
			pas->addAffector(paf);
			paf->drop();

			pas->setMaterialFlag(video::EMF_LIGHTING, false);
			pas->setMaterialTexture(0, device->getVideoDriver()->getTexture("../../media/smoke.bmp"));
			pas->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

			scene::ISceneNodeAnimator* anim = sm->createDeleteAnimator(2000);
			pas->addAnimator(anim);
			anim->drop();

			// delete entry
			Impacts.erase(i);
			i--;			

			// play impact sound
			#ifdef USE_AUDIERE
			if (impactSound)
				impactSound->play();
			#endif
		}
}




#ifdef USE_AUDIERE
void CDemo::startAudiere()
{
	audiereDevice = OpenDevice();
	if (!audiereDevice) 
		return;

	stream = OpenSound(audiereDevice.get(), "IrrlichtTheme.mp3", true);
	if (!stream) 
	{
		stream = OpenSound(audiereDevice.get(), "..\\..\\media\\IrrlichtTheme.mp3", true);
		if (!stream)
			return;
	}

	ballSound = OpenSound(audiereDevice.get(), "ball.wav", false);
	if (!ballSound) 
	{
		ballSound = OpenSound(audiereDevice.get(), "..\\..\\media\\ball.wav", false);
	}

	impactSound = OpenSound(audiereDevice.get(), "impact.wav", false);
	if (!impactSound) 
	{
		impactSound = OpenSound(audiereDevice.get(), "..\\..\\media\\impact.wav", false);
	}
		
	stream->setRepeat(true);
	//stream->setVolume(0.5f); // 50% volume
	stream->play();
}
#endif

