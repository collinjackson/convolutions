// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CCameraChaseOverSceneNode.h"
#include "IVideoDriver.h"
#include "ISceneManager.h"
#include "os.h"
#include "Keycodes.h"
#include "stdio.h"

namespace irr
{
namespace scene
{

//! constructor
CCameraChaseOverSceneNode::CCameraChaseOverSceneNode(ISceneNode* parent, ISceneManager* mgr,
		gui::ICursorControl* cursorControl, s32 id, f32 rotateSpeed , f32 moveSpeed,
		SKeyMap* keyMapArray, s32 keyMapSize)
: CCameraSceneNode(parent, mgr, id), CursorControl(cursorControl),
	MoveSpeed(moveSpeed), RotateSpeed(rotateSpeed), /*RotationX(0.0f), 
	RotationY(0.0f),*/ firstUpdate(true)
{
	if (CursorControl)
		CursorControl->grab();

	MoveSpeed /= 1000.0f;

	recalculateViewArea();

	allKeysUp();

	// create key map
	if (!keyMapArray || !keyMapSize)
	{
		// create default key map
		KeyMap.push_back(SCamKeyMap(0, irr::KEY_UP));
		KeyMap.push_back(SCamKeyMap(1, irr::KEY_DOWN));
		KeyMap.push_back(SCamKeyMap(2, irr::KEY_LEFT));
		KeyMap.push_back(SCamKeyMap(3, irr::KEY_RIGHT));
	}
	else
	{
		// create custom key map

		for (s32 i=0; i<keyMapSize; ++i)
		{
			switch(keyMapArray[i].Action)
			{
			case EKA_MOVE_FORWARD: KeyMap.push_back(SCamKeyMap(0, keyMapArray[i].KeyCode));
				break;
			case EKA_MOVE_BACKWARD: KeyMap.push_back(SCamKeyMap(1, keyMapArray[i].KeyCode));
				break;
			case EKA_STRAFE_LEFT: KeyMap.push_back(SCamKeyMap(2, keyMapArray[i].KeyCode));
				break;
			case EKA_STRAFE_RIGHT: KeyMap.push_back(SCamKeyMap(3, keyMapArray[i].KeyCode));
				break;
			} // end switch
		} // end for
	}// end if
	
}


//! destructor
CCameraChaseOverSceneNode::~CCameraChaseOverSceneNode()
{
	if (CursorControl)
		CursorControl->drop();
}


//! It is possible to send mouse and key events to the camera. Most cameras
//! may ignore this input, but camera scene nodes which are created for 
//! example with scene::ISceneManager::addMayaCameraSceneNode or
//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
//! for changing their position, look at target or whatever. 
bool CCameraChaseOverSceneNode::OnEvent(SEvent event)
{
	if (!InputReceiverEnabled)
		return false;

	if (event.EventType == EET_KEY_INPUT_EVENT )
	{
		s32 cnt = (s32)KeyMap.size();
		for (s32 i=0; i<cnt; ++i)
			if (KeyMap[i].keycode == event.KeyInput.Key)
			{
				CursorKeys[KeyMap[i].action] = event.KeyInput.PressedDown; 
				return true;
			}
	}

	return false;
}



//! onPostRender is called just after rendering the whole scene.
//! nodes may calculate or store animations here, and may do other useful things,
//! dependent on what they are.
void CCameraChaseOverSceneNode::OnPostRender(u32 timeMs)
{
	animate();

	core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
				for (; ait != Animators.end(); ++ait)
					(*ait)->animateNode(this, timeMs);

	updateAbsolutePosition();
	Target = getPosition() + TargetVector;

	core::list<ISceneNode*>::Iterator it = Children.begin();
				for (; it != Children.end(); ++it)
					(*it)->OnPostRender(timeMs);
}


void CCameraChaseOverSceneNode::animate()
{
	if (SceneManager->getActiveCamera() != this)
		return;
	
	if (firstUpdate)
	{
		if (CursorControl)
			CursorControl->setPosition(0.5f, 0.5f);

		LastAnimationTime = os::Timer::getTime();

		firstUpdate = false;
		return;
	}

	// get time

	s32 now = os::Timer::getTime();
	s32 timeDiff = now - LastAnimationTime;
	LastAnimationTime = now;

	// Update rotation

	Target.set(0,0,1);

	if (!CursorControl)
		return;

	RelativeRotation.X *= -1.0f;
	RelativeRotation.Y *= -1.0f;

	if (InputReceiverEnabled) 
	{
		core::position2d<f32> cursorpos = CursorControl->getRelativePosition();

		if (cursorpos.X < 0.49f || cursorpos.X > 0.51f ||
			cursorpos.Y < 0.49f || cursorpos.Y > 0.51f)
		{

			
			RelativeRotation.Y += (0.5f - cursorpos.X) * RotateSpeed;		
			RelativeRotation.X += (0.5f - cursorpos.Y) * RotateSpeed;
			
			CursorControl->setPosition(0.5f, 0.5f);

			if (RelativeRotation.X > 89.0f) RelativeRotation.X = 89.0f;
			if (RelativeRotation.X < -89.0f) RelativeRotation.X = -89.0f;

		}
	}

	// set target

	core::matrix4 mat;
	mat.setRotationDegrees(core::vector3df(-RelativeRotation.X, -RelativeRotation.Y, 0));
	mat.transformVect(Target);

	// update position

	core::vector3df pos = getPosition();	

	core::vector3df movedir = Target;
	if(MouseInputReceiverEnabled){
		
	}

	movedir.normalize();


	core::vector3df strafevect = Target;
	strafevect = strafevect.crossProduct(UpVector);
	strafevect.normalize();

	pos = Uncle->getPosition();
	if(!MouseInputReceiverEnabled || 1){
	
	movedir = strafevect;
	movedir.rotateXZBy(270, core::vector3df(0,0,0));
	
	//movedir.rotateYZBy(90,strafevect);

		if (CursorKeys[0])
			pos += movedir * (f32)timeDiff * MoveSpeed;

		if (CursorKeys[1])
			pos -= movedir * (f32)timeDiff * MoveSpeed;

		// strafing



		if (CursorKeys[2])
			pos += strafevect * (f32)timeDiff * MoveSpeed;

		if (CursorKeys[3])
			pos -= strafevect * (f32)timeDiff * MoveSpeed;	
	}
	else{
	
		if (CursorKeys[0])
			pos += movedir * (f32)timeDiff * MoveSpeed;

		if (CursorKeys[1])
			pos -= movedir * (f32)timeDiff * MoveSpeed;

		// strafing



		if (CursorKeys[2])
			pos += strafevect * (f32)timeDiff * MoveSpeed;

		if (CursorKeys[3])
			pos -= strafevect * (f32)timeDiff * MoveSpeed;	


	}

	// write translation


	//setPosition(cpos);

	// write right target
	


	TargetVector = Target;
	Target += pos;
	

	
	//core::vector3df cpos = getPosition();

	core::vector3df cpos = pos;
	
	//cpos.X = Target.X - pos.X; cpos.Y =  Target.Y - pos.Y; cpos.Z = Target.Z - pos.Z;
	//cpos = pos - ((Target - pos) * 2000);

	cpos = pos - ((Target - pos) * 2000);
	
	//cpos.Y =pos.Y + 2000;
	//cpos.Z =pos.Z;
	//u	cpos.Y += 2000;
	
	Uncle->setPosition(pos);

	RelativeRotation.X *= -1.0f;
	RelativeRotation.Y *= -1.0f;
	
	
	cpos = Uncle->getAbsolutePosition();
	cpos.X += 0; cpos.Y += 500; cpos.Z += 0;
	setPosition(cpos);
	// setTarget(Uncle->getPosition());
	printf("x = %lf, y = %lf, z = %lf\n", cpos.X, cpos.Y, cpos.Z);
	printf("ALT x = %lf, y = %lf, z = %lf\n", pos.X, pos.Y, pos.Z);

}




void CCameraChaseOverSceneNode::setRotateSpeed(f32 speed){
	RotateSpeed = speed;
}


f32 CCameraChaseOverSceneNode::getRotateSpeed(){
	return RotateSpeed;
}


void CCameraChaseOverSceneNode::allKeysUp()
{
	for (s32 i=0; i<4; ++i)
		CursorKeys[i] = false;
}



//! sets the look at target of the camera
//! \param pos: Look at target of the camera.
void CCameraChaseOverSceneNode::setTarget(const core::vector3df& wtgt)
{

	
	// calculate rotation to fit new target
	core::vector3df cpos = getAbsolutePosition();
	
	core::vector2df wline(cpos.Z-wtgt.Z, cpos.X-wtgt.X);
	wline.normalize();
	f32 wangle = (f32)wline.getAngle();
    RelativeRotation.Y = wangle + 180;

	// This part of the function is quite weired.

	wline.set(cpos.X-wtgt.X, cpos.Y-wtgt.Y);
	wline.normalize();
	wangle =  (f32)wline.getAngle();
	RelativeRotation.X =  180 - wangle;

	// I don't remember why I wrote this, but
	// it works partially. There is absolutely
	// no need for a while loop. I'll rewrite it 
	// when I am in the mood...

	s32 cnt = 0;
	while((RelativeRotation.X < -90.0f || RelativeRotation.X > 90.0f) && cnt<10)
	{
		if (RelativeRotation.X < -90.0f)
			RelativeRotation.X = -(RelativeRotation.X + 180.0f);
		if (RelativeRotation.X > 90.0f)
			RelativeRotation.X = -(RelativeRotation.X - 180.0f);

		++cnt;
	}
}


void CCameraChaseOverSceneNode::setViewMatrix(const core::matrix4& v)
{
	View = v;
}


void CCameraChaseOverSceneNode::setUncle(ISceneNode *uncle){
	Uncle = uncle;
}
		
ISceneNode* CCameraChaseOverSceneNode::getUncle(){
	return Uncle;
}

} // end namespace
} // end namespace





