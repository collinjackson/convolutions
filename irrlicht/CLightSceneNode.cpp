// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CLightSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"

namespace irr
{
namespace scene
{

//! constructor
CLightSceneNode::CLightSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,	
	const core::vector3df& position, video::SColorf color,f32 radius)
: ILightSceneNode(parent, mgr, id, position)
{
	LightData.Radius = radius;
	LightData.DiffuseColor = color;
	LightData.Position = position;
}

CLightSceneNode::~CLightSceneNode()
{
}


//! pre render event
void CLightSceneNode::OnPreRender()
{
	if (IsVisible)
	{
		SceneManager->registerNodeForRendering(this, SNRT_LIGHT_AND_CAMERA);
		ISceneNode::OnPreRender();
	}
}


//! render
void CLightSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	if (!driver)
		return;

	LightData.Position = getAbsolutePosition();
	driver->addDynamicLight(LightData);
}


//! returns the light data
video::SLight& CLightSceneNode::getLightData()
{
	return LightData;
}

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CLightSceneNode::getBoundingBox() const
{
	return BBox;
}

} // end namespace scene
} // end namespace irr
