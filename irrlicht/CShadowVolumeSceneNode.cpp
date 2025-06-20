// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CShadowVolumeSceneNode.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"

namespace irr
{
namespace scene
{


//! constructor
CShadowVolumeSceneNode::CShadowVolumeSceneNode(ISceneNode* parent,
											   ISceneManager* mgr,
											   s32 id,
											    bool zfailmethod,
												f32 infinity)
: IShadowVolumeSceneNode(parent, mgr, id), Edges(0), EdgeCount(0), 
	ShadowVolumesUsed(0), Indices(0), Vertices(0), IndexCount(0),
	VertexCount(0),	IndexCountAllocated(0), 
	VertexCountAllocated(0), UseZFailMethod(zfailmethod),
	Adjacency(0), FaceData(0), Infinity(infinity)
{
	AutomaticCullingEnabled = false;
}



//! destructor
CShadowVolumeSceneNode::~CShadowVolumeSceneNode()
{
	if (Edges)
		delete [] Edges;

	for (u32 i=0; i<ShadowVolumes.size(); ++i)
		delete [] ShadowVolumes[i].vertices;

	delete [] Vertices;
	delete [] Indices;
	delete [] Adjacency;
	delete [] FaceData;
}


void CShadowVolumeSceneNode::createShadowVolume(const core::vector3df& light)
{
	SShadowVolume* svp = 0;

	// builds the shadow volume and adds it to the shadow volume list.

	if (ShadowVolumes.size() > (u32)ShadowVolumesUsed)
	{
		// get the next unused buffer
		svp = &ShadowVolumes[ShadowVolumesUsed];
		if (svp->size >= IndexCount*5)
			svp->count = 0;
		else
		{
			svp->size = IndexCount*5;
			svp->count = 0;
			delete [] svp->vertices;
			svp->vertices = new core::vector3df[svp->size];
		}

		++ShadowVolumesUsed;
	}
	else
	{
		// add a buffer
		SShadowVolume tmp;
		ShadowVolumes.push_back(tmp);	
		svp = &ShadowVolumes[ShadowVolumes.size()-1];
		++ShadowVolumesUsed;

		// wir machen mal einen ziemlich gro�en shadowbuffer
		svp->size = IndexCount*5;
		svp->count = 0;
		svp->vertices = new core::vector3df[svp->size];
	}

	s32 faceCount = (int)(IndexCount / 3);

	if (faceCount * 6 > EdgeCount || !Edges)
	{
		delete [] Edges;
		EdgeCount = faceCount * 6;
		Edges = new u16[EdgeCount];
	}

	

	s32 numEdges = 0;
	core::vector3df ls = light * Infinity; // light scaled

	//if (!UseZFailMethod)
	//	createZPassVolume(faceCount, numEdges, light, svp);
	//else
	//	createZFailVolume(faceCount, numEdges, light, svp);

	// the createZFailVolume does currently not work 100% correctly,
	// so we create createZPassVolume with caps if the zfail method
	// is used
	createZPassVolume(faceCount, numEdges, light, svp, UseZFailMethod);
    
	
	for (s32 i=0; i<numEdges; ++i)
    {
		core::vector3df &v1 = Vertices[Edges[2*i+0]];
		core::vector3df &v2 = Vertices[Edges[2*i+1]];
		core::vector3df v3(v1 - ls);
		core::vector3df v4(v2 - ls);

		// Add a quad (two triangles) to the vertex list
		svp->vertices[svp->count++] = v1;
		svp->vertices[svp->count++] = v2;
		svp->vertices[svp->count++] = v3;

		svp->vertices[svp->count++] = v2;
		svp->vertices[svp->count++] = v4;
		svp->vertices[svp->count++] = v3;
	}
}

void CShadowVolumeSceneNode::createZFailVolume(s32 faceCount, s32& numEdges,
											   const core::vector3df light,
											   SShadowVolume* svp)
{
	u16 wFace0, wFace1, wFace2;
	core::vector3df ls = light * Infinity; // light scaled
	s32 i;

	// Check every face if it is front or back facing the light.
    for (i=0; i<faceCount; ++i)
    {
        wFace0 = Indices[3*i+0];
        wFace1 = Indices[3*i+1];
        wFace2 = Indices[3*i+2];

		core::vector3df v0 = Vertices[wFace0];
        core::vector3df v1 = Vertices[wFace1];
        core::vector3df v2 = Vertices[wFace2];

		core::vector3df normal = (v2-v1).crossProduct(v1-v0);

		if(normal.dotProduct(light) >= 0.0f )
		{
			FaceData[i] = false; // it's a back facing face

			// add front cap
			svp->vertices[svp->count++] = v0;
			svp->vertices[svp->count++] = v2;
			svp->vertices[svp->count++] = v1;

			// add back cap
			svp->vertices[svp->count++] = v0 - ls;
			svp->vertices[svp->count++] = v1 - ls;
			svp->vertices[svp->count++] = v2 - ls;
		}
		else
			FaceData[i] = true; // it's a front facing face
	}


    for(i=0; i<faceCount; ++i)
    {
		if (FaceData[i] == true)
		{
			wFace0 = Indices[3*i+0];
			wFace1 = Indices[3*i+1];
			wFace2 = Indices[3*i+2];

			u16 adj0 = Adjacency[3*i+0];
			u16 adj1 = Adjacency[3*i+1];
			u16 adj2 = Adjacency[3*i+2];

			if (adj0 != (u16)-1 && FaceData[adj0] == false)
			{
				//	add edge v0-v1
			    Edges[2*numEdges+0] = wFace0;
				Edges[2*numEdges+1] = wFace1;
				++numEdges;
			}

			if (adj1 != (u16)-1 && FaceData[adj1] == false)
			{
				//	add edge v1-v2
			    Edges[2*numEdges+0] = wFace1;
				Edges[2*numEdges+1] = wFace2;
				++numEdges;
			}

			if (adj2 != (u16)-1 && FaceData[adj2] == false)
			{
				//	add edge v2-v0
			    Edges[2*numEdges+0] = wFace2;
				Edges[2*numEdges+1] = wFace0;
				++numEdges;
			}
		}
	}

}

void CShadowVolumeSceneNode::createZPassVolume(s32 faceCount,
											   s32& numEdges,
											   const core::vector3df lightsource,
											   SShadowVolume* svp, bool caps)
{
	core::vector3df light = lightsource;
    if (light == core::vector3df(0,0,0))
		light += core::vector3df(0.0001f, 0.0001f, 0.0001f);

	core::vector3df normal;
	u16 wFace0, wFace1, wFace2;
	core::vector3df ls = light * Infinity; // light scaled

	for (s32 i=0; i<faceCount; ++i)
	{
		wFace0 = Indices[3*i+0];
        wFace1 = Indices[3*i+1];
		wFace2 = Indices[3*i+2];

		core::vector3df v0(Vertices[wFace2] - Vertices[wFace1]);
		core::vector3df v1(Vertices[wFace1] - Vertices[wFace0]);

		normal = v0.crossProduct(v1);

		if (normal.dotProduct(light) >= 0.0f)
		{
			Edges[2*numEdges+0] = wFace0;
			Edges[2*numEdges+1] = wFace1;
			++numEdges;

			Edges[2*numEdges+0] = wFace1;
			Edges[2*numEdges+1] = wFace2;
			++numEdges;

			Edges[2*numEdges+0] = wFace2;
			Edges[2*numEdges+1] = wFace0;
			++numEdges;

			if (caps)
			{
				svp->vertices[svp->count++] = Vertices[wFace0];
				svp->vertices[svp->count++] = Vertices[wFace2];
				svp->vertices[svp->count++] = Vertices[wFace1];

				svp->vertices[svp->count++] = Vertices[wFace0] - ls;
				svp->vertices[svp->count++] = Vertices[wFace1] - ls;
				svp->vertices[svp->count++] = Vertices[wFace2] - ls;
			}
		}
	}
}

//! sets the mesh from which the shadow volume should be generated.
void CShadowVolumeSceneNode::setMeshToRenderFrom(IMesh* mesh)
{
	ShadowVolumesUsed = 0;
	
	s32 oldIndexCount = IndexCount;
	s32 oldVertexCount = VertexCount;

	VertexCount = 0;
	IndexCount = 0;

	if (!mesh)
		return;

	// calculate total amount of vertices and indices

	s32 i;
	s32 totalVertices = 0;
	s32 totalIndices = 0;
	s32 bufcnt = mesh->getMeshBufferCount();
	IMeshBuffer* b;

	for (i=0; i<bufcnt; ++i)
	{
		b = mesh->getMeshBuffer(i);
		totalIndices += b->getIndexCount();
		totalVertices += b->getVertexCount();
	}

	// allocate memory if nececcary

	if (totalVertices > VertexCountAllocated)
	{
		delete [] Vertices;
		Vertices = new core::vector3df[totalVertices];
		VertexCountAllocated = totalVertices;
	}

	if (totalIndices > IndexCountAllocated)
	{
		delete [] Indices;
		Indices = new u16[totalIndices];
		IndexCountAllocated = totalIndices;

		if (UseZFailMethod)
		{
			delete [] FaceData;
			FaceData = new bool[totalIndices / 3];
		}
	}

	// copy mesh
	
	for (i=0; i<bufcnt; ++i)
	{
		b = mesh->getMeshBuffer(i);
		
		s32 idxcnt = b->getIndexCount();
		s32 vtxnow = VertexCount;

		const u16* idxp = b->getIndices();
		const u16* idxpend = idxp + idxcnt;

		for (; idxp!=idxpend; ++idxp)
			Indices[IndexCount++] = *idxp + vtxnow;

		s32 vtxcnt = b->getVertexCount();

		switch(b->getVertexType())
		{
		case video::EVT_STANDARD:
			{
				const video::S3DVertex* vp = (video::S3DVertex*)b->getVertices();
				const video::S3DVertex* vpend = vp + vtxcnt;

				for (; vp!=vpend; ++vp)
					Vertices[VertexCount++] = (*vp).Pos;
			}
			break;
		case video::EVT_2TCOORDS:
			{
				const video::S3DVertex2TCoords* vp = (video::S3DVertex2TCoords*)b->getVertices();
				const video::S3DVertex2TCoords* vpend = vp + vtxcnt;

				for (; vp!=vpend; ++vp)
					Vertices[VertexCount++] = (*vp).Pos;
			}
			break;
		}
	}

	// recalculate adjacency if neccessarry
	if (oldVertexCount != VertexCount && 
		oldIndexCount != IndexCount && UseZFailMethod)
		calculateAdjacency();

	// create as much shadow volumes as there are lights but 
	// do not ignore the max light settings.

	s32 lights = SceneManager->getVideoDriver()->getDynamicLightCount();
	core::matrix4 mat = Parent->getAbsoluteTransformation();
	core::vector3df parentpos = Parent->getAbsolutePosition();
	core::vector3df lpos;
	mat.makeInverse();

	for (i=0; i<lights; ++i)
	{
		const video::SLight& dl = SceneManager->getVideoDriver()->getDynamicLight(i);
		lpos = dl.Position;
		if (dl.CastShadows && 
			fabs((lpos - parentpos).getLengthSQ()) <= (dl.Radius*dl.Radius*4.0f))
		{
			mat.transformVect(lpos);
			createShadowVolume(lpos);
		}
	}
}



//! pre render method
void CShadowVolumeSceneNode::OnPreRender()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this, scene::SNRT_SHADOW);
}



//! renders the node.
void CShadowVolumeSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (!ShadowVolumesUsed || !driver)
		return;

	driver->setTransform(video::ETS_WORLD, Parent->getAbsoluteTransformation());

	for (s32 i=0; i<ShadowVolumesUsed; ++i)
		driver->drawStencilShadowVolume(ShadowVolumes[i].vertices,
			ShadowVolumes[i].count, UseZFailMethod);
}



//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CShadowVolumeSceneNode::getBoundingBox() const
{
	return Box;
}



//! returns the material based on the zero based index i.
video::SMaterial& CShadowVolumeSceneNode::getMaterial(s32 i)
{
	// this should never be called, because a shadow volume has got no
	// material
	return *((video::SMaterial*)(0));

}



//! returns amount of materials used by this scene node.
s32 CShadowVolumeSceneNode::getMaterialCount()
{
	return 0;
}


//! Generates adjacency information based on mesh indices.
void CShadowVolumeSceneNode::calculateAdjacency(f32 epsilon)
{
	delete [] Adjacency;
	Adjacency = new u16[IndexCount];

	epsilon *= epsilon;

	f32 t = 0;

	// go through all faces and fetch their three neighbours
	for (s32 f=0; f<IndexCount; f+=3)
	{
		for (s32 edge = 0; edge<3; ++edge)
		{
			core::vector3df v1 = Vertices[Indices[f+edge]];
			core::vector3df v2 = Vertices[Indices[f+((edge+1)%3)]];

			// now we search an_O_ther _F_ace with these two 
			// vertices, which is not the current face.

			s32 of;
			
			for (of=0; of<IndexCount; of+=3)
				if (of != f)
				{
					s32 cnt1 = 0;
					s32 cnt2 = 0;

					for (s32 e=0; e<3; ++e)
					{
						t = v1.getDistanceFromSQ(Vertices[Indices[of+e]]);
						if (t <= epsilon && t >= -epsilon)
							++cnt1;

						t = v2.getDistanceFromSQ(Vertices[Indices[of+e]]);
						if (t <= epsilon && t >= -epsilon)
							++cnt2;
					}

					if (cnt1 == 1 && cnt2 == 1)
						break;
				}

			if (of == IndexCount)
				Adjacency[f + edge] = f;
			else
				Adjacency[f + edge] = of / 3;
		}
	}
}


} // end namespace scene
} // end namespace irr
