// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CSceneNodeAnimatorCollisionResponse.h"
#include "ISceneCollisionManager.h"
#include "ISceneManager.h"
#include "os.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorCollisionResponse::CSceneNodeAnimatorCollisionResponse(
		ISceneManager* scenemanager,
		ITriangleSelector* world, ISceneNode* object, 
		const core::vector3df& ellipsoidRadius,
		const core::vector3df& gravityPerSecond,
		f32 accelerationPerSecond,
		const core::vector3df& ellipsoidTranslation,
		f32 slidingSpeed)
: SceneManager(scenemanager), World(world), Object(object),
	Radius(ellipsoidRadius), Gravity(gravityPerSecond / 1000.0f),
	SlidingSpeed(slidingSpeed), Translation(ellipsoidTranslation)
{
	setAccelerationPerSecond(accelerationPerSecond);

	if (World)
		World->grab();

	if (Object)
		LastPosition = Object->getPosition();

	Falling = false;

	LastTime = os::Timer::getTime();
	FallStartTime = LastTime;

	RefTriangle.pointA.set(0.0f, 0.0f, 0.0f);
	RefTriangle.pointB.set(0.0f, 0.0f, 0.0f);
	RefTriangle.pointC.set(0.0f, 0.0f, 0.0f);
}



//! destructor
CSceneNodeAnimatorCollisionResponse::~CSceneNodeAnimatorCollisionResponse()
{
	if (World)
		World->drop();
}


//! Returns if the attached scene node is falling, which means that
//! there is no blocking wall from the scene node in the direction of
//! the gravity.
bool CSceneNodeAnimatorCollisionResponse::isFalling()
{
	return Falling;
}


//! Sets the radius of the ellipsoid with which collision detection and
//! response is done. 
void CSceneNodeAnimatorCollisionResponse::setEllipsoidRadius(
	const core::vector3df& radius)
{
	Radius = radius;
}

//! Returns the radius of the ellipsoid with wich the collision detection and
//! response is done.
core::vector3df CSceneNodeAnimatorCollisionResponse::getEllipsoidRadius() const
{
	return Radius;
}


//! Sets the gravity of the environment.
void CSceneNodeAnimatorCollisionResponse::setGravity(const core::vector3df& gravity)
{
	Gravity = gravity;
}


//! Returns current vector of gravity.
core::vector3df CSceneNodeAnimatorCollisionResponse::getGravity() const
{
	return Gravity;
}


//! Sets the translation of the ellipsoid for collision detection.
void CSceneNodeAnimatorCollisionResponse::setEllipsoidTranslation(core::vector3df translation)
{
	Translation = translation;
}



//! Returns the translation of the ellipsoid for collision detection.
core::vector3df CSceneNodeAnimatorCollisionResponse::getEllipsoidTranslation() const
{
	return Translation;
}



//! Returns the acceleration per second, if the scene node is falling down.
f32 CSceneNodeAnimatorCollisionResponse::getAccelerationPerSecond() const
{
	return AccelerationPerSecond * 1000000.0f;
}



//! Sets a new acceleration per second value.
void CSceneNodeAnimatorCollisionResponse::setAccelerationPerSecond(f32 accel)
{
	AccelerationPerSecond = accel / 1000000.0f;
}



//! Sets a triangle selector holding all triangles of the world with which
//! the scene node may collide.
void CSceneNodeAnimatorCollisionResponse::setWorld(ITriangleSelector* newWorld)
{
	if (World)
		World->drop();

	World = newWorld;
	if (World)
		World->grab();
}



//! Returns the current triangle selector containing all triangles for
//! collision detection.
ITriangleSelector* CSceneNodeAnimatorCollisionResponse::getWorld() const
{
	return World;
}



//! animates a scene node
void CSceneNodeAnimatorCollisionResponse::animateNode(ISceneNode* node, u32 timeMs)
{
	if (node != Object)
	{
		os::Printer::log("CollisionResponseAnimator only works with same scene node as set as object during creation", ELL_ERROR);
		return;
	}

	if (!World)
		return;

	u32 diff = timeMs - LastTime;
	LastTime = timeMs;

	core::vector3df pos = Object->getPosition();
	core::vector3df vel = pos - LastPosition;
	core::vector3df g = Gravity * (f32)diff;

	if (Falling)
		g = g * (AccelerationPerSecond * (f32)(timeMs - FallStartTime)*(timeMs - FallStartTime));

	core::triangle3df triangle = RefTriangle;

	if (vel+g != core::vector3df(0,0,0))
	{
		// TODO: divide SlidingSpeed by frame time

		bool f = false;
		pos = SceneManager->getSceneCollisionManager()->getCollisionResultPosition(
				World, LastPosition-Translation,
				Radius, vel, triangle, f, SlidingSpeed, g);

		pos += Translation;

		if (f)//triangle == RefTriangle)
		{
			if (!Falling)
				FallStartTime = timeMs;

			Falling = true;
		}
		else
			Falling = false;

		Object->setPosition(pos);
	}

	LastPosition = Object->getPosition();
}



} // end namespace scene
} // end namespace irr

