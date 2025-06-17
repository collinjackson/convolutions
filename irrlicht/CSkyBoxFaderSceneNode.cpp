// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CSkyBoxFaderSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "ICameraSceneNode.h"
#include "S3DVertex.h"
#include "os.h"

namespace irr
{
namespace scene
{

//! constructor
CSkyBoxFaderSceneNode::CSkyBoxFaderSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
: ISkyBoxFaderSceneNode(parent, mgr, id)
{
	#ifdef _DEBUG
	setDebugName("CSkyBoxFaderSceneNode");
	#endif

}


//! destructor
CSkyBoxFaderSceneNode::~CSkyBoxFaderSceneNode()
{
}


//! renders the node.
void CSkyBoxFaderSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();
	if (!driver)
		return;

	u32 now = os::Timer::getTime();
	if (now > EndTime && Action == EFA_FADE_IN)
	{
		Action = EFA_NOTHING;
		return;
	}

	if (driver)
	{
		f32 d;

		if (now > EndTime)
			d = 0.0f;
		else
			d = (EndTime - now) / (f32)(EndTime - StartTime);

		video::SColor newCol = FullColor.getInterpolated(TransColor, d);
		driver->draw2DRectangle(newCol,driver->getViewPort(), 0);
	}

}

//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CSkyBoxFaderSceneNode::getBoundingBox() const
{
	return Box;
}


void CSkyBoxFaderSceneNode::OnPreRender()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this, SNRT_SKY_BOX_FADER);

	ISceneNode::OnPreRender();
}


//! Gets the color to fade out to or to fade in from.
video::SColor CSkyBoxFaderSceneNode::getColor() const
{
	return Color;
}



//! Sets the color to fade out to or to fade in from.
void CSkyBoxFaderSceneNode::setColor(video::SColor color)
{
	Color = color;
	FullColor = Color;
	TransColor = Color;

	if (Action == EFA_FADE_OUT)
	{
		FullColor.setAlpha(0);	
		TransColor.setAlpha(255);
	}
	else
	if (Action == EFA_FADE_IN)
	{
		FullColor.setAlpha(255);	
		TransColor.setAlpha(0);
	}
}


//! Returns if the fade in or out process is done.
bool CSkyBoxFaderSceneNode::isReady() const 
{
	u32 now = os::Timer::getTime();
	return (now > EndTime);
}



//! Starts the fade in process.
void CSkyBoxFaderSceneNode::fadeIn(u32 time)
{
	StartTime = os::Timer::getTime();
	EndTime = StartTime + time;
	Action = EFA_FADE_IN;
	setColor(Color);
}


//! Starts the fade out process.
void CSkyBoxFaderSceneNode::fadeOut(u32 time)
{
	StartTime = os::Timer::getTime();
	EndTime = StartTime + time;
	Action = EFA_FADE_OUT;
	setColor(Color);
}



} // end namespace scene
} // end namespace irr

