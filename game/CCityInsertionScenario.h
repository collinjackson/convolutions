// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __C_CITY_INSERTION_SCENARIO_H_INCLUDED__
#define __C_CITY_INSERTION_SCENARIO_H_INCLUDED__

#include "ICityScenario.h"
#include "CVictoryTriggerLocation.h"

using namespace irr;

class CCityInsertionScenario : public ICityScenario
{
public:

	CCityInsertionScenario(CStartupMenu* startupMenu) 
		: ICityScenario(startupMenu), victory(0) 
	{
	  introAnimStartPos = core::vector3df(-1323.506348f, 6000.356323f, 6000.068115f);
      introAnimEndPos = core::vector3df(-1323.506348f, -1273.356689f, -2899.068115f); 
	  introAnimationTime = 1000;
	}

	~CCityInsertionScenario() 
	{
		delete victory;
	}

	virtual scenario_t getScenarioID() { return SCENARIO_CITY_INSERTION; }

	virtual void loadSceneData()
	{
	addResultImage("media/text/insertion_text.jpg", 1000);

		ICityScenario::loadSceneData();
	
		// You win if you go over to the hobgoblin
//		model1->setPosition(core::vector3df(-1000, -1250, 1000));
		core::aabbox3d<f32> victorybox(core::vector3df(		-2100.067017f, -10000.362915f, -750.148865f
), core::vector3df(		-1800.067017f, 10000.362915f, -550.148865f
)); // = model1->getBoundingBox();
		victory	= new CVictoryTriggerLocation(this, operative->getModel(), victorybox);
	}

protected:

	CVictoryTriggerLocation* victory;
};

#endif