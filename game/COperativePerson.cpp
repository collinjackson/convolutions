// Concrete operative class

#include "COperativePerson.h"
#include "IScenario.h"
#include "CMainMenu.h"
#include <assert.h>

COperativePerson::COperativePerson(IScenario* scenario, SKeyMap* km)
	: IPerson(scenario), KeyMap(km) 
{ 
	char* modelFilename = scenario->getOperativeModelFilename();
	char* textureFilename = scenario->getOperativeTextureFilename();

	defaultForwardSpeed = 0.3f;
	defaultBackwardSpeed = 0.3f;
	defaultLeftSpeed = 0.3f;
	defaultRightSpeed = 0.3f;
	defaultUpSpeed = 2.0f;
	refireTime = 50;
	health = 1.0f;
	
	IrrlichtDevice *device = scenario->getDevice();
	scene::ISceneManager* sm = device->getSceneManager();
	scene::IAnimatedMesh* mesh = 0;
	if(modelFilename) mesh = sm->getMesh(modelFilename);
	if(mesh) model = sm->addAnimatedMeshSceneNode(mesh);
	if (model)
	{
		model->setScale(core::vector3df(0.5f,0.5f,0.5f));
		if(textureFilename) model->setMaterialTexture(0, device->getVideoDriver()->getTexture(textureFilename));
		model->setMaterialFlag(video::EMF_LIGHTING, false);  // causes player to ignore surrounding lighting (!)
		faceAwayFromCamera();
	}


	startPhysics();
	allKeysUp(); 	
}

void COperativePerson::faceAwayFromCamera()
{
	scene::ICameraSceneNode* camera = scenario->getDevice()->getSceneManager()->getActiveCamera();		
	if(camera)
	{
		face(camera->getPosition());
		aboutFace();
	}
}

bool COperativePerson::OnEvent(SEvent event)
{
	if (event.EventType == EET_KEY_INPUT_EVENT )
	{
		s32 cnt = MAX_KEY_MAPPINGS;
		for (s32 i=0; i<cnt; ++i)
			if (KeyMap[i].KeyCode == event.KeyInput.Key)
			{
				CursorKeys[KeyMap[i].Action] = event.KeyInput.PressedDown; 
				jump(CursorKeys[EKA_JUMP]);
				if(!collider->isFalling() || !CursorKeys[EKA_FOCUS]) scenario->setBulletTime(CursorKeys[EKA_FOCUS]);
				if(!scenario->getBulletTime())
				{
					sprint(CursorKeys[EKA_MOVE_FORWARD] - CursorKeys[EKA_MOVE_BACKWARD]);
					strafe(CursorKeys[EKA_STRAFE_RIGHT] - CursorKeys[EKA_STRAFE_LEFT]);
				}
				return true;
			}
	}
	else
	if (event.EventType == EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN)
	{
		shoot();
		shooting = true;
	} 
	else
	if (event.EventType == EET_MOUSE_INPUT_EVENT &&
		event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP)
	{
		shooting = false;
	}

	return false;
}

void COperativePerson::allKeysUp()
{
	for (s32 i=0; i<EKA_COUNT; ++i)
		CursorKeys[i] = false;
}

void COperativePerson::animate(u32 now)
{
	if(scenario->getBulletTime())
	{
		gravitate(0.01f);
	}
	else
	{
		gravitate(1.0f);
		scene::ICameraSceneNode* camera = scenario->getDevice()->getSceneManager()->getActiveCamera();		
		faceAwayFromCamera();
	}
	IPerson::animate(now);
}

void COperativePerson::die()
{
	if(scenario->getScene() == IScenario::SCENE_MAIN) 
	{
		scenario->switchToScene(IScenario::SCENE_DEFEAT);
		IPerson::die();
	}
}

void COperativePerson::damage(core::triangle3df where, f32 howMuch)
{ 
	//	OutputStreamPtr damageSound = OpenSound(scenario->audiereDevice.get(), "media\\sound\\bulletbody.wav", false);
	scenario->damageSound->play();
	IPerson::damage(where, howMuch);
}
