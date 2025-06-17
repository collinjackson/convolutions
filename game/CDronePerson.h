// Concrete Drone class
//

#ifndef __C_Drone_PERSON_H_INCLUDED__
#define __C_Drone_PERSON_H_INCLUDED__

#define MAXPATH 100*1*100

#include "IPerson.h"
#include "ITriangleSelector.h"
#include "CFollowerAI.h"

using namespace irr;

class IScenario;


/*
struct node {
	core::vector3df position;
	f32 g;  //cost so far
	f32 h;  //predicted cost
	f32 f;  //g+h
	
	bool blocked;
	int hierarchy;
	struct node *parent;
};


struct step {
	core::vector3df position;
	u32 scent;
};*/



class CDronePerson : public IPerson
{
public:

	// Instantiate the Drone in a scenario that attracts to a scene node
	CDronePerson::CDronePerson(IScenario* scenario, IPerson *targetPerson, 
		char* modelFilename = "media/models/sydney/sydney.md2", 
		char* textureFilename = "media/models/sydney/sydney.bmp");
	~CDronePerson() {  delete ai; }
	
	//set face, move "toward" target, call iperson::animate
	void animate(u32 now);

	void die();

//private:
	f32 CDronePerson::getHeuristic(core::vector3di start);
	f64 CDronePerson::getCubeDistance(core::vector3di start, core::vector3di end);
	struct node* CDronePerson::getNode(core::vector3di p);
	double epsilon;
	

public:
	SKeyMap* KeyMap;
	
	CFollowerAI *ai;
	IPerson *target;
	scene::ITriangleSelector* selector; 

	int recompute;

	//core::vector3di cubeNodes;
	//core::vector3df cubeSize;
	bool active;

};

#endif