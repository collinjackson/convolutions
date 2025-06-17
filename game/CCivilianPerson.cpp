// Concrete Civilian class

#include <assert.h>
#include "CCivilianPerson.h"
#include "IScenario.h"
#include <stdio.h>

CCivilianPerson::CCivilianPerson(IScenario* scenario, IPerson *targetPerson, char* modelFilename, char* textureFilename)
	: IPerson(scenario), recompute(0), epsilon(200), target(targetPerson), panic(false)
{ 

	active = false;
	IrrlichtDevice *device = scenario->getDevice();
	scene::ISceneManager* sm = device->getSceneManager();
	scene::IAnimatedMesh* mesh = 0;
	if(modelFilename) mesh = sm->getMesh(modelFilename);
	if(mesh) model = sm->addAnimatedMeshSceneNode(mesh);
	if (model)
	{
		core::vector3df position = targetPerson->getPosition();
		position.X += 200;
		position.Z += 200;
		model->setPosition(position);
		model->setScale(core::vector3df(0.5f,0.5f,0.5f));
		if(textureFilename) model->setMaterialTexture(0, device->getVideoDriver()->getTexture("media/models/civi/civi.pcx"));
		model->setMaterialFlag(video::EMF_LIGHTING, false);  // causes player to ignore surrounding lighting (!)
	}
	startPhysics();

}



void CCivilianPerson::animate(u32 now){

	if(panic && find(target->getPosition())){
		core::vector3df tpos = target->getPosition();
		core::vector3df pos = getPosition();
		face(pos + pos - tpos);
		sprint(1);
	} else {
		core::vector3df pos = getPosition();
		if(recompute <= 0){ 
			face(core::vector3df(pos.X + (rand() % 200) - 100, pos.Y + (rand() % 200) - 100, pos.Z + (rand() % 200) - 100));
			recompute = 30;
		}
		recompute--;
		sprint(1);
	}
	IPerson::animate(now);
}

void CCivilianPerson::die()
{
	IPerson::die();
	scene::ISceneNodeAnimator* anim = scenario->getDevice()->getSceneManager()->createDeleteAnimator(1000);
	model->addAnimator(anim);
	anim->drop();

	delete this;
}

void CCivilianPerson::setPanic(bool panic){
	this->panic = panic;
}