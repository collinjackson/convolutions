// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CSceneNodeAnimatorFlyStraight.h"

namespace irr
{
namespace scene
{


//! constructor
CSceneNodeAnimatorFlyStraight::CSceneNodeAnimatorFlyStraight(const core::vector3df& startPoint, 
							const core::vector3df& endPoint, u32 timeForWay,
							bool loop, u32 now)
: Start(startPoint), End(endPoint), StartTime(now), TimeForWay(timeForWay), Loop(loop), On(true)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorFlyStraight");
	#endif

	EndTime = now + timeForWay;

	Vector = End - Start;
	WayLength = (f32)Vector.getLength();
	Vector.normalize();

	TimeFactor = WayLength / TimeForWay;
}



//! destructor
CSceneNodeAnimatorFlyStraight::~CSceneNodeAnimatorFlyStraight()
{
}



//! animates a scene node
void CSceneNodeAnimatorFlyStraight::animateNode(ISceneNode* node, u32 timeMs)
{
	if (!node || !On)
		return;

	u32 t = (timeMs-StartTime);

	core::vector3df pos = Start;

	if (!Loop && t >= TimeForWay)
	{
		pos = End;
		On = !On;
	}
	else
		pos += Vector * (f32)fmod((f32)t, (f32)TimeForWay) * TimeFactor;

	node->setPosition(pos);
}



} // end namespace scene
} // end namespace irr
