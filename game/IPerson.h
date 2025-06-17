// Abstract person class header
//

#ifndef __I_PERSON_H_INCLUDED__
#define __I_PERSON_H_INCLUDED__

#include <irrlicht.h>
#include "IAnimatable.h"
#include "IDamagable.h"

using namespace irr;
using namespace scene;

class IScenario;

class IPerson : public IEventReceiver, public IAnimatable, public IDamagable
{
public:

	// Person needs a scenario to live in and a model and texture
	IPerson(IScenario*); 

	~IPerson();

	// By default the person ignores all events
	// Can and should be overridden by the concrete implementation
	virtual bool OnEvent(SEvent event) { return false; }

	// Rotate person to look at the desired point. The model
	// may not rotate fully (it needs to stay upright), but the
	// person class keeps track of what it's really looking at.
	virtual void face(core::vector3df what) { look(what - model->getPosition()); }

	// Return true if the person can see the desired position (using line of sight)
	virtual bool find(core::vector3df what);

	// Rotate a person to look in a desired direction
	virtual void look(core::vector3df where) 
	{ 
		f32 nRot;
		if(where.X > 0) nRot = -atanf(where.Z/where.X) * 180.0f / 3.14159f;
		else if(where.X == 0) nRot = where.Z > 0 ? 90.0f : -90.0f;
		else nRot = 180 - atanf(where.Z/where.X) * 180.0f / 3.14159f;

		model->setRotation(core::vector3df(0,nRot,0));
	}

	// Returns a normalized vector indicating the direction the player is looking at
	// Currently has no up/down component
	virtual core::vector3df getDirection()
	{
		core::vector3df forward(0,0,1);
		forward.rotateXZBy(270 - model->getRotation().Y, core::vector3df(0,0,0));
		return forward;
	}

	virtual core::vector3df getPosition() { return model->getPosition(); }

	virtual void aboutFace() { model->setRotation(core::vector3df(0,180 + model->getRotation().Y,0)); }

	// This is kind of a hack, I'd like the model to not be public
	virtual scene::IAnimatedMeshSceneNode* getModel() { return model; }

	// So that bullets etc. can collide with the person
	virtual scene::ITriangleSelector* getPersonSelector() { return personSelector; }
	virtual scene::IMetaTriangleSelector* getMovableSelector() { return movableSelector; }
	virtual scene::IMetaTriangleSelector* getMetaSelector() { return metaSelector; }

	// Instead of providing a desired point, it is possible to provide
	// a desired model. If that model moves, this one will move to face it
	virtual void face(ISceneNode* what);

	// Person attempts to fire weapon at whatever it is looking at
	// Returns true if the attempt succeeded, false if they couldn't shoot
	// for some reason
	virtual bool shoot();

	// Person attempts to jump up (or down, which might work in weird environments like water)
	// True if the jump succeeded, false if they were already falling or something
	// \params up is checked for positive, negative, or zero; actual value not used
	virtual bool jump(int up) { return jump((up>0)*defaultUpSpeed - (up<0)*defaultDownSpeed); }
	
	// Person attempts to sprint forward (or backwards, which won't be as fast)
	// True unless something went wrong
	virtual bool sprint(int forward) { return sprint((forward>0)*defaultForwardSpeed - (forward<0)*defaultBackwardSpeed); }

	// Person attempts to dextrously strafe right (or left, for the sinister types)
	// True unless something went wrong
	virtual bool strafe(int right) { return strafe((right>0)*defaultRightSpeed - (right<0)*defaultLeftSpeed); }
	
	// Cancel all sprinting and strafing. Jumping can't be reversed in this manner.
	// True unless something went wrong (stopping a jump is ignored and isn't considered wrong)
	virtual bool stop() { return sprint(0.0f) && strafe(0.0f); }

	// Sets the personal gravity for this person. 
	// The scale is a fraction of the global gravity, with 0.0f being off and 1.0f being normal
	virtual void gravitate(f32 scale);

	// This function is truly an abomination
	virtual void addNephew(ICameraSceneNode* nephew) { nephew->setUncle(model); }

	// Do basic physics stuff like jumping, walking, etc. Called during main loop.
	virtual void animate(u32 now);

	// Required by the IDamagable API. Default implementation subtracts howMuch from the health,
	// and kills off character if necessary
	virtual void damage(core::triangle3df where, f32 howMuch);

	// Kill off this person
	// Warning: Causes person to die! Use with discretion.
	virtual void die();

	// Tell us how much health you have
	f32 getHealth() { return health; }

	virtual core::vector3df bulletTimeScale() { return core::vector3df(0.0f, 0.0f, 0.0f); }

protected:

	// The scenario the person is living in
	IScenario* scenario;

	// These are the same as the public versions, but take a specific speed value
	virtual bool jump(f32 upSpeed);
	virtual bool sprint(f32 forwardSpeed);
	virtual bool strafe(f32 rightSpeed);

	// And here are the default speeds for those calls
	f32 defaultUpSpeed, defaultDownSpeed;
	f32 defaultForwardSpeed, defaultBackwardSpeed;
	f32 defaultRightSpeed, defaultLeftSpeed;

	// And here are the current speeds of this person (can be negative)
	f32 currentUpSpeed;
	f32 currentRightSpeed;
	f32 currentForwardSpeed;

	// The person remembers the last time update() was called 
	u32 lastUpdateTime;

	// Used when the player has a weapon to shoot
	bool shooting;
	s32 refireTime;
	s32 shootTimeRemaining;
	s32 shootingRange;

	// The person's health. Starts at 1.0f by default
	f32 health;

	// Sets an absolute value for gravity (not based on global gravity)
	virtual void gravitateAbsolute(f32 value);

	// The model associated with this person
	scene::IAnimatedMeshSceneNode* model;

	// The collider associated with this person
	scene::ISceneNodeAnimatorCollisionResponse* collider;
	
	// The selector associated with this person (so that others can collide with it)
	scene::ITriangleSelector* personSelector;

	// The metaselector with this person (for its own collision detection)
	scene::IMetaTriangleSelector* metaSelector;

	// The movable selector associated with this person (for fast bullet collisions)
	scene::IMetaTriangleSelector* movableSelector;

	// The current permanent animation of this model, e.g. running, walking...
	scene::EMD2_ANIMATION_TYPE permanentAnimation;

	// The current timed animation of this model, e.g. jumping
	scene::EMD2_ANIMATION_TYPE temporaryAnimation;

	// The time that this model has left on the temporary animation
	s32 temporaryAnimationTimeLeft;

	// Start a temporary timed animation (or -1 to set the permanent animation)
	// To stop the current temporary animation, set a new one with a howLong of 0
	void setAnimation(scene::EMD2_ANIMATION_TYPE newAnimation, s32 howLong);
	
	// Turn on animation, collision detection and gravity for this person
	virtual void startPhysics();

public:

	// TEMPORARY STUFF THAT SHOULD BE REMOVED ONCE I FINISH
	core::aabbox3d<f32> getBoundingBox() { return model ? model->getBoundingBox() : core::aabbox3d<f32>(); }

	void setPosition(core::vector3df pos) { model->setPosition(pos); }

};

#endif