// Concrete Civilian class
//

#ifndef __C_Civilian_PERSON_H_INCLUDED__
#define __C_Civilian_PERSON_H_INCLUDED__

#define MAXPATH 100*1*100

#include "IPerson.h"
#include "ITriangleSelector.h"
#include "CFollowerAI.h"

using namespace irr;

class IScenario;

class CCivilianPerson : public IPerson
{
public:

	// Instantiate the Civilian in a scenario that attracts to a scene node
	CCivilianPerson::CCivilianPerson(IScenario* scenario, IPerson *targetPerson, 
		char* modelFilename = "media/models/sydney/sydney.md2", 
		char* textureFilename = "media/models/sydney/sydney.bmp");
	~CCivilianPerson() {   }
	
	//set face, move "toward" target, call iperson::animate
	void animate(u32 now);

	void die();

//private:
	f32 CCivilianPerson::getHeuristic(core::vector3di start);
	f64 CCivilianPerson::getCubeDistance(core::vector3di start, core::vector3di end);
	struct node* CCivilianPerson::getNode(core::vector3di p);
	double epsilon;
	

public:
	SKeyMap* KeyMap;
	IPerson *target;
	scene::ITriangleSelector* selector; 
	int recompute;
	bool active;
	bool panic;
	void setPanic(bool panic);
};

#endif