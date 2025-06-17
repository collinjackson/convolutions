// Class for keeping track of animatables
//

#ifndef __C_DAMAGABLEREGISTRY_H_INCLUDED__
#define __C_DAMAGABLEREGISTRY_H_INCLUDED__

#include "IDamagable.h"
#include <irrlicht.h>
#include <stdio.h>

class CDamagableRegistry : public IDamagable

{
public:

	CDamagableRegistry() {}
	~CDamagableRegistry() {}

	virtual void add(IDamagable* thing) { things.push_back(thing); box.addInternalBox(thing->getBoundingBox()); }
	virtual void remove(IDamagable* thing) { for (s32 i=0; i<(s32)things.size(); i++) if(things[i] == thing) things.erase(i--); } 

	virtual void damage(core::triangle3df where, f32 howMuch)
	{ 
		for (s32 i=0; i<(s32)things.size(); i++) 
			if(things[i]->getBoundingBox().isPointInside(where.pointA - things[i]->getPosition()) ||
			   things[i]->getBoundingBox().isPointInside(where.pointB - things[i]->getPosition()) ||
			   things[i]->getBoundingBox().isPointInside(where.pointC - things[i]->getPosition()))
			{
			   things[i]->damage(where, howMuch); 
			}
	}

	virtual core::aabbox3d<f32> getBoundingBox() { return box; }
	virtual core::vector3d<f32> getPosition() { return core::vector3d<f32>(); }

	core::array<IDamagable*> things;

	core::aabbox3d<f32> box;

};

#endif