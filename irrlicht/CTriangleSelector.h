// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_TRIANGLE_SELECTOR_H_INCLUDED__
#define __C_TRIANGLE_SELECTOR_H_INCLUDED__

#include "ITriangleSelector.h"
#include "IMesh.h"

namespace irr
{
namespace scene
{

class ISceneNode;

//! Stupid triangle selector without optimization
class CTriangleSelector : public ITriangleSelector
{
public:

	//! Constructs a selector based on a mesh
	CTriangleSelector(ISceneNode* node);

	//! Constructs a selector based on a mesh
	CTriangleSelector(IMesh* mesh, ISceneNode* node);

	//! Constructs a selector based on a bounding box
	CTriangleSelector(core::aabbox3d<f32> box, ISceneNode* node);

	~CTriangleSelector();

	//! Gets all triangles.
	void getTriangles(core::triangle3df* triangles, s32 arraySize, s32& outTriangleCount, 
		const core::matrix4* transform=0);

	//! Determines whether a particular bounding box contains any triangles
	virtual bool isEmpty(const core::aabbox3d<f32>& box);

	//! Gets all triangles which lie within a specific bounding box.
	void getTriangles(core::triangle3df* triangles, s32 arraySize, s32& outTriangleCount, 
		const core::aabbox3d<f32>& box, const core::matrix4* transform=0);

	//! Gets all triangles which have or may have contact with a 3d line.
	virtual void getTriangles(core::triangle3df* triangles, s32 arraySize,
		s32& outTriangleCount, const core::line3d<f32>& line, 
		const core::matrix4* transform=0);

	//! Returns amount of all available triangles in this selector
	virtual s32 getTriangleCount() const;

protected:

	ISceneNode* SceneNode;
	core::array<core::triangle3df> Triangles;
};

} // end namespace scene
} // end namespace irr


#endif

