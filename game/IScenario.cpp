// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#include "IScenario.h"
#include "ICityScenario.h"
#include "CCastleScenario.h"
#include "CCityEscapeScenario.h"
#include "CCityInsertionScenario.h"
#include "CMazeScenario.h"
#include "stereogram.h"
#include "COperativePerson.h"
#include "CDronePerson.h"
#include <assert.h>

#include <stdio.h>
#include <matrix4.h> 
#include "CStartupMenu.h"
#include "CMainMenu.h"
#include "ISkyBoxFaderSceneNode.h"

IScenario::IScenario(CStartupMenu* startupMenu)
	: startupMenu(startupMenu),
	  currentScene(SCENE_START),
	  operative(0), 
	  skyboxFader(0), timeForThisScene(0),
	  movableTangibleSelector(0), stationaryTangibleSelector(0),
	  device(0),
	  firstCollider(0), 
	  introAnimationTime(0),   // off by default
	  jumping(0), isBulletTime(false),
	  resultImage(0), resultImageTime(-1)
{
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

	keyMap[8].Action = EKA_JUMP;
	keyMap[8].KeyCode = KEY_SPACE;
	keyMap[9].Action = EKA_FOCUS;
	keyMap[9].KeyCode = KEY_SHIFT;
}

char* IScenario::getOperativeModelFilename() 
{ 
	return startupMenu->getModelFilename(startupMenu->currentOperativeModel); 
}

char* IScenario::getOperativeTextureFilename() 
{ 
	return startupMenu->getTextureFilename(startupMenu->currentOperativeModel); 
}

IScenario* IScenario::createScenario(scenario_t which, CStartupMenu* startupMenu)
{ 
	switch(which)
	{
	case SCENARIO_CITY_INSERTION: return new CCityInsertionScenario(startupMenu);
//	case SCENARIO_EMPTY: return new IScenario(startupMenu);
//	case SCENARIO_CASTLE: return new CCastleScenario(startupMenu);
	case SCENARIO_MAZE: return new CMazeScenario(startupMenu);
	case SCENARIO_CITY_ESCAPE: return new CCityEscapeScenario(startupMenu);
	default: return 0;
	}
}

wchar_t* IScenario::getScenarioName(IScenario::scenario_t which)
{
	switch(which)
	{
	case IScenario::SCENARIO_CITY_INSERTION: return L"City Insertion";
//	case IScenario::SCENARIO_EMPTY: return L"Empty";
//	case IScenario::SCENARIO_CASTLE: return L"Castle";
	case IScenario::SCENARIO_MAZE: return L"Office Maze";
	case IScenario::SCENARIO_CITY_ESCAPE: return L"City Escape";
	default: return L"";
	}
}

IScenario::~IScenario()
{
//	if (movableTangibleSelector)
//		movableTangibleSelector->drop();
//	if (stationaryTangibleSelector)
//		stationaryTangibleSelector->drop();
}


void IScenario::makeHRotX(irr::core::matrix4 m, double thetaX ) {
	f32 cosT = cosf((f32)thetaX);
	f32 sinT = sinf((f32)thetaX);

	m.M[0] = 1.0; m.M[1] =  0.0; m.M[2] =   0.0; m.M[3] = 0.0;
	m.M[4] = 0.0; m.M[5] = cosT; m.M[6] = -sinT; m.M[7] = 0.0;
	m.M[8] = 0.0; m.M[9] = sinT; m.M[10] =  cosT; m.M[11] = 0.0;
	m.M[12] = 0.0; m.M[13] =  0.0; m.M[14] =   0.0; m.M[15] = 1.0;
}

void IScenario::makeHRotY(irr::core::matrix4 *m, double thetaX ) {
	f32 cosT = cosf((f32)thetaX);
	f32 sinT = sinf((f32)thetaX);

	m->M[0] = cosT; m->M[1] =  0.0; m->M[2] =   sinT; m->M[3] = 0.0;
	m->M[4] = 0.0; m->M[5] = 1; m->M[6] = 0.0; m->M[7] = 0.0;
	m->M[8] = -sinT; m->M[9] = 0; m->M[10] =  cosT; m->M[11] = 0.0;
	m->M[12] = 0.0; m->M[13] =  0.0; m->M[14] =   0.0; m->M[15] = 1.0;
}



bool IScenario::run()
{
	device = createDevice(video::EDT_OPENGL,
		core::dimension2d<s32>(640, 480), 16, startupMenu->gameFullscreen, startupMenu->shadows, this);

	scene::ISceneManager *sm = device->getSceneManager();
	
	device->getFileSystem()->addZipFileArchive("irrlicht.dat");
	device->getFileSystem()->addZipFileArchive("media\\irrlicht.dat");
	
	device->getFileSystem()->addZipFileArchive("demo_map.pk3");
	device->getFileSystem()->addZipFileArchive("media\\demo_map.pk3");

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();

	device->setWindowCaption(L"Construct");
	
	stereogram = driver->createStereogram();

	// load music
	if (startupMenu->music) startAudiere();

	// initialize main menu
	mainMenu = new CMainMenu(this);
		
	wchar_t tmp[511];

	while(device->run() && driver)
	{

		if (device->isWindowActive())
		{
			// load next scene if necessary
			u32 now = device->getTimer()->getTime();
			if (now - sceneStartTime > timeForThisScene && timeForThisScene!=-1)
				switchToNextScene();

			// Something random Jim added... locks the overhead view, but does so poorly	
			if(device->getSceneManager()->getActiveCamera()==freeOverCamera && freeOverCamera){	
				core::vector3df nPosition = this->freeOverCamera->getPosition();
				nPosition.Y = freeOverRadius;
				this->freeOverCamera->setPosition(nPosition);
			}

			// This does most of the hard work of managing the scene objects
			if(currentScene >= SCENE_MAIN) animatableRegistry.animate(now);
			else if(operative) operative->animate(now);

			// if(stationaryTangibleSelector) stationaryTangibleSelector->isEmpty(operative->getModel()->getBoundingBox());

			// What does this do?
			/*
			if(camera==overCamera){
				camera->View.M[5] = 0;camera->View.M[6] = -1;
				camera->View.M[9] = 1;camera->View.M[10] = 0;
			}*/
	
			// create fog
			// Note: Material property fog must also be set
			driver->setFog(video::SColor(0,255,255,255), true, 100.0f, 2000.0f, 0.002f, true, false);
			
			driver->beginScene(true, true, backColor);

			smgr->drawAll();
			stereogram->draw();
			guienv->drawAll();

			if(resultImage) 
			{
				core::dimension2d<int> size = device->getVideoDriver()->getScreenSize();
				const int lwidth = 363; // resultImage->getSize().Width;
				const int lheight = 104; // resultImage->getSize().Height;
				s32 moveUp;
				if(resultImageTime < 0) moveUp = size.Height/4-lheight/2;
				else
				{
					resultImageTime -= 5;
					if(resultImageTime < 400) moveUp = size.Height/4-lheight/2 - 400 + resultImageTime;
					else moveUp = size.Height/4-lheight/2;
					if(resultImageTime < 0) resultImage = 0;
				}
				core::position2d<s32> pos(size.Width/2-lwidth/2,moveUp);

				driver->draw2DImage(resultImage, 
								pos, core::rect<s32>(0,0,lwidth,lheight), 0,
								video::SColor(255, 255, 255, 255),
								true);
			}

			driver->endScene();

			// write statistics

			swprintf(tmp, 511, L"%s fps:%d polys:%d health: %d", 
				driver->getName(),
				driver->getFPS(), 
				driver->getPrimitiveCountDrawn(), 
				operative ? (int)(operative->getHealth() * 100) : 0);

			statusText->setText(tmp);
		}
	}

	device->drop();

	return mainMenu->getReturnToStartupMenu();
}


bool IScenario::OnEvent(SEvent event)
{
	if (!device)
		return false;

	
	// Toggle running animation; also pass input on to camera
	/*
	if (event.EventType == EET_KEY_INPUT_EVENT)
	{
		switch(event.KeyInput.Key)
		{
		case KEY_KEY_W: case KEY_KEY_A: case KEY_KEY_S: case KEY_KEY_D:
		case KEY_UP: case KEY_DOWN: case KEY_LEFT: case KEY_RIGHT: 
			if(event.KeyInput.PressedDown && !isRunning)
			{
				isRunning = true;
				operativeModel->setMD2Animation(scene::EMAT_RUN);
			} 
			else 
			if(!event.KeyInput.PressedDown && isRunning)
			{
				isRunning = false;
				operativeModel->setMD2Animation(scene::EMAT_STAND);
			}
			break;
		}
	}
	*/

	scene::ICameraSceneNode* camera = device->getSceneManager()->getActiveCamera();		
	
	if(operative && operative->OnEvent(event)) return true;
	else
	if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_ESCAPE &&
		event.KeyInput.PressedDown == false)
	{
		mainMenu->show();
		// user wants to quit.
		//		if (currentScene < 3)
		//			timeForThisScene = 0;
		//		else
		//			device->closeDevice();
	}
	else
/*	if (event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_SPACE &&
		event.KeyInput.PressedDown == true) 
	{
		operative->jump(1);
	} 
	else */
	if ((event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_F1 &&
		event.KeyInput.PressedDown == false) )
	{
		irr::core::matrix4 m = camera->getProjectionMatrix();
		m.setRotationDegrees(irr::core::vector3df(1,1,1));
		core::vector3df nPosition = camera->getPosition();
		//makeHRotY(&m, 3.14);
		//nPosition.X = 0;
		nPosition.Y = 0;
		nPosition.Z = 0;
		//camera->setTarget(nPosition);
		camera->setMouseInputReceiverEnabled(true);
		camera->setKeyInputReceiverEnabled(true);
		//CCameraFPSSceneNode) camera->setRotateSpeed(savedSpeed);
		//camera->setAspectRatio(camera->getAspectRatio() / 2);
		
	}
	else if ((event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_F2 &&
		event.KeyInput.PressedDown == false) )
	{

		//allow only one successive call!
		//irr::core::matrix4 m = camera->getProjectionMatrix();
		//m.setRotationDegrees(irr::core::vector3df(1,1,1));
		core::vector3df nPosition = camera->getPosition();
		//makeHRotY(&m, 3.14);
		//nPosition.X = 0;
		core::vector3df n;
		n.X = nPosition.X + 10;
		n.Y = nPosition.Y + 40;
		n.Z = nPosition.Z;
		
		nPosition.X = nPosition.X - 10;
		nPosition.Y = nPosition.Y - 40;
		camera->setPosition(nPosition);

// commented out for chase test 		camera->setTarget(n);		
		
		//camera->setKeyInputReceiverEnabled(false);
		camera->setMouseInputReceiverEnabled(false);
	}
	else if ((event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_F3 &&
		event.KeyInput.PressedDown == false) )
	{
//		operativeModel->setVisible(true);
		device->getSceneManager()->setActiveCamera(this->chaseCamera);
	}
	else if ((event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_F4 &&
		event.KeyInput.PressedDown == false) )
	{
//		operativeModel->setVisible(true);
		device->getSceneManager()->setActiveCamera(this->overCamera);
		core::vector3df nPosition = this->overCamera->getPosition();
		camera = this->overCamera;

		// core::matrix4 m = camera->getProjectionMatrix();
		// camera->setTarget(core::vector3df(2000,2000000,2000));
		// camera->setTarget(operativeModel->getPosition());

//		nPosition = operativeModel->getPosition();
		this->overCamera->setMouseInputReceiverEnabled(false);

	}
	else if ((event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_F5 &&
		event.KeyInput.PressedDown == false) )
	{
//		operativeModel->setVisible(false);
		device->getSceneManager()->setActiveCamera(this->firstCamera);	
		camera->setMouseInputReceiverEnabled(true);
//		this->firstCamera->setPosition(operativeModel->getPosition());
	}
	else if ((event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_F6 &&
		event.KeyInput.PressedDown == false) )
	{
//		operativeModel->setVisible(true);
		device->getSceneManager()->setActiveCamera(this->freeOverCamera);
		core::vector3df nPosition = this->freeOverCamera->getPosition();
		camera = this->freeOverCamera;

		core::matrix4 m = camera->getProjectionMatrix();
		camera->setTarget(core::vector3df(2000,2000000,2000));
		
//		nPosition = operativeModel->getPosition();
		
		freeOverRadius = 500;
//		camera->setPosition(nPosition);
		this->overCamera->setMouseInputReceiverEnabled(false);
		
	}
	else if ((event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_KEY_O))
	{
		freeOverRadius += 30;
	}
	else if ((event.EventType == EET_KEY_INPUT_EVENT &&
		event.KeyInput.Key == KEY_KEY_L) )
	{
		freeOverRadius -= 30;
		freeOverRadius = freeOverRadius < 10 ? 10 : freeOverRadius;
	}
	else
#ifdef AVERYLONGIDENTIFIERTHATJIMTERRYISNOTVERYLIKELYTOUSE
		// _DEBUG   -- I'm not sure what this is, but it's causing compilation problems and is annoying
	
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

void IScenario::switchToNextScene()
{
	currentScene = (scene_t)((s32)currentScene + 1);
	if (currentScene == SCENE_COUNT) return;
	switchToScene(currentScene);
}

void IScenario::switchToScene(scene_t scene)
{
	currentScene = scene;

	scene::ISceneManager* sm = device->getSceneManager();
	scene::ISceneNodeAnimator* sa = 0;

	// Can use this to remove cameras
	// if (camera)
	// {
	// 	sm->setActiveCamera(0);
	// 	camera->remove();
	// }

	switch(currentScene)
	{
	case SCENE_LOADING_SCREEN: // loading screen
		timeForThisScene = 0;
		createLoadingScreen();
		break;

	case SCENE_LOAD: // load scene
		timeForThisScene = 0;
		loadSceneData();
		//currentScene += 2;
		break;

	case SCENE_INTRO: // construct appears
		{
			timeForThisScene = introAnimationTime;
			
			// Fade in the model
			inOutFader->fadeIn(1000);

			// Add various camera views

			chaseCamera = sm->addCameraSceneNodeChase(); // 0, 100.0f, 0.1f, -1);
			operative->addNephew(chaseCamera);
			chaseCamera->setTarget(operative->getModel()->getPosition()); // Position(core::vector3df(0,50,-50));
			
			firstCamera = sm->addCameraSceneNodeFPS(0, 100.0f, 300.0f, -1, keyMap, 8);
			operative->addNephew(firstCamera);
			firstCamera->setPosition(core::vector3df(25,50,0));

			overCamera = sm->addCameraSceneNodeChaseOver(0, 100.0f, 300.0f, -1, keyMap, 8);
			operative->addNephew(firstCamera);

			freeOverCamera = sm->addCameraSceneNodeFPS(0, 100.0f, 300.0f, -1, keyMap, 8);
			operative->addNephew(freeOverCamera);

			sm->setActiveCamera(chaseCamera);

			// Start the intro fly-in sequence
			operative->setPosition(introAnimStartPos);
			scene::ISceneNodeAnimator* anim = sm->createFlyStraightAnimator(introAnimStartPos, introAnimEndPos, introAnimationTime, false);
			operative->getModel()->addAnimator(anim);
			anim->drop();
		}
		break;
		
	case SCENE_MAIN: // main game
		{

			timeForThisScene = -1;

			// Intro sequence is finished, so it's safe to add lighting, physics and sky
			//operativeModel->setMaterialFlag(video::EMF_LIGHTING, true);
			skyboxFader->fadeIn((u32)3000);
			core::vector3df translation = core::vector3df(0,0,0);
			core::vector3df  gravity = core::vector3df(0,-.5,0);
			core::aabbox3d<f32> box = operative->getBoundingBox();
			core::vector3df radius = box.MaxEdge - box.getCenter();
			radius.X += 20;
			radius.Y += 22;
			radius.Z += 20;  //tweak for animating		
//			firstCollider = sm->createCollisionResponseAnimator(metaSelector, this->firstCamera, radius, gravity, 100.0f,translation, 0.0005f);
			radius.X = radius.Y = radius.Z = 50;
			gravity = core::vector3df(0,0,0);			
//			scene::ISceneNodeAnimatorCollisionResponse* collider2 = 
//				sm->createCollisionResponseAnimator(metaSelector, chaseCamera, radius, gravity, 100.0f,translation, 0.0005f);
//			scene::ISceneNodeAnimatorCollisionResponse* collider3 = 
//				sm->createCollisionResponseAnimator(metaSelector, overCamera, radius, gravity, 100.0f,translation, 0.0005f);
//			scene::ISceneNodeAnimatorCollisionResponse* collider4 = 
//				sm->createCollisionResponseAnimator(metaSelector, freeOverCamera, radius, gravity, 100.0f,translation, 0.0005f);

//			this->chaseCamera->addAnimator(collider2);		

//			this->firstCamera->addAnimator(firstCollider);		
//			this->overCamera->addAnimator(collider3);
//			this->freeOverCamera->addAnimator(collider4);

//			collider2->drop();
		} break;
	case SCENE_VICTORY: 
		{
			timeForThisScene = 2000; 
			inOutFader->fadeOut(2000);	
			addResultImage("media/text/victory_text.jpg");

			// gui::IGUIStaticText* victoryText = 
			// 	device->getGUIEnvironment()->addStaticText(L"VICTORY", pos, false);
			// victoryText->setOverrideColor(video::SColor(255,0,100,0));
		
		} break;
	case SCENE_VICTORY_END:
		{
			// Go to the next scenario!
			if((s32)getScenarioID() + 1 < (s32)SCENARIO_COUNT)
			{
				startupMenu->currentScenario = (scenario_t)((s32)getScenarioID() + 1);
				startupMenu->skip = true;
			}
			else 
			{
				startupMenu->currentScenario = (scenario_t)0;
				startupMenu->skip = false;
			}
			mainMenu->resign();
		} break;
	case SCENE_DEFEAT: 
		{
			timeForThisScene = 2000; 
			inOutFader->fadeOut(2000);	
			addResultImage("media/text/failure_text.jpg");
		} break; 
	case SCENE_DEFEAT_END:
		{
			startupMenu->skip = true;
			mainMenu->resign();
		} break;

	}

	sceneStartTime = device->getTimer()->getTime();

	// if we've got a new created camera, we call OnPostRender to let all animators
	// set the right position of the camera, otherwise the camera would
	// be at a wrong position in the first frame
	if (device->getSceneManager()->getActiveCamera())
		device->getSceneManager()->getActiveCamera()->OnPostRender(sceneStartTime);
}

void IScenario::addMap(char *name, core::vector3df position, bool fog)
{
	scene::ISceneManager* sm = device->getSceneManager();
	IAnimatedMesh* levelMesh = sm->getMesh(name);

	if (levelMesh)
	{
		scene::ISceneNode* levelNode = sm->addOctTreeSceneNode(levelMesh->getMesh(0));
		if (levelNode)
		{
			levelNode->setPosition(position);
			levelNode->setVisible(true);
			
			// create map triangle selector
			scene::ITriangleSelector* mapSelector = sm->createOctTreeTriangleSelector(levelMesh->getMesh(0),
				levelNode, 128);

			stationaryTangibleRegistry.add(mapSelector, 0);

			// set additive blending if wanted
			if (startupMenu->additive)
				levelNode->setMaterialType(video::EMT_LIGHTMAP_ADD);
			
			if(startupMenu->fog && fog) levelNode->setMaterialFlag(video::EMF_FOG_ENABLE, true);
		}
	}
}

void IScenario::loadSceneData()
{
	// the concrete scene implementation is responsible for initializing levelMesh
	// from a quake map or whatever

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* sm = device->getSceneManager();

	skyboxFader = sm->addSkyBoxFaderSceneNode();
	skyboxFader->setColor(video::SColor(255,255,255,255));
	skyboxFader->fadeOut((u32)0);		// Initially the skybox is set to white

	operative = new COperativePerson(this, keyMap);

	// register the meta triangle selector to receive object notifications
	movableTangibleSelector = sm->createMetaTriangleSelector();
	movableTangibleRegistry.add(0, movableTangibleSelector);
	
	stationaryTangibleSelector = sm->createMetaTriangleSelector();
	stationaryTangibleRegistry.add(0, stationaryTangibleSelector);
	
	// load bullet impact particle system

	smokeParticleSystem = sm->addParticleSystemSceneNode(false, 0, -1, 
		core::vector3df(100,120,600), core::vector3df(0,0,0),
		core::vector3df(2,2,2));

	smokeParticleSystem->setParticleSize(core::dimension2d<f32>(10.0f, 5.0f));

	scene::IParticleAffector* paf = smokeParticleSystem->createFadeOutParticleAffector();
	smokeParticleSystem->addAffector(paf);
	paf->drop();

	smokeParticleSystem->setMaterialFlag(video::EMF_LIGHTING, false);
	smokeParticleSystem->setMaterialTexture(0, driver->getTexture("media\\fireball.bmp"));
	smokeParticleSystem->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

	// set background color

}



void IScenario::createLoadingScreen()
{
	core::dimension2d<int> size = device->getVideoDriver()->getScreenSize();
	device->getCursorControl()->setVisible(false);

	// create in fader

	inOutFader = device->getGUIEnvironment()->addInOutFader();
	inOutFader->setColor(backColor);

	// irrlicht logo

	//gui::IGUIImage* img = device->getGUIEnvironment()->addImage(core::rect<int>(10,10,98,41));
	//img->setImage(device->getVideoDriver()->getTexture("media\\irrlichtlogoaligned.jpg"));

	// loading text

	const int lwidth = 230;
	const int lheight = 15;

	core::rect<int> pos(10, size.Height-lheight-10, 10+lwidth, size.Height-10);
	
	device->getGUIEnvironment()->addImage(pos);
	statusText = device->getGUIEnvironment()->addStaticText(L"Loading...",	pos, true);
	statusText->setOverrideColor(video::SColor(255,205,200,200));


	// load bigger font

	device->getGUIEnvironment()->getSkin()->setFont(
		device->getGUIEnvironment()->getFont("media/fonthaettenschweiler.bmp"));

	// set new font color

	device->getGUIEnvironment()->getSkin()->setColor(gui::EGDC_BUTTON_TEXT,
		video::SColor(255,100,100,100));
}


void IScenario::startAudiere()
{
	audiereDevice = OpenDevice();
	if (!audiereDevice) 
		return;

	stream = OpenSound(audiereDevice.get(), "IrrlichtTheme.mp3", true);
	if (!stream) 
	{
		stream = OpenSound(audiereDevice.get(), "media\\music\\IrrlichtTheme.mp3", true);
		if (!stream)
			return;
	}

	ballSound = OpenSound(audiereDevice.get(), "media\\sound\\44.wav", false);
	impactSound = OpenSound(audiereDevice.get(), "media\\sound\\smash.wav", false);
	damageSound = OpenSound(audiereDevice.get(), "media\\sound\\bulletbody.wav", false);
	
	stream->setRepeat(true);
	stream->setVolume(0.5f); // 50% volume
	stream->play();
}

void IScenario::addResultImage(char* filename, s32 time)
{
	resultImageTime = time;
	resultImage = device->getVideoDriver()->getTexture(filename);
	// device->getVideoDriver()->makeColorKeyTexture(resultImage, core::position2d<s32>(1,1));
}

