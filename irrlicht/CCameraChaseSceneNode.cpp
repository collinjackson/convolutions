// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CCameraChaseSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "stdio.h"

namespace irr
{
namespace scene
{

//! constructor
CCameraChaseSceneNode::CCameraChaseSceneNode(ISceneNode* parent, ISceneManager* mgr, gui::ICursorControl* cursorControl,
		s32 id,
	 f32 rs, f32 zs, f32 ts)
: CCameraSceneNode(parent, mgr, id), CursorControl(cursorControl), firstUpdate(true)
{
	if (CursorControl)
		CursorControl->grab();

	zooming = false;
	rotating = false;
	moving = false;
	translating = false;
	zoomSpeed = zs;
	rotateSpeed = rs;
	translateSpeed = ts;
	currentZoom = 30.0f;
	Uncle = 0;
	Target.set(0.0f, 0.0f, 0.0f);
	rotX = 90;
	rotY = 45;		// Nice overhead angle
	nElevation = 15;	// Shift it all up a bit
	oldTargetX = Target.X;
	oldTargetY = Target.Y;
	oldTargetZ = Target.Z;

	allKeysUp();
	recalculateViewArea();
}


//! destructor
CCameraChaseSceneNode::~CCameraChaseSceneNode()
{
	if (CursorControl)
		CursorControl->drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for 
//! example with scene::ISceneManager::addChaseCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever.
bool CCameraChaseSceneNode::OnEvent(SEvent event)
{
	return false;
}

//! onPostRender is called just after rendering the whole scene.
//! nodes may calculate or store animations here, and may do other useful things,
//! dependent on what they are.
void CCameraChaseSceneNode::OnPostRender(u32 timeMs)
{
	animate();

	setPosition(Pos);
	updateAbsolutePosition();

	// This scene node cannot be animated by scene node animators.
}


bool CCameraChaseSceneNode::isMouseKeyDown(s32 key)
{
	return MouseKeys[key];
}



void CCameraChaseSceneNode::animate()
{
	if(!MouseInputReceiverEnabled)
	{
		firstUpdate = true; 
		return; 
	}
	else	
	if (firstUpdate)
	{
		CursorControl->setPosition(0.5f, 0.5f);
		firstUpdate = false;
		return;
	}
	
	core::position2d<f32> cursorpos = CursorControl->getRelativePosition();
	rotX += (0.5f - cursorpos.X) * rotateSpeed;
	rotY += (0.5f - cursorpos.Y) * rotateSpeed;	
	CursorControl->setPosition(0.5f, 0.5f);
	
	// Used to be 89. This should make things more reasonable
	if (rotY > 80.0f) rotY = 80.0f;
	if (rotY < -60.0f) rotY = -60.0f;

	const SViewFrustrum* va = getViewFrustrum();

	f32 nRotX = rotX;
	f32 nRotY = rotY;
	f32 nZoom = currentZoom;
/*
	if (isMouseKeyDown(0) && isMouseKeyDown(2))
	{
		if (!zooming)
		{
			zoomStartX = MousePos.X;
			zoomStartY = MousePos.Y;
			zooming = true;
			nZoom = currentZoom;
		}
		else
		{
			f32 old = nZoom;
			nZoom += (zoomStartX - MousePos.X) * zoomSpeed;

			if (nZoom < 0)
				nZoom = old;
		}
	}
	else
	{
		if (zooming)
		{
			f32 old = currentZoom;
			currentZoom = currentZoom + (zoomStartX - MousePos.X ) * zoomSpeed;
			nZoom = currentZoom;

			if (nZoom < 0)
				nZoom = currentZoom = old;
		}

		zooming = false;
	}
*/
	// Translation ---------------------------------
/*
	core::vector3df translate(oldTargetX, oldTargetY, oldTargetZ);

	core::vector3df tvectX = Pos - Target;
	tvectX = tvectX.crossProduct(UpVector);
	tvectX.normalize();

	core::vector3df tvectY = (va->getFarLeftDown() - va->getFarRightDown());
	tvectY = tvectY.crossProduct(UpVector.Y > 0 ? Pos - Target : Target - Pos);
	tvectY.normalize();
	

	if (isMouseKeyDown(2) && !zooming)
	{
		if (!translating)
		{
			translateStartX = MousePos.X;
			translateStartY = MousePos.Y;
			translating = true;
		}
		else
		{
			translate +=	tvectX * (translateStartX - MousePos.X)*translateSpeed + 
								tvectY * (translateStartY - MousePos.Y)*translateSpeed;
		}
	}
	else
	{
		if (translating)
		{
			translate +=	tvectX * (translateStartX - MousePos.X)*translateSpeed + 
								tvectY * (translateStartY - MousePos.Y)*translateSpeed;
			oldTargetX = translate.X;
			oldTargetY = translate.Y;
			oldTargetZ = translate.Z;
		}

		translating = false;
	}
*/
	// Rotation ------------------------------------
/*
	if (isMouseKeyDown(0) && !zooming)
	{
		if (!rotating)
		{
			rotateStartX = MousePos.X;
			rotateStartY = MousePos.Y;
			rotating = true;
			nRotX = rotX;
			nRotY = rotY;
		}
		else
		{
			nRotX += (rotateStartX - MousePos.X) * rotateSpeed;
			nRotY += (rotateStartY - MousePos.Y) * rotateSpeed;
		}
	}
	else
	{
		if (rotating)
		{
			rotX = rotX + (rotateStartX - MousePos.X) * rotateSpeed;
			rotY = rotY + (rotateStartY - MousePos.Y) * rotateSpeed;
			nRotX = rotX;
			nRotY = rotY;
		}

		rotating = false;
	}
*/
	// Set Pos ------------------------------------

	if(Uncle) Target = Uncle->getPosition();

	Pos.X = nZoom + Target.X;
	Pos.Y = Target.Y;
	Pos.Z = Target.Z;

	Pos.rotateXYBy(nRotY, Target);
	Pos.rotateXZBy(-nRotX, Target);

	Pos.Y += nElevation;

	// Rotation Error ----------------------------

	if (nRotY < 0.0f)
		nRotY *= -1.0f;
	
	nRotY = (f32)fmod(nRotY, 360.0f);
	
	if (nRotY >= 90.0f && nRotY <= 270.0f)
		UpVector.set(0, -1, 0);
	else
		UpVector.set(0, 1, 0);
}

void CCameraChaseSceneNode::allKeysUp()
{
	for (s32 i=0; i<3; ++i)
		MouseKeys[i] = false;
}


} // end namespace
} // end namespace