// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CSceneNodeAnimatorRotation.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneNodeAnimatorRotation::CSceneNodeAnimatorRotation(u32 time, const core::vector3df& rotation)
: Rotation(rotation), StartTime(time)
{
	#ifdef _DEBUG
	setDebugName("CSceneNodeAnimatorRotation");
	#endif
}


//! destructor
CSceneNodeAnimatorRotation::~CSceneNodeAnimatorRotation()
{
}



//! animates a scene node
void CSceneNodeAnimatorRotation::animateNode(ISceneNode* node, u32 timeMs)
{
	if (node)
		node->setRotation(Rotation* ((timeMs-StartTime)/10.0f));
}


} // end namespace scene
} // end namespace irr

