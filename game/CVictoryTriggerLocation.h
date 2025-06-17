// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __C_VICTORY_TRIGGER_LOCATION_H_INCLUDED__
#define __C_VICTORY_TRIGGER_LOCATION_H_INCLUDED__

#include "ITriggerLocation.h"
#include "IScenario.h"


class CVictoryTriggerLocation : public ITriggerLocation
{
public:

	CVictoryTriggerLocation(IScenario *scenario, ISceneNode* thing, core::aabbox3d<f32>trigger)
		: ITriggerLocation(scenario, thing, trigger) {}

	~CVictoryTriggerLocation() {}

	virtual void trigger(unsigned int now)
	{
		if(scenario->getScene() == IScenario::SCENE_MAIN)
			scenario->switchToScene(IScenario::SCENE_VICTORY);
	}

};

#endif