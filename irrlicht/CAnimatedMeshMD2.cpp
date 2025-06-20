// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CAnimatedMeshMD2.h"
#include "os.h"
#include "SColor.h"
#include "IReadFile.h"

namespace irr
{
namespace scene
{

#ifdef _MSC_VER
#	pragma pack( push, packing )
#	pragma pack( 1 )
#	define PACK_STRUCT
#elif defined( __GNUC__ )
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error compiler not supported
#endif

	// structs needed to load the md2-format

	const s32 MD2_MAGIC_NUMBER = 844121161;
	const s32 MD2_VERSION		= 8;
	const s32 MD2_MAX_VERTS		= 2048;
	const s32 MD2_FRAME_SHIFT = 3;

	struct SMD2Header
	{
		s32 magic;
		s32 version;
		s32 skinWidth;
		s32 skinHeight;
		s32 frameSize;
		s32 numSkins;
		s32 numVertices;
		s32 numTexcoords;
		s32 numTriangles;	
		s32 numGlCommands;
		s32 numFrames;
		s32 offsetSkins;
		s32 offsetTexcoords;
		s32 offsetTriangles;
		s32 offsetFrames;
		s32 offsetGlCommands;
		s32 offsetEnd;
	} PACK_STRUCT;

	struct SMD2Vertex
	{
		u8 vertex[3];
		u8 lightNormalIndex;
	} PACK_STRUCT;

	struct SMD2Frame
	{
		f32	scale[3];
		f32	translate[3];
		c8	name[16];
		SMD2Vertex vertices[1];
	} PACK_STRUCT;

	struct SMD2Triangle
	{
		s16 vertexIndices[3];
		s16 textureIndices[3];
	} PACK_STRUCT;

	struct SMD2TextureCoordinate
	{
		s16 s;
		s16 t;
	} PACK_STRUCT;

	struct SMD2GLCommand
	{
		f32 s, t;
		s32 vertexIndex;
	} PACK_STRUCT;

// Default alignment
#ifdef _MSC_VER
#	pragma pack( pop, packing )
#endif

#undef PACK_STRUCT


const s32 Q2_VERTEX_NORMAL_TABLE_SIZE = 162;

const f32 Q2_VERTEX_NORMAL_TABLE[Q2_VERTEX_NORMAL_TABLE_SIZE][3] = {
	{-0.525731f, 0.000000f, 0.850651f}, 
	{-0.442863f, 0.238856f, 0.864188f}, 
	{-0.295242f, 0.000000f, 0.955423f}, 
	{-0.309017f, 0.500000f, 0.809017f}, 
	{-0.162460f, 0.262866f, 0.951056f}, 
	{0.000000f, 0.000000f, 1.000000f}, 
	{0.000000f, 0.850651f, 0.525731f}, 
	{-0.147621f, 0.716567f, 0.681718f}, 
	{0.147621f, 0.716567f, 0.681718f}, 
	{0.000000f, 0.525731f, 0.850651f}, 
	{0.309017f, 0.500000f, 0.809017f}, 
	{0.525731f, 0.000000f, 0.850651f}, 
	{0.295242f, 0.000000f, 0.955423f}, 
	{0.442863f, 0.238856f, 0.864188f}, 
	{0.162460f, 0.262866f, 0.951056f}, 
	{-0.681718f, 0.147621f, 0.716567f}, 
	{-0.809017f, 0.309017f, 0.500000f}, 
	{-0.587785f, 0.425325f, 0.688191f}, 
	{-0.850651f, 0.525731f, 0.000000f}, 
	{-0.864188f, 0.442863f, 0.238856f}, 
	{-0.716567f, 0.681718f, 0.147621f}, 
	{-0.688191f, 0.587785f, 0.425325f}, 
	{-0.500000f, 0.809017f, 0.309017f}, 
	{-0.238856f, 0.864188f, 0.442863f}, 
	{-0.425325f, 0.688191f, 0.587785f}, 
	{-0.716567f, 0.681718f, -0.147621f}, 
	{-0.500000f, 0.809017f, -0.309017f}, 
	{-0.525731f, 0.850651f, 0.000000f}, 
	{0.000000f, 0.850651f, -0.525731f}, 
	{-0.238856f, 0.864188f, -0.442863f}, 
	{0.000000f, 0.955423f, -0.295242f}, 
	{-0.262866f, 0.951056f, -0.162460f}, 
	{0.000000f, 1.000000f, 0.000000f}, 
	{0.000000f, 0.955423f, 0.295242f}, 
	{-0.262866f, 0.951056f, 0.162460f}, 
	{0.238856f, 0.864188f, 0.442863f}, 
	{0.262866f, 0.951056f, 0.162460f}, 
	{0.500000f, 0.809017f, 0.309017f}, 
	{0.238856f, 0.864188f, -0.442863f}, 
	{0.262866f, 0.951056f, -0.162460f}, 
	{0.500000f, 0.809017f, -0.309017f}, 
	{0.850651f, 0.525731f, 0.000000f}, 
	{0.716567f, 0.681718f, 0.147621f}, 
	{0.716567f, 0.681718f, -0.147621f}, 
	{0.525731f, 0.850651f, 0.000000f}, 
	{0.425325f, 0.688191f, 0.587785f}, 
	{0.864188f, 0.442863f, 0.238856f}, 
	{0.688191f, 0.587785f, 0.425325f}, 
	{0.809017f, 0.309017f, 0.500000f}, 
	{0.681718f, 0.147621f, 0.716567f}, 
	{0.587785f, 0.425325f, 0.688191f}, 
	{0.955423f, 0.295242f, 0.000000f}, 
	{1.000000f, 0.000000f, 0.000000f}, 
	{0.951056f, 0.162460f, 0.262866f}, 
	{0.850651f, -0.525731f, 0.000000f}, 
	{0.955423f, -0.295242f, 0.000000f}, 
	{0.864188f, -0.442863f, 0.238856f}, 
	{0.951056f, -0.162460f, 0.262866f}, 
	{0.809017f, -0.309017f, 0.500000f}, 
	{0.681718f, -0.147621f, 0.716567f}, 
	{0.850651f, 0.000000f, 0.525731f}, 
	{0.864188f, 0.442863f, -0.238856f}, 
	{0.809017f, 0.309017f, -0.500000f}, 
	{0.951056f, 0.162460f, -0.262866f}, 
	{0.525731f, 0.000000f, -0.850651f}, 
	{0.681718f, 0.147621f, -0.716567f}, 
	{0.681718f, -0.147621f, -0.716567f}, 
	{0.850651f, 0.000000f, -0.525731f}, 
	{0.809017f, -0.309017f, -0.500000f}, 
	{0.864188f, -0.442863f, -0.238856f}, 
	{0.951056f, -0.162460f, -0.262866f}, 
	{0.147621f, 0.716567f, -0.681718f}, 
	{0.309017f, 0.500000f, -0.809017f}, 
	{0.425325f, 0.688191f, -0.587785f}, 
	{0.442863f, 0.238856f, -0.864188f}, 
	{0.587785f, 0.425325f, -0.688191f}, 
	{0.688191f, 0.587785f, -0.425325f}, 
	{-0.147621f, 0.716567f, -0.681718f}, 
	{-0.309017f, 0.500000f, -0.809017f}, 
	{0.000000f, 0.525731f, -0.850651f}, 
	{-0.525731f, 0.000000f, -0.850651f}, 
	{-0.442863f, 0.238856f, -0.864188f}, 
	{-0.295242f, 0.000000f, -0.955423f}, 
	{-0.162460f, 0.262866f, -0.951056f}, 
	{0.000000f, 0.000000f, -1.000000f}, 
	{0.295242f, 0.000000f, -0.955423f}, 
	{0.162460f, 0.262866f, -0.951056f}, 
	{-0.442863f, -0.238856f, -0.864188f}, 
	{-0.309017f, -0.500000f, -0.809017f}, 
	{-0.162460f, -0.262866f, -0.951056f}, 
	{0.000000f, -0.850651f, -0.525731f}, 
	{-0.147621f, -0.716567f, -0.681718f}, 
	{0.147621f, -0.716567f, -0.681718f}, 
	{0.000000f, -0.525731f, -0.850651f}, 
	{0.309017f, -0.500000f, -0.809017f}, 
	{0.442863f, -0.238856f, -0.864188f}, 
	{0.162460f, -0.262866f, -0.951056f}, 
	{0.238856f, -0.864188f, -0.442863f}, 
	{0.500000f, -0.809017f, -0.309017f}, 
	{0.425325f, -0.688191f, -0.587785f}, 
	{0.716567f, -0.681718f, -0.147621f}, 
	{0.688191f, -0.587785f, -0.425325f}, 
	{0.587785f, -0.425325f, -0.688191f}, 
	{0.000000f, -0.955423f, -0.295242f}, 
	{0.000000f, -1.000000f, 0.000000f}, 
	{0.262866f, -0.951056f, -0.162460f}, 
	{0.000000f, -0.850651f, 0.525731f}, 
	{0.000000f, -0.955423f, 0.295242f}, 
	{0.238856f, -0.864188f, 0.442863f}, 
	{0.262866f, -0.951056f, 0.162460f}, 
	{0.500000f, -0.809017f, 0.309017f}, 
	{0.716567f, -0.681718f, 0.147621f}, 
	{0.525731f, -0.850651f, 0.000000f}, 
	{-0.238856f, -0.864188f, -0.442863f}, 
	{-0.500000f, -0.809017f, -0.309017f}, 
	{-0.262866f, -0.951056f, -0.162460f}, 
	{-0.850651f, -0.525731f, 0.000000f}, 
	{-0.716567f, -0.681718f, -0.147621f}, 
	{-0.716567f, -0.681718f, 0.147621f}, 
	{-0.525731f, -0.850651f, 0.000000f}, 
	{-0.500000f, -0.809017f, 0.309017f}, 
	{-0.238856f, -0.864188f, 0.442863f}, 
	{-0.262866f, -0.951056f, 0.162460f}, 
	{-0.864188f, -0.442863f, 0.238856f}, 
	{-0.809017f, -0.309017f, 0.500000f}, 
	{-0.688191f, -0.587785f, 0.425325f}, 
	{-0.681718f, -0.147621f, 0.716567f}, 
	{-0.442863f, -0.238856f, 0.864188f}, 
	{-0.587785f, -0.425325f, 0.688191f}, 
	{-0.309017f, -0.500000f, 0.809017f}, 
	{-0.147621f, -0.716567f, 0.681718f}, 
	{-0.425325f, -0.688191f, 0.587785f}, 
	{-0.162460f, -0.262866f, 0.951056f}, 
	{0.442863f, -0.238856f, 0.864188f}, 
	{0.162460f, -0.262866f, 0.951056f}, 
	{0.309017f, -0.500000f, 0.809017f}, 
	{0.147621f, -0.716567f, 0.681718f}, 
	{0.000000f, -0.525731f, 0.850651f}, 
	{0.425325f, -0.688191f, 0.587785f}, 
	{0.587785f, -0.425325f, 0.688191f}, 
	{0.688191f, -0.587785f, 0.425325f}, 
	{-0.955423f, 0.295242f, 0.000000f}, 
	{-0.951056f, 0.162460f, 0.262866f}, 
	{-1.000000f, 0.000000f, 0.000000f}, 
	{-0.850651f, 0.000000f, 0.525731f}, 
	{-0.955423f, -0.295242f, 0.000000f}, 
	{-0.951056f, -0.162460f, 0.262866f}, 
	{-0.864188f, 0.442863f, -0.238856f}, 
	{-0.951056f, 0.162460f, -0.262866f}, 
	{-0.809017f, 0.309017f, -0.500000f}, 
	{-0.864188f, -0.442863f, -0.238856f}, 
	{-0.951056f, -0.162460f, -0.262866f}, 
	{-0.809017f, -0.309017f, -0.500000f}, 
	{-0.681718f, 0.147621f, -0.716567f}, 
	{-0.681718f, -0.147621f, -0.716567f}, 
	{-0.850651f, 0.000000f, -0.525731f}, 
	{-0.688191f, 0.587785f, -0.425325f}, 
	{-0.587785f, 0.425325f, -0.688191f}, 
	{-0.425325f, 0.688191f, -0.587785f}, 
	{-0.425325f, -0.688191f, -0.587785f}, 
	{-0.587785f, -0.425325f, -0.688191f}, 
	{-0.688191f, -0.587785f, -0.425325f}, 
	};

struct SMD2AnimationType
{
	s32 begin;
	s32 end;
	s32 fps;
};

SMD2AnimationType MD2AnimationTypeList[21] =
{
    {   0,  39,  9 },   // STAND
    {  40,  45, 10 },   // RUN
    {  46,  53, 10 },   // ATTACK
    {  54,  57,  7 },   // PAIN_A
    {  58,  61,  7 },   // PAIN_B
    {  62,  65,  7 },   // PAIN_C
    {  66,  71,  7 },   // JUMP
    {  72,  83,  7 },   // FLIP
    {  84,  94,  7 },   // SALUTE
    {  95, 111, 10 },   // FALLBACK
    { 112, 122,  7 },   // WAVE
    { 123, 134,  6 },   // POINT
    { 135, 153, 10 },   // CROUCH_STAND
    { 154, 159,  7 },   // CROUCH_WALK
    { 160, 168, 10 },   // CROUCH_ATTACK
    { 196, 172,  7 },   // CROUCH_PAIN
    { 173, 177,  5 },   // CROUCH_DEATH
    { 178, 183,  7 },   // DEATH_FALLBACK
    { 184, 189,  7 },   // DEATH_FALLFORWARD
    { 190, 197,  7 },   // DEATH_FALLBACKSLOW
    { 198, 198,  5 },   // BOOM
};


//! constructor
CAnimatedMeshMD2::CAnimatedMeshMD2()
: FrameCount(0), FrameList(0)
{
	#ifdef _DEBUG
	IAnimatedMesh::setDebugName("CAnimatedMeshMD2 IAnimatedMesh");
	IMesh::setDebugName("CAnimatedMeshMD2 IMesh");
	IMeshBuffer::setDebugName("CAnimatedMeshMD2 IMeshBuffer");
	#endif	
}



//! destructor
CAnimatedMeshMD2::~CAnimatedMeshMD2()
{
	if (FrameList)
		delete [] FrameList;
}



//! returns the amount of frames in milliseconds. If the amount is 1, it is a static (=non animated) mesh.
s32 CAnimatedMeshMD2::getFrameCount()
{
	return FrameCount<<MD2_FRAME_SHIFT;
}



//! returns the animated mesh based on a detail level. 0 is the lowest, 255 the highest detail. Note, that some Meshes will ignore the detail level.
IMesh* CAnimatedMeshMD2::getMesh(s32 frame, s32 detailLevel, s32 startFrameLoop, s32 endFrameLoop)
{
	if ((u32)frame > (FrameCount<<MD2_FRAME_SHIFT))
		frame = (frame % (FrameCount<<MD2_FRAME_SHIFT));

	if (startFrameLoop == -1 && endFrameLoop == -1)
	{
		startFrameLoop = 0;
		endFrameLoop = FrameCount<<MD2_FRAME_SHIFT;
	}

	updateInterpolationBuffer(frame, startFrameLoop, endFrameLoop);
	return this;
}


//! returns amount of mesh buffers.
s32 getMeshBufferCount()
{
	return 1;
}


//! returns pointer to a mesh buffer
IMeshBuffer* CAnimatedMeshMD2::getMeshBuffer(s32 nr)
{
	return this;
}



//! returns the material of this meshbuffer
const video::SMaterial& CAnimatedMeshMD2::getMaterial() const
{
	return Material;
}


//! returns the material of this meshbuffer
video::SMaterial& CAnimatedMeshMD2::getMaterial()
{
	return Material;
}



//! returns pointer to vertices
const void* CAnimatedMeshMD2::getVertices() const
{
	return InterpolateBuffer.const_pointer();
}


//! returns pointer to vertices
void* CAnimatedMeshMD2::getVertices()
{
	return InterpolateBuffer.pointer();
}



//! returns which type of vertex data is stored.
video::E_VERTEX_TYPE CAnimatedMeshMD2::getVertexType() const
{
	return video::EVT_STANDARD;
}



//! returns amount of vertices
s32 CAnimatedMeshMD2::getVertexCount() const
{
	return FrameList[0].size();
}



//! returns pointer to Indices
const u16* CAnimatedMeshMD2::getIndices() const
{
	return Indices.const_pointer();
}



//! returns pointer to Indices
u16* CAnimatedMeshMD2::getIndices()
{
	return Indices.pointer();
}



//! returns amount of indices
s32 CAnimatedMeshMD2::getIndexCount() const
{
	return Indices.size();
}


//! returns amount of mesh buffers.
s32 CAnimatedMeshMD2::getMeshBufferCount()
{
	return 1;
}


// updates the interpolation buffer
void CAnimatedMeshMD2::updateInterpolationBuffer(s32 frame, s32 startFrameLoop, s32 endFrameLoop)
{
	u32 firstFrame, secondFrame;
	f32 div;

	if (endFrameLoop - startFrameLoop == 0)
	{
		firstFrame = frame>>MD2_FRAME_SHIFT;
		secondFrame = frame>>MD2_FRAME_SHIFT;
		div = 1.0f;
	}
	else
	{
		firstFrame = frame>>MD2_FRAME_SHIFT;

		if (!((endFrameLoop>>MD2_FRAME_SHIFT) - (startFrameLoop>>MD2_FRAME_SHIFT)))
			secondFrame = firstFrame;
		else
		secondFrame = (startFrameLoop>>MD2_FRAME_SHIFT) + (((frame>>MD2_FRAME_SHIFT)+1 - (startFrameLoop>>MD2_FRAME_SHIFT)) % 
			((endFrameLoop>>MD2_FRAME_SHIFT) - (startFrameLoop>>MD2_FRAME_SHIFT)));

		div = (frame % (1<<MD2_FRAME_SHIFT)) / (f32)(1<<MD2_FRAME_SHIFT);
	}
		
	video::S3DVertex* target = &InterpolateBuffer[0];
	video::S3DVertex* first = FrameList[firstFrame].pointer();
	video::S3DVertex* second = FrameList[secondFrame].pointer();

	s32 count = FrameList[firstFrame].size();

	// interpolate both frames
	for (s32 i=0; i<count; ++i)
	{
		target->Pos = (second->Pos - first->Pos) * div + first->Pos;
		target->Normal = (second->Normal - first->Normal) * div + first->Normal;

		++target;
		++first;
		++second;
	}

	//update bounding box
	BoundingBox = BoxList[secondFrame].getInterpolated(BoxList[firstFrame], div);
}



//! returns max element
inline s32 CAnimatedMeshMD2::max(s32 a, s32 b)
{
	return a>b ? a : b;
}


//! loads an md3 file
bool CAnimatedMeshMD2::loadFile(io::IReadFile* file)
{
	if (!file)
		return false;

	SMD2Header header;
	s8* frames = 0;

	file->read(&header, sizeof(SMD2Header));

	if (header.magic != MD2_MAGIC_NUMBER || header.version != MD2_VERSION)
	{
		os::Printer::log("MD2 Loader: Wrong file header", file->getFileName(), ELL_WARNING);
		return false;
	}

	// create Memory for indices and frames

	Indices.reallocate(header.numTriangles);
	FrameList = new core::array<video::S3DVertex>[header.numFrames];
	FrameCount = header.numFrames;

	s32 i;
	
	for (i=0; i<header.numFrames; ++i)
		FrameList[i].reallocate(header.numVertices);

	// read TextureCoords

	file->seek(header.offsetTexcoords, false);
	SMD2TextureCoordinate* textureCoords = new SMD2TextureCoordinate[header.numTexcoords];

	if (!file->read(textureCoords, sizeof(SMD2TextureCoordinate)*header.numTexcoords))
	{
		os::Printer::log("MD2 Loader: Error reading TextureCoords.", file->getFileName(), ELL_ERROR);
		return false;
	}

	// read Triangles

	file->seek(header.offsetTriangles, false);

	SMD2Triangle *triangles = new SMD2Triangle[header.numTriangles];
	if (!file->read(triangles, header.numTriangles *sizeof(SMD2Triangle)))
	{
		os::Printer::log("MD2 Loader: Error reading triangles.", file->getFileName(), ELL_ERROR);
		return false;
	}

	// read Vertices

	s8 buffer[MD2_MAX_VERTS*4+128];
	SMD2Frame* frame = (SMD2Frame*)buffer;

	core::array< core::vector3df >* vertices = new core::array< core::vector3df >[header.numFrames];
	core::array< core::vector3df >* normals = new core::array< core::vector3df >[header.numFrames];

	file->seek(header.offsetFrames, false);

	for (i = 0; i<header.numFrames; ++i)
	{
		// read vertices

		vertices[i].reallocate(header.numVertices);
		file->read(frame, header.frameSize);

		// store frame data

		SFrameData fdata;
		fdata.begin = i;
		fdata.end = i;
		fdata.fps = 7;

		if (frame->name[0])
		{
			for (s32 s = 0; frame->name[s]!=0 && (frame->name[s] < '0' ||
				frame->name[s] > '9'); ++s)
				fdata.name += frame->name[s];

			if (!FrameData.empty() && FrameData[FrameData.size()-1].name == fdata.name)
				++FrameData[FrameData.size()-1].end;
			else
				FrameData.push_back(fdata);
		}

		// add vertices

		for (s32 j=0; j<header.numVertices; ++j)
		{
			core::vector3df v;
			v.X = (f32)frame->vertices[j].vertex[0] * frame->scale[0] + frame->translate[0];
			v.Z = (f32)frame->vertices[j].vertex[1] * frame->scale[1] + frame->translate[1];
			v.Y = (f32)frame->vertices[j].vertex[2] * frame->scale[2] + frame->translate[2];

			vertices[i].push_back(v);

			s32 normalidx = frame->vertices[j].lightNormalIndex;
			if (normalidx > 0 && normalidx < Q2_VERTEX_NORMAL_TABLE_SIZE)
			{
				v.X = Q2_VERTEX_NORMAL_TABLE[normalidx][0];
				v.Y = Q2_VERTEX_NORMAL_TABLE[normalidx][1];
				v.Z = Q2_VERTEX_NORMAL_TABLE[normalidx][2];
			}	

			normals[i].push_back(v);
		}

		// calculate bounding boxes
		if (header.numVertices)
		{
			core::aabbox3d<f32> box;
			box.reset(vertices[i][0]);

			for (s32 j=1; j<header.numVertices; ++j)
				box.addInternalPoint(vertices[i][j]);

			BoxList.push_back(box);
		}
			
	}

	// put triangles into frame list
	
	f32 dmaxs = 1.0f/(header.skinWidth);
	f32 dmaxt = 1.0f/(header.skinHeight);

	video::S3DVertex vtx;
	vtx.Color = video::SColor(255,255,255,255);
	
	for (s32 f = 0; f<header.numFrames; ++f)
	{
		core::array< core::vector3df >& vert = vertices[f];

		for (s32 t=0; t<header.numTriangles; ++t)
		{
			for (s32 n=0; n<3; ++n)
			{
				vtx.Pos = vert[triangles[t].vertexIndices[n]];
				vtx.TCoords.X = (textureCoords[triangles[t].textureIndices[n]].s + 0.5f) * dmaxs;
				vtx.TCoords.Y = (textureCoords[triangles[t].textureIndices[n]].t + 0.5f) * dmaxt;
				vtx.Normal = normals[f].pointer()[triangles[t].vertexIndices[n]];
				FrameList[f].push_back(vtx);
			}
		}
	}

	// create indices

	Indices.reallocate(header.numVertices);
	s32 count = header.numTriangles*3;
	for (s32 n=0; n<count; n+=3)
	{
		Indices.push_back(n);
		Indices.push_back(n+1);
		Indices.push_back(n+2);
	}

	//calculateNormals();

	// reallocate interpolate buffer
	if (header.numFrames)
	{
		InterpolateBuffer.set_used(FrameList[0].size());

		s32 count = FrameList[0].size();
		for (i=0; i<count; ++i)
		{
			InterpolateBuffer[i].TCoords = FrameList[0].pointer()[i].TCoords;
			InterpolateBuffer[i].Color = vtx.Color;
		}
	}

	// clean up

	delete [] normals;
	delete [] vertices;
	delete [] triangles;
	delete [] textureCoords;
	TriangleCount = Indices.size() / 3;

	// return

	calculateBoundingBox();

	return true;
}



//! calculates the bounding box
void CAnimatedMeshMD2::calculateBoundingBox()
{
	BoundingBox.reset(0,0,0);

	if (FrameCount)
	{
		u32 defaultFrame = 1;

		if (defaultFrame>=FrameCount)
			defaultFrame = 0;

			for (u32 j=0; j<FrameList[defaultFrame].size(); ++j)
				BoundingBox.addInternalPoint(FrameList[defaultFrame].pointer()[j].Pos);
	}
}


//! sets a flag of all contained materials to a new value
void CAnimatedMeshMD2::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	Material.Flags[flag] = newvalue;
}



//! returns an axis aligned bounding box
const core::aabbox3d<f32>& CAnimatedMeshMD2::getBoundingBox() const
{
	return BoundingBox;
}



//! returns an axis aligned bounding box
core::aabbox3d<f32>& CAnimatedMeshMD2::getBoundingBox()
{
	return BoundingBox;
}



//! calculates normals
void CAnimatedMeshMD2::calculateNormals()
{
	for (u32 i=0; i<FrameCount; ++i)
	{
		video::S3DVertex* vtx = FrameList[i].pointer();

		for (u32 j=0; j<Indices.size(); j+=3)
		{
			core::plane3d<f32> plane(
				vtx[Indices[j]].Pos, vtx[Indices[j+1]].Pos,	vtx[Indices[j+2]].Pos);

			vtx[Indices[j]].Normal = plane.Normal;
			vtx[Indices[j+1]].Normal = plane.Normal;
			vtx[Indices[j+2]].Normal = plane.Normal;
		}
	}
}


//! Returns the type of the animated mesh.
EANIMATED_MESH_TYPE CAnimatedMeshMD2::getMeshType() const
{
	return EAMT_MD2;
}


//! Returns frame loop data for a special MD2 animation type.
void CAnimatedMeshMD2::getFrameLoop(EMD2_ANIMATION_TYPE l, 
									s32& outBegin, s32& outEnd, s32& outFPS) const
{
	if (l < 0 || l >= EMAT_COUNT)
		return;

	outBegin = MD2AnimationTypeList[l].begin << MD2_FRAME_SHIFT;
	outEnd = MD2AnimationTypeList[l].end << MD2_FRAME_SHIFT;
	outFPS = MD2AnimationTypeList[l].fps * 5;
}


//! Returns frame loop data for a special MD2 animation type.
bool CAnimatedMeshMD2::getFrameLoop(const c8* name, 
	s32& outBegin, s32&outEnd, s32& outFps) const
{
	for (s32 i=0; i<(s32)FrameData.size(); ++i)
		if (FrameData[i].name == name)
		{
			outBegin = FrameData[i].begin << MD2_FRAME_SHIFT;
			outEnd = FrameData[i].end << MD2_FRAME_SHIFT;
			outFps = FrameData[i].fps * 5;
			return true;
		}
	
	return false;
}



//! Returns amount of md2 animations in this file.
s32 CAnimatedMeshMD2::getAnimationCount() const
{
	return FrameData.size();
}


//! Returns name of md2 animation. 
const c8* CAnimatedMeshMD2::getAnimationName(s32 nr) const
{
	if (nr < 0 || nr >= (s32)FrameData.size())
		return 0;

	return FrameData[nr].name.c_str();
}


} // end namespace scene
} // end namespace irr

