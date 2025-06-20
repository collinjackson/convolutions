// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OCT_TREE_SCENE_NODE_H_INCLUDED__
#define __C_OCT_TREE_SCENE_NODE_H_INCLUDED__

#include "ISceneNode.h"
#include "IMesh.h"
#include "OctTree.h"

namespace irr
{
namespace scene
{
	//! implementation of the IBspTreeSceneNode
	class COctTreeSceneNode : public ISceneNode
	{
	public:

		//! constructor
		COctTreeSceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id, 
			s32 minimalPolysPerNode=128);

		//! destructor
		virtual ~COctTreeSceneNode();

		virtual void OnPreRender();

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! creates the tree
		bool createTree(IMesh* mesh);

		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hirachy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		virtual video::SMaterial& getMaterial(s32 i);
		
		//! returns amount of materials used by this scene node.
		virtual s32 getMaterialCount();

	private:

		core::aabbox3d<f32> Box;

		OctTree<video::S3DVertex>* StdOctTree;
		core::array< OctTree<video::S3DVertex>::SMeshChunk > StdMeshes;

		OctTree<video::S3DVertex2TCoords>* LightMapOctTree;
		core::array< OctTree<video::S3DVertex2TCoords>::SMeshChunk > LightMapMeshes;

		video::E_VERTEX_TYPE vertexType;
		core::array< video::SMaterial > Materials;

		s32 MinimalPolysPerNode;
	};

} // end namespace scene
} // end namespace irr

#endif

