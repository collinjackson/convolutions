// Abstract person class header
//

#ifndef __C_TANGIBLEREGISTRY_H_INCLUDED__
#define __C_TANGIBLEREGISTRY_H_INCLUDED__

#include <irrlicht.h>

using namespace irr;
using namespace scene;

class CTangibleRegistry
{
public:

	// Person needs a scenario to live in
	CTangibleRegistry() {}
	~CTangibleRegistry() {}

	// Register a selector to receive immediate notification of all other
	// existing selectors and eventual notification of future created selectors
	virtual void add(ITriangleSelector* me = 0, IMetaTriangleSelector* world = 0)
	{
		// Order is very important here; please don't move this around unless you're sure
		if(me) for (s32 i=0; i<(s32)worlds.size(); i++)	worlds[i]->addTriangleSelector(me);
		if(world) for (s32 i=0; i<(s32)objects.size(); ++i) world->addTriangleSelector(objects[i]);
		if(me) { objects.push_back(me); }
		if(world) { worlds.push_back(world); }
	}

	virtual void removeThing(ITriangleSelector* doomed)
	{
		if(doomed) 
		{
			for(s32 i=0; i<(s32)worlds.size(); i++) worlds[i]->removeTriangleSelector(doomed);
			for(i=0; i<(s32)objects.size(); ++i) if(objects[i] == doomed) objects.erase(i--);
		}		
	}

	virtual void removeWorld(IMetaTriangleSelector* doomed)
	{
		if(doomed) 
		{
			for (s32 i=0; i<(s32)worlds.size(); i++) if(worlds[i] == doomed) worlds.erase(i--);
		}
	}

	core::array<ITriangleSelector*> objects;
	core::array<IMetaTriangleSelector*> worlds;

};

#endif