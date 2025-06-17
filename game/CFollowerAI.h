// Concrete Drone class
//

#ifndef __C_FOLLOWERAI_PERSON_H_INCLUDED__
#define __C_FOLLOWERAI_PERSON_H_INCLUDED__

#define MAXPATH 100*1*100
#define MAXENEMIES 10

#include "IPerson.h"
#include "ITriangleSelector.h"

using namespace irr;

class IScenario;

struct step {
	core::vector3df position;
	u32 scent;
};

class CFollowerAI {

public:

	// Instantiate the Drone in a scenario that attracts to a scene node
	CFollowerAI(IPerson *sourcePerson, IPerson *targetPerson) :  threshold(10000), pathDecayThreshold (5000), mergePathThreshold(0), hostility(true), numEnemies(0){
		this->threshold = threshold;
		source = sourcePerson;
		target = targetPerson;
		recompute =0;
		epsilon = 6;
		numSteps = 0;
		currentStep = 0;
	}
	~CFollowerAI() {}
	
	//set face, move "toward" target, call iperson::animate
	void animate(u32 now);
	void addEnemy(IPerson *enemy);
	void setHostility(bool hostility) { this->hostility = hostility ; } 
	

public:
	double epsilon;
	IPerson *source;
	IPerson *target;
	scene::ITriangleSelector* selector; 
	int recompute;
	bool active;
	f32 threshold;
	struct step path[MAXPATH];
	int currentStep;
	int numSteps;
	int facingIndex;
	
	bool hostility;
	int numEnemies;
	IPerson *enemies[MAXENEMIES];
	

private:
	core::vector3df init;
	u32 pathDecayThreshold;  //maximum time diff for path to remain active
	f32 mergePathThreshold;  //if path node to be added is within this radius of an existing node,
							 //overwrite the existing node
};

#endif