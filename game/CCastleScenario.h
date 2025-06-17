// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __C_Castle_SCENARIO_H_INCLUDED__
#define __C_Castle_SCENARIO_H_INCLUDED__

#include "IScenario.h"

using namespace irr;

class CCastleScenario : public IScenario
{
public:

	CCastleScenario(CStartupMenu* startupMenu);

	~CCastleScenario();

//	virtual scenario_t getScenarioID() { return SCENARIO_CASTLE; }

	// Returns true if the user wants to go back to the startup menu
	// bool run();

private:

	void loadSceneData();

		
	scene::ISceneNode* skyboxNode;
	scene::IAnimatedMeshSceneNode* model1;

};

#endif