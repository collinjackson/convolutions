// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_EMPTY_SCENE_NODE_H_INCLUDED__
#define __C_EMPTY_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"

namespace irr
{
namespace scene
{

	class CEmptySceneNode : public ISceneNode
	{
	public:

		//! constructor
		CEmptySceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id);

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! This method is called just before the rendering process of the whole scene.
		virtual void OnPreRender();

		//! does nothing.
		virtual void render();

	private:

		core::aabbox3d<f32> Box;
	};

} // end namespace scene
} // end namespace irr

#endif

