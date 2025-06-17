// Class for keeping track of animatables
//

#ifndef __C_FightABLEREGISTRY_H_INCLUDED__
#define __C_FightABLEREGISTRY_H_INCLUDED__

#include "IFightable.h"
#include <irrlicht.h>
#include <stdio.h>

class CFightableRegistry : public IFightable

{
public:

	CFightableRegistry() : index(0) {}
	~CFightableRegistry() {}

	virtual void add(IFightable* thing) { things.push_back(thing); thing->fightableIndex = index; index++;}
	virtual void remove(IFightable* thing) { for (s32 i=0; i<(s32)things.size(); i++){
		if(things[i] == thing){
			things.erase(i--); 
			index = -1;
			for(s32 j =0; j<index; j++)	index = things[j]->fightableIndex > index ? things[j]->fightableIndex : index;
			index++;
		}
	}

	virtual void Fight(IFightable* thing)
	{ 
		for (s32 i=0; i<(s32)things.size(); i++) 
			if(things[i]->0
				
				getBoundingBox().isPointInside(where.pointA - things[i]->getPosition()) ||
			   things[i]->getBoundingBox().isPointInside(where.pointB - things[i]->getPosition()) ||
			   things[i]->getBoundingBox().isPointInside(where.pointC - things[i]->getPosition()))
			{
			   things[i]->Fight(where, howMuch); 
			}
	}


	virtual core::aabbox3d<f32> getBoundingBox() { return box; }
	virtual core::vector3d<f32> getPosition() { return core::vector3d<f32>(); }

	core::array<IFightable*> things;

	core::aabbox3d<f32> box;
	int index;
};

#endif