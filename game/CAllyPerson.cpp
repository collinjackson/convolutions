// Concrete Ally class

#include <assert.h>
#include "CAllyPerson.h"
#include "IScenario.h"

CAllyPerson::CAllyPerson(IScenario* scenario, IPerson *targetPerson, char* modelFilename, char* textureFilename)
	: IPerson(scenario), recompute(0), epsilon(200), target(targetPerson)
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
		position.X += 100;
		position.Z += 100;
		model->setPosition(position);
		model->setScale(core::vector3df(0.5f,0.5f,0.5f));
		if(textureFilename) model->setMaterialTexture(0, device->getVideoDriver()->getTexture(textureFilename));
		model->setMaterialFlag(video::EMF_LIGHTING, false);  // causes player to ignore surrounding lighting (!)
	}

	ai = new CFollowerAI(this, target);
	ai->setHostility(false);
	startPhysics();

}

void CAllyPerson::animate(u32 now){
	ai->animate(now);
	IPerson::animate(now);
}

void CAllyPerson::die()
{
	IPerson::die();
	scene::ISceneNodeAnimator* anim = scenario->getDevice()->getSceneManager()->createDeleteAnimator(1000);
	model->addAnimator(anim);
	anim->drop();
	delete this;
}
