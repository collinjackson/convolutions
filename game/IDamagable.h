// Class for things that can be animated
//

#ifndef __I_DAMAGABLE_H_INCLUDED__
#define __I_DAMAGABLE_H_INCLUDED__

#include <irrlicht.h>

using namespace irr;

class IDamagable
{
public:

	virtual void damage(core::triangle3df where, f32 howMuch) = 0;

	virtual core::aabbox3d<f32> getBoundingBox() = 0;
	virtual core::vector3d<f32> getPosition() = 0;

};

#endif