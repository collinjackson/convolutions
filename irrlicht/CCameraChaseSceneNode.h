// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_CAMERA_CHASE_SCENE_NODE_H_INCLUDED__
#define __C_CAMERA_CHASE_SCENE_NODE_H_INCLUDED__

#include "CCameraSceneNode.h"
#include "vector2d.h"
#include "ICursorControl.h"

namespace irr
{
namespace scene
{

	class CCameraChaseSceneNode : public CCameraSceneNode
	{
	public:

		//! constructor
		CCameraChaseSceneNode(ISceneNode* parent, ISceneManager* mgr, gui::ICursorControl* cursorControl, s32 id,
			f32 rotateSpeed = -1500.0f, f32 zoomSpeed = 200.0f, f32 translationSpeed = 100.0f);

		//! destructor
		virtual ~CCameraChaseSceneNode();

		//! It is possible to send mouse and key events to the camera. Most cameras
		//! may ignore this input, but camera scene nodes which are created for 
		//! example with scene::ISceneManager::addChaseCameraSceneNode or
		//! scene::ISceneManager::addMeshViewerCameraSceneNode, may want to get this input
		//! for changing their position, look at target or whatever. 
		virtual bool OnEvent(SEvent event);

		//! onPostRender is called just after rendering the whole scene.
		//! nodes may calculate or store animations here, and may do other useful things,
		//! dependent on what they are.
		virtual void OnPostRender(u32 timeMs);
		void setRotateSpeed(f32 speed){}

		f32 getRotateSpeed(){ return 0;}

		ISceneNode *getUncle() { return Uncle;  }
		void setUncle(ISceneNode *uncle) { Uncle = uncle; }


	private:

		void allKeysUp();
		void animate();
		bool isMouseKeyDown(s32 key);

		bool MouseKeys[3];

		core::vector3df Pos;
		bool zooming, rotating, moving, translating;
		f32 zoomSpeed;
		f32 translateSpeed;
		f32 rotateSpeed;
		f32 rotateStartX, rotateStartY;
		f32 zoomStartX, zoomStartY;
		f32 translateStartX, translateStartY;
		f32 currentZoom;
		f32 rotX, rotY;
		f32 oldTargetX, oldTargetY, oldTargetZ;
		f32 nElevation;

		core::vector2df MousePos;

		ISceneNode* Uncle;

		gui::ICursorControl* CursorControl;
	
		bool firstUpdate;

	};

} // end namespace
} // end namespace

#endif

