// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __I_ANIMATED_MESH_H_INCLUDED__
#define __I_ANIMATED_MESH_H_INCLUDED__

#include "IUnknown.h"
#include "IMesh.h"
#include "matrix4.h"

namespace irr
{
namespace scene
{
	enum EANIMATED_MESH_TYPE
	{
		//! Unknown animated mesh type.
		EAMT_UNKNOWN = 0,

		//! Quake 2 MD2 model file
		EAMT_MD2, 

		//! Milkshape 3d skeletal animation file
		EAMT_MS3D,

		//! Maya .obj not animated model
		EAMT_OBJ,

		//! Quake 3 .bsp Map, not animated
		EAMT_BSP,

		//! 3D Studio .3ds file
		EAMT_3DS,

		//! Microsoft Direct3D .x-file. Can contain static and skeletal animated 
		//! skinned meshes. This is the standard and best supported 
		//! format of the Irrlicht Engine.
		EAMT_X
	};

	//! Interface for an animated mesh.
	class IAnimatedMesh : public IUnknown
	{
	public:

		//! destructor
		virtual ~IAnimatedMesh() {};

		//! Gets the frame count of the animated mesh.
		//! \return Returns the amount of frames. If the amount is 1, it is a static, non animated mesh.
		virtual s32 getFrameCount() = 0;

		//! Returns the IMesh interface for a frame.
		//! \param frame: Frame number as zero based index. The maximum frame number is
		//! getFrameCount() - 1;
		//! \param detailLevel: Level of detail. 0 is the lowest,
		//! 255 the highest level of detail. Most meshes will ignore the detail level.
		//! \param startFrameLoop: Because some animated meshes (.MD2) are blended between 2
		//! static frames, and maybe animated in a loop, the startFrameLoop and the endFrameLoop
		//! have to be defined, to prevent the animation to be blended between frames which are
		//! outside of this loop. 
		//! If startFrameLoop and endFrameLoop are both -1, they are ignored.
		//! \param endFrameLoop: see startFrameLoop.
		//! \return Returns the animated mesh based on a detail level. 
		virtual IMesh* getMesh(s32 frame, s32 detailLevel=255, s32 startFrameLoop=-1, s32 endFrameLoop=-1) = 0;

		//! Returns an axis aligned bounding box of the mesh.
		//! \return A bounding box of this mesh is returned.
		virtual const core::aabbox3d<f32>& getBoundingBox() const = 0;

		//! Returns the type of the animated mesh.
		//! In most cases it is not neccessary to use this method.
		//! This is useful for making a save downcast, for example
		//! if getMeshType() returns EAMT_MD2, its save to cast the
		//! IAnimatedMesh to IAnimatedMeshMD2.
		//! \returns Type of the mesh.
		virtual EANIMATED_MESH_TYPE getMeshType() const
		{
			return EAMT_UNKNOWN;
		}
	};

} // end namespace scene
} // end namespace irr

#endif

