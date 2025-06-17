// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __C_CITY_ESCAPE_SCENARIO_H_INCLUDED__
#define __C_CITY_ESCAPE_SCENARIO_H_INCLUDED__

#include "ICityScenario.h"
#include "CVictoryTriggerLocation.h"
#include <assert.h>

using namespace irr;

class CCityEscapeScenario : public ICityScenario
{
public:

	CCityEscapeScenario(CStartupMenu* startupMenu) : ICityScenario(startupMenu), victory(0) 
	{
	  introAnimStartPos = core::vector3df(-2155.94f, -105.35f, -1652.41f);
      introAnimEndPos = core::vector3df(-2050.94f, -105.35f, -1652.41f); 
	  introAnimationTime = 1000;
	}

	~CCityEscapeScenario() 
	{
		delete victory;
	}

	virtual scenario_t getScenarioID() { return SCENARIO_CITY_ESCAPE; }

	virtual void loadSceneData()
	{
		ICityScenario::loadSceneData();
		
		addResultImage("media/text/extraction_text.jpg", 1000);
		
		// You win if you go over to the hobgoblin
		// model1->setPosition(core::vector3df(-1000, -1250, 1000));

		core::aabbox3d<f32> victorybox(core::vector3df(-568.747f, -400.362f, -1023.442f), core::vector3df(-447.389f, 0.362f, -897.465f));
		victory	= new CVictoryTriggerLocation(this, operative->getModel(), victorybox);

	}
protected:

	CVictoryTriggerLocation* victory;

};

#endif