// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __S_ANIMATED_MESH_H_INCLUDED__
#define __S_ANIMATED_MESH_H_INCLUDED__

#include "IAnimatedMesh.h"

namespace irr
{
namespace scene
{

	//! Simple implementation of the IAnimatedMesh interface.
	struct SAnimatedMesh : public IAnimatedMesh
	{
		//! constructor
		SAnimatedMesh()
		{
			#ifdef _DEBUG
			setDebugName("SAnimatedMesh");
			#endif

			Type = EAMT_UNKNOWN;
		}


		//! destructor
		~SAnimatedMesh()
		{
			// drop meshes
			for (u32 i=0; i<Meshes.size(); ++i)
				Meshes[i]->drop();
		};


		//! Gets the frame count of the animated mesh.
		//! \return Returns the amount of frames. If the amount is 1, it is a static, non animated mesh.
		virtual s32 getFrameCount()
		{
			return Meshes.size();
		}



		//! Returns the IMesh interface for a frame.
		//! \param frame: Frame number as zero based index. The maximum frame number is
		//! getFrameCount() - 1;
		//! \param detailLevel: Level of detail. 0 is the lowest,
		//! 255 the highest level of detail. Most meshes will ignore the detail level.
		//! \return Returns the animated mesh based on a detail level. 
		virtual IMesh* getMesh(s32 frame, s32 detailLevel, s32 startFrameLoop=-1, s32 endFrameLoop=-1)
		{
			return Meshes[frame];
		}


		//! adds a Mesh
		void addMesh(IMesh* mesh)
		{
			if (mesh)
			{
				mesh->grab();
				Meshes.push_back(mesh);
			}
		}

        
		//! Returns an axis aligned bounding box of the mesh.
		//! \return A bounding box of this mesh is returned.
		virtual const core::aabbox3d<f32>& getBoundingBox() const
		{
			return Box;
		}

		void recalculateBoundingBox()
		{
			Box.reset(0,0,0);

			if (Meshes.empty())
				return;

			Box = Meshes[0]->getBoundingBox();

			for (u32 i=1; i<Meshes.size(); ++i)
				Box.addInternalBox(Meshes[i]->getBoundingBox());				
		}


		//! Returns the type of the animated mesh.
		virtual EANIMATED_MESH_TYPE getMeshType() const
		{
			return Type;
		}

		core::aabbox3d<f32> Box;
		core::array<IMesh*> Meshes;
		EANIMATED_MESH_TYPE Type;
	};


} // end namespace scene
} // end namespace irr

#endif

