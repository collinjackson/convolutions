// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_SKY_BOX_FADER_SCENE_NODE_H_INCLUDED__
#define __C_SKY_BOX_FADER_SCENE_NODE_H_INCLUDED__

#include "ISkyBoxFaderSceneNode.h"
#include "S3DVertex.h"

namespace irr
{
namespace scene
{

	// Skybox, rendererd with zbuffer turned off, before all other nodes.
	class CSkyBoxFaderSceneNode : public ISkyBoxFaderSceneNode
	{
	public:

		//! constructor
		CSkyBoxFaderSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id);

		//! destructor
		virtual ~CSkyBoxFaderSceneNode();

		virtual void OnPreRender();

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! Gets the color to fade out to or to fade in from.
		virtual video::SColor getColor() const;

		//! Sets the color to fade out to or to fade in from.
		virtual void setColor(video::SColor color);

		//! Starts the fade in process.
		virtual void fadeIn(u32 time);

		//! Starts the fade out process.
		virtual void fadeOut(u32 time);

		//! Returns if the fade in or out process is done.
		virtual bool isReady() const;

	private:

		enum EFadeAction
		{
			EFA_NOTHING = 0,
			EFA_FADE_IN,
			EFA_FADE_OUT
		};

		u32 StartTime;
		u32 EndTime;
		EFadeAction Action;		

		video::SColor Color;
		video::SColor FullColor;
		video::SColor TransColor;

		core::aabbox3d<f32> Box;
	};

} // end namespace scene
} // end namespace irr

#endif

