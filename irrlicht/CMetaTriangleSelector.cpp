// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CMetaTriangleSelector.h"

namespace irr
{
namespace scene
{

//! constructor
CMetaTriangleSelector::CMetaTriangleSelector()
{
	#ifdef _DEBUG
	setDebugName("CMetaTriangleSelector");
	#endif
}

//! destructor
CMetaTriangleSelector::~CMetaTriangleSelector()
{
	removeAllTriangleSelectors();
}



//! Returns amount of all available triangles in this selector
s32 CMetaTriangleSelector::getTriangleCount() const
{
	s32 count = 0;
	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
		count += TriangleSelectors[i]->getTriangleCount();

	return count;
}



//! Gets all triangles.
void CMetaTriangleSelector::getTriangles(core::triangle3df* triangles, s32 arraySize,
	s32& outTriangleCount, const core::matrix4* transform)
{
	s32 outWritten = 0;

	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
	{
		s32 t = 0;
		TriangleSelectors[i]->getTriangles(triangles + outWritten, arraySize - outWritten, t, transform);
		outWritten += t;
	}

    outTriangleCount = outWritten;
}



//! Gets all triangles which lie within a specific bounding box.
void CMetaTriangleSelector::getTriangles(core::triangle3df* triangles, s32 arraySize,
	s32& outTriangleCount, const core::aabbox3d<f32>& box, 
	const core::matrix4* transform)
{
	s32 outWritten = 0;

	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
	{
		s32 t = 0;
		TriangleSelectors[i]->getTriangles(triangles + outWritten, arraySize - outWritten, t,
			box, transform);
		outWritten += t;
	}

    outTriangleCount = outWritten;
}



//! Gets all triangles which have or may have contact with a 3d line.
void CMetaTriangleSelector::getTriangles(core::triangle3df* triangles, s32 arraySize,
	s32& outTriangleCount, const core::line3d<f32>& line, 
	const core::matrix4* transform)
{
	s32 outWritten = 0;

	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
	{
		s32 t = 0;
		TriangleSelectors[i]->getTriangles(triangles + outWritten, arraySize - outWritten, t,
			line, transform);
		outWritten += t;
	}

    outTriangleCount = outWritten;
}



//! Adds a triangle selector to the collection of triangle selectors
//! in this metaTriangleSelector.
void CMetaTriangleSelector::addTriangleSelector(ITriangleSelector* toAdd)
{
	if (!toAdd)
		return;

	TriangleSelectors.push_back(toAdd);
	toAdd->grab();
}



//! Removes a specific triangle selector which was added before	from the collection.
bool CMetaTriangleSelector::removeTriangleSelector(ITriangleSelector* toRemove)
{
	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
		if (toRemove == TriangleSelectors[i])
		{
			TriangleSelectors[i]->drop();
			TriangleSelectors.erase(i);
			return true;
		}

    return false;
}



//! Removes all triangle selectors from the collection.
void CMetaTriangleSelector::removeAllTriangleSelectors()
{
	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
		TriangleSelectors[i]->drop();

	TriangleSelectors.clear();
}

#include <assert.h>

//! Returns true if there are no triangles in the specified bounding box
bool CMetaTriangleSelector::isEmpty(const core::aabbox3d<f32>& box)
{
	bool result = true;
	for (s32 i=0; i<(s32)TriangleSelectors.size(); ++i)
		if(!TriangleSelectors[i]->isEmpty(box)) result = false;

	return result;
}


} // end namespace scene
} // end namespace irr

