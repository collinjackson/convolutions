// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __I_CITY_SCENARIO_H_INCLUDED__
#define __I_CITY_SCENARIO_H_INCLUDED__

#include "IScenario.h"

using namespace irr;

class ICityScenario : public IScenario
{
public:

	ICityScenario(CStartupMenu* startupMenu);

	~ICityScenario() {}

	// This is an abstract class; it cannot be instantiated
	virtual scenario_t getScenarioID() = 0;

	// Returns true if the user wants to go back to the startup menu
	// bool run();

	void loadSceneData();

protected:
		
	scene::ISceneNode* skyboxNode;
	scene::IAnimatedMeshSceneNode* model1;

};

#endif