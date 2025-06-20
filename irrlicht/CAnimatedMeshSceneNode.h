// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__
#define __C_ANIMATED_MESH_SCENE_NODE_H_INCLUDED__

#include "IAnimatedMeshSceneNode.h"
#include "IAnimatedMesh.h"

namespace irr
{
namespace scene
{
	class IDummyTransformationSceneNode;

	class CAnimatedMeshSceneNode : public IAnimatedMeshSceneNode
	{
	public:

		//! constructor
		CAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, ISceneManager* mgr,	s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! destructor
		virtual ~CAnimatedMeshSceneNode();

		//! sets the current frame. from now on the animation is played from this frame.
		virtual void setCurrentFrame(s32 frame);

		//! OnPostRender() is called just after rendering the whole scene.
		virtual void OnPostRender(u32 timeMs);

		//! frame
		virtual void OnPreRender();

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! sets the frames between the animation is looped.
		//! the default is 0 - MaximalFrameCount of the mesh.
		virtual bool setFrameLoop(s32 begin, s32 end);

		//! sets the speed with witch the animation is played
		virtual void setAnimationSpeed(s32 framesPerSecond);

		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hirachy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		virtual video::SMaterial& getMaterial(s32 i);
		
		//! returns amount of materials used by this scene node.
		virtual s32 getMaterialCount();

		//! Creates shadow volume scene node as child of this node
		//! and returns a pointer to it. 
		virtual IShadowVolumeSceneNode* addShadowVolumeSceneNode(s32 id,
			bool zfailmethod=true, f32 infinity=10000.0f);

		//! Returns a pointer to a child node, wich has the same transformation as 
		//! the corrsesponding joint, if the mesh in this scene node is a ms3d mesh.
		virtual ISceneNode* getMS3DJointNode(const c8* jointName);

		//! Removes a child from this scene node.
		//! Implemented here, to be able to remove the shadow properly, if there is one,
		//! or to remove attached childs.
		virtual bool removeChild(ISceneNode* child);

		//! Starts a MD2 animation. 
		virtual bool setMD2Animation(EMD2_ANIMATION_TYPE anim);

		//! Starts a special MD2 animation. 
		virtual bool setMD2Animation(const c8* animationName);

		//! Returns the current displayed frame number.
		virtual s32 getFrameNr();

	private:

		core::array<video::SMaterial> Materials;
		core::aabbox3d<f32> Box;
		IAnimatedMesh* Mesh;
		s32 BeginFrameTime;
		s32 StartFrame, EndFrame;
		s32 FramesPerSecond;

		IShadowVolumeSceneNode* Shadow;

		core::array<IDummyTransformationSceneNode* > JointChildSceneNodes;
	};

} // end namespace scene
} // end namespace irr

#endif

