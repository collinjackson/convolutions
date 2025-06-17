// This is the current Techdemo of the Irrlicht Engine (c) 2003 by N.Gebhardt.
// This file is not documentated.

#ifndef __I_ELEVATOR_LOCATION_H_INCLUDED__
#define __I_ELEVATOR_LOCATION_H_INCLUDED__

#include "IAnimatable.h"
#include "ITriggerLocation.h"
#include <assert.h>
#include <irrlicht.h>

#define MAX_ELEVATORS 10

enum {
	BOTTOM=0,
	TOP,
	RISING,
	FALLING
}	;

class CElevator : public ITriggerLocation
{
public:


	CElevator(IScenario *scenario, ISceneNode* thing, core::aabbox3d<f32>trigger)
		: ITriggerLocation(scenario, thing, trigger) {  running = false; }

	~CElevator() {}

	void animate(unsigned int now) 
	{ 
		if(location.isPointInside(thing->getPosition()) || status==RISING || status==FALLING) {
			//assert(0);
			trigger(now); 
		}
		immobilityTimer = immobilityTimer  <= 0 ? 0 : immobilityTimer - 1;

	}


	void trigger(unsigned int now){
		
		//printf("Elevator is of status %d . . . \n",status);
		if(status == RISING || status == FALLING){
			if(status == RISING){
				if(position.Y >= target.Y){
					status = TOP;
					immobilityTimer = 20;
				} else {
					position.Y += riseRate;
					core::vector3df tpos = thing->getPosition();
					elevator->setPosition(position);
					if(location.isPointInside(thing->getPosition()))	thing->setPosition(core::vector3df(tpos.X, tpos.Y + riseRate, tpos.Z));
					
				}
			}
			else if(status == FALLING){
				if(position.Y <= init.Y){
					status = BOTTOM;
					immobilityTimer = 20;
				} else {
					position.Y -= riseRate;
					core::vector3df tpos = thing->getPosition();
					elevator->setPosition(position);
					if(location.isPointInside(thing->getPosition())) thing->setPosition(core::vector3df(tpos.X, tpos.Y - riseRate, tpos.Z));
					
				}
			}
		}
		else {
			printf("Position: %f, %f, %f",thing->getPosition().X, thing->getPosition().Y, thing->getPosition().Z);
			if(location.isPointInside(thing->getPosition()) && immobilityTimer <= 0){
				printf("Did we trip?",status);
				if(status == BOTTOM) status = RISING;
				else status = FALLING;
			}

		}
		
		location = core::aabbox3df(position.X - scale.X * 10, -99999, position.Z - scale.Z * 10, position.X + scale.X * 10, 99999, position.Z + scale.Z * 10);
	}

	void addElevator(core::vector3df pos, float riseRate, float riseDistance){
			running = true;
			this->riseRate = riseRate;
			position = pos;
			init = pos;
			target = pos;
			target.Y = target.Y  + riseDistance;

			scale = core::vector3df(5,1,5);
			
			
			location = core::aabbox3df(position.X - scale.X * 10, -99999, position.Z - scale.Z * 10, position.X + scale.X * 10, 99999, position.Z + scale.Z * 10);

			elevator = scenario->getDevice()->getSceneManager()->addTestSceneNode (10.0f, 0, -1, core::vector3df(position), core::vector3df(0, 0, 0), scale);
			elevator->setMaterialTexture(0, scenario->getDevice()->getVideoDriver()->getTexture("media\\maze_wall.jpg"));
			ITriangleSelector* selector = scenario->getDevice()->getSceneManager()->createTriangleSelectorFromBoundingBox(elevator);
			scenario->getStationaryTangibleRegistry()->add(selector, 0);
			status = BOTTOM;
			immobilityTimer = 0;
		
	}
	core::aabbox3d<f32>location;
	scene::ISceneNode* elevator;
	
	float riseRate;
	float riseDistance;
	int status;
	int immobilityTimer;
	bool running;

	core::vector3df position;
	core::vector3df init;
	core::vector3df target;
	core::vector3df scale ;
};


// ITriggerLocation(IScenario *s, ISceneNode* what, core::aabbox3d<f32>trig)
#endif
