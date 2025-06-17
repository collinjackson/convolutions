// Class for keeping track of animatables
//

#ifndef __C_ANIMATABLEREGISTRY_H_INCLUDED__
#define __C_ANIMATABLEREGISTRY_H_INCLUDED__

#include "IAnimatable.h"
#include <irrlicht.h>

class CAnimatableRegistry : public IAnimatable

{
public:

	CAnimatableRegistry() {}
	~CAnimatableRegistry() {}

	virtual void add(IAnimatable* thing) { things.push_back(thing); }
	virtual void remove(IAnimatable* thing) { for (s32 i=0; i<(s32)things.size(); i++) if(things[i] == thing) things.erase(i--); } 
	virtual void animate(u32 now) { for (s32 i=0; i<(s32)things.size(); i++) things[i]->animate(now); }

	core::array<IAnimatable*> things;

};

#endif