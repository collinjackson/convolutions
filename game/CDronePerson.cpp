// Concrete Drone class

#include <assert.h>
#include "CDronePerson.h"
#include "IScenario.h"

CDronePerson::CDronePerson(IScenario* scenario, IPerson *targetPerson, char* modelFilename, char* textureFilename)
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
	startPhysics();

}

/*struct node* CDronePerson::getNode(core::vector3di p){
	return &nodes[p.X][p.Y][p.Z];
}*/


//gets heuristic distance between cube-space coordinates and target
//note that present "heuristic" is to calculate abs distance  (stupid but serviceable)
/*f32 CDronePerson::getHeuristic(core::vector3di start){
	core::vector3df pos = core::vector3df(init.X+start.X*cubeSize.X,init.Y+start.Y*cubeSize.Y,init.Z+start.Z*cubeSize.Z);
	f32 distance = pos.getDistanceFrom(target->getPosition());
	return distance;
}*/

//similar to getHeuristic, but between two cube-space coordinates
/*f64 CDronePerson::getCubeDistance(core::vector3di start, core::vector3di end){
	core::vector3df pos1 = core::vector3df(init.X+start.X*cubeSize.X,init.Y+start.Y*cubeSize.Y,init.Z+start.Z*cubeSize.Z);
	core::vector3df pos2 = core::vector3df(init.X+end.X*cubeSize.X,init.Y+end.Y*cubeSize.Y,init.Z+end.Z*cubeSize.Z);
	f64 distance = pos1.getDistanceFrom(pos2);
	return distance;
}*/


void CDronePerson::animate(u32 now){

	// Simple version for now

	
/*	face(target->getPosition());
	sprint(0.1f);*/


//	cubeNodes = core::vector3di(100, 1, 100);
//	cubeSize = core::vector3df(10, 1, 10);



	ai->animate(now);
	IPerson::animate(now);
	//model->setPosition(target->getPosition());
}

void CDronePerson::die()
{
	IPerson::die();
	scene::ISceneNodeAnimator* anim = scenario->getDevice()->getSceneManager()->createDeleteAnimator(1000);
	model->addAnimator(anim);
	anim->drop();

	delete this;
}
