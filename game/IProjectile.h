// Abstract Projectile class header
//

#ifndef __I_PROJECTILE_H_INCLUDED__
#define __I_PROJECTILE_H_INCLUDED__

#include "IAnimatable.h"
#include <irrlicht.h>

using namespace irr;
using namespace scene;

class IScenario;

class IProjectile : public IAnimatable
{
public:

	// Projectile needs a scenario to live in
	IProjectile(IScenario* s, core::line3d<f32> path, ITriangleSelector* targets); 

	~IProjectile();

	// Continue to speed along to the destination
	virtual void animate(u32 now);

	// Start the bullet moving. You should just pass in "now" for the parameter
	virtual void fire(u32 when);

	// Delete the bullet and damage any adjacent entity
	virtual void hit(core::triangle3df where);

	// Delete the bullet and do no damage
	virtual void wallhit();

protected:

	struct SParticleImpact
	{
		bool exists;
		core::vector3df pos;
		core::vector3df outVector;
	} imp;

	virtual void createParticleImpact(SParticleImpact imp);

	// Determine the proper position for the projectile at some time
	core::vector3df interpolate(u32 when);

	// The scenario the Projectile is living in
	IScenario* scenario;

	// And here is the current speed of this Projectile
	f32 currentForwardSpeed;

	// And how far it can go
	f32 range;

	// And how much damage it does
	f32 damage;

	// The Projectile remembers when it was fired
	s32 firstUpdateTime;

	// The Projectile remembers the last time update() was called 
	s32 lastUpdateTime;

	// The Projectile will die after this time
	s32 finalUpdateTime;

	// The max path of this Projectile
	core::line3d<f32> maxPath;

	// The starting point of this Projectile
	core::vector3df startPos;

	// The node associated with this Projectile
	scene::ISceneNode* projectileNode;

	// The normalized direction vector of this Projectile
	core::vector3df direction;

	// The selector of movable objects that can be hit
	scene::ITriangleSelector* movableObjects;

};

#endif