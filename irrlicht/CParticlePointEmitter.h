// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_PARTICLE_POINT_EMITTER_H_INCLUDED__
#define __C_PARTICLE_POINT_EMITTER_H_INCLUDED__

#include "IParticleEmitter.h"
#include "array.h"

namespace irr
{
namespace scene
{

//! A default point emitter
class CParticlePointEmitter : public IParticleEmitter
{
public:

	//! constructor
	CParticlePointEmitter(
		core::vector3df direction = core::vector3df(0.0f,0.03f,0.0f), 
		u32 minParticlesPerSecond = 5,
		u32 maxParticlePerSecond = 10,
		video::SColor minStartColor = video::SColor(255,0,0,0),
		video::SColor maxStartColor = video::SColor(255,255,255,255),
		u32 lifeTimeMin=2000,
		u32 lifeTimeMax=4000,
		s32 maxAngleDegrees=0);

	//! Prepares an array with new particles to emitt into the system
	//! and returns how much new particles there are.
	virtual s32 emitt(u32 now, u32 timeSinceLastCall, SParticle*& outArray);

private:

	SParticle Particle;
	core::vector3df Direction;
	u32 MinParticlesPerSecond, MaxParticlesPerSecond;
	video::SColor MinStartColor, MaxStartColor;
	u32 MinLifeTime, MaxLifeTime;
	s32 MaxAngleDegrees;

	u32 Time;
	u32 Emitted;
};

} // end namespace scene
} // end namespace irr


#endif

