// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __I_SKY_BOX_FADER_SCENE_NODE_H_INCLUDED__
#define __I_SKY_BOX_FADER_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"

namespace irr
{
namespace scene
{

	// Skybox, rendererd with zbuffer turned off, before all other nodes.
	class ISkyBoxFaderSceneNode : public ISceneNode
	{
	public:

		//! constructor
		ISkyBoxFaderSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id)
			: ISceneNode(parent, mgr, id) {}

		//! destructor
		virtual ~ISkyBoxFaderSceneNode() {}

		virtual void OnPreRender() = 0;

		//! renders the node.
		virtual void render() = 0;

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const = 0;

		//! Gets the color to fade out to or to fade in from.
		virtual video::SColor getColor() const = 0;

		//! Sets the color to fade out to or to fade in from.
		virtual void setColor(video::SColor color) = 0;

		//! Starts the fade in process.
		virtual void fadeIn(u32 time) = 0;

		//! Starts the fade out process.
		virtual void fadeOut(u32 time) = 0;

		//! Returns if the fade in or out process is done.
		virtual bool isReady() const = 0;
	};

} // end namespace scene
} // end namespace irr

#endif

