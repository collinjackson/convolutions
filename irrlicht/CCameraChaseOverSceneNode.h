// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_CAMERA_ChaseOver_SCENE_NODE_H_INCLUDED__
#define __C_CAMERA_ChaseOver_SCENE_NODE_H_INCLUDED__

#include "ICursorControl.h"
#include "CCameraSceneNode.h"
#include "vector2d.h"
#include "SKeyMap.h"
#include "array.h"

namespace irr
{
namespace scene
{

	class CCameraChaseOverSceneNode : public CCameraSceneNode
	{
	public:

		//! constructor
		CCameraChaseOverSceneNode(ISceneNode* parent, ISceneManager* mgr, 
			gui::ICursorControl* cursorControl, s32 id,
			f32 rotateSpeed, f32 moveSpeed,
			SKeyMap* keyMapArray, s32 keyMapSize);

		//! destructor
		virtual ~CCameraChaseOverSceneNode();

		//! It is possible to send mouse and key events to the camera. Most cameras
		//! may ignore this input, but camera scene nodes which are created for 
		//! example with scene::ISceneManager::addMayaCameraSceneNode or
		//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
		//! for changing their position, look at target or whatever. 

		virtual bool OnEvent(SEvent event);

		//! onPostRender is called just after rendering the whole scene.
		//! nodes may calculate or store animations here, and may do other useful things,
		//! dependent on what they are.
		virtual void OnPostRender(u32 timeMs);

		//! sets the look at target of the camera
		//! \param pos: Look at target of the camera.
		virtual void setTarget(const core::vector3df& pos);

		void setRotateSpeed(f32 speed);
		f32 getRotateSpeed();
		
		ISceneNode *getUncle();
		void setUncle(ISceneNode *uncle);

		void setViewMatrix(const core::matrix4& v);
	private:

		struct SCamKeyMap
		{
			SCamKeyMap() {};
			SCamKeyMap(s32 a, EKEY_CODE k) : action(a), keycode(k) {}

			s32 action;
			EKEY_CODE keycode;
		};

		void allKeysUp();
		void animate();

		bool CursorKeys[4];

		f32 MoveSpeed;
		f32 RotateSpeed;

		//f32 RotationX;
		//f32 RotationY;

		gui::ICursorControl* CursorControl;

		bool firstUpdate;
		s32 LastAnimationTime;

		core::vector3df TargetVector;
		core::array<SCamKeyMap> KeyMap;

		ISceneNode* Uncle;
	};

} // end namespace
} // end namespace

#endif

