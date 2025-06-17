// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __I_TRIGGER_LOCATION_H_INCLUDED__
#define __I_TRIGGER_LOCATION_H_INCLUDED__

#include "IAnimatable.h"
#include <assert.h>
#include <irrlicht.h>

class ITriggerLocation : public IAnimatable
{
public:

	ITriggerLocation(IScenario *s, ISceneNode* what, core::aabbox3d<f32>trig)
		: scenario(s), thing(what), location(trig) 
	{
		scenario->getAnimatableRegistry()->add(this);
	}

	~ITriggerLocation() 
	{
		scenario->getAnimatableRegistry()->remove(this);
	}

	virtual void animate(unsigned int now) 
	{ 
		if(location.isPointInside(thing->getPosition())) trigger(now); 
	}

	virtual void trigger(unsigned int now) = 0;

	core::aabbox3d<f32>location;
	ISceneNode* thing;
	IScenario* scenario;

};

#endif