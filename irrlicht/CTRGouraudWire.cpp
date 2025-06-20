// Copyright (C) 2002-2004 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "CTRTextureGouraud.h"
#include "SColor.h"

namespace irr
{
namespace video
{

class CTRGouraudWire : public CTRTextureGouraud
{
public:

	CTRGouraudWire(IZBuffer* zbuffer)
		: CTRTextureGouraud(zbuffer)
	{
		#ifdef _DEBUG
		setDebugName("CTRGouraudWire");
		#endif
	}

	//! draws an indexed triangle list
	virtual void drawIndexedTriangleList(S2DVertex* vertices, s32 vertexCount, const u16* indexList, s32 triangleCount)
	{
		const S2DVertex *v1, *v2, *v3;

		f32 tmpDiv; // temporary division factor
		f32 longest; // saves the longest span
		s32 height; // saves height of triangle
		s16* targetSurface; // target pointer where to plot pixels
		s32 spanEnd; // saves end of spans
		f32 leftdeltaxf; // amount of pixels to increase on left side of triangle
		f32 rightdeltaxf; // amount of pixels to increase on right side of triangle
		s32 leftx, rightx; // position where we are 
		f32 leftxf, rightxf; // same as above, but as f32 values
		s32 span; // current span
		s32 leftR, leftG, leftB, rightR, rightG, rightB; // color values
		s32 leftStepR, leftStepG, leftStepB,
			rightStepR, rightStepG, rightStepB; // color steps

		core::rect<s32> TriangleRect;

		s32 leftZValue, rightZValue;
		s32 leftZStep, rightZStep;
		TZBufferType* zTarget; // target of ZBuffer;

		lockedSurface = (s16*)RenderTarget->lock();
		lockedZBuffer = ZBuffer->lock();

		for (s32 i=0; i<triangleCount; ++i)
		{
			v1 = &vertices[*indexList];
			++indexList;
			v2 = &vertices[*indexList];
			++indexList;
			v3 = &vertices[*indexList];
			++indexList;

			// back face culling

			if (BackFaceCullingEnabled)
			{
				s32 z = ((v3->Pos.X - v1->Pos.X) * (v3->Pos.Y - v2->Pos.Y)) -
					((v3->Pos.Y - v1->Pos.Y) * (v3->Pos.X - v2->Pos.X));

				if (z < 0)
					continue;
			}

			//near plane clipping

			if (v1->ZValue<0 && v2->ZValue<0 && v3->ZValue<0)
				continue;

			// sort for width for inscreen clipping

			if (v1->Pos.X > v2->Pos.X)	swapVertices(&v1, &v2);
			if (v1->Pos.X > v3->Pos.X)	swapVertices(&v1, &v3);
			if (v2->Pos.X > v3->Pos.X)	swapVertices(&v2, &v3);

			if ((v1->Pos.X - v3->Pos.X) == 0)
				continue;

			TriangleRect.UpperLeftCorner.X = v1->Pos.X;
			TriangleRect.LowerRightCorner.X = v3->Pos.X;

			// sort for height for faster drawing.

			if (v1->Pos.Y > v2->Pos.Y)	swapVertices(&v1, &v2);
			if (v1->Pos.Y > v3->Pos.Y)	swapVertices(&v1, &v3);
			if (v2->Pos.Y > v3->Pos.Y)	swapVertices(&v2, &v3);

			TriangleRect.UpperLeftCorner.Y = v1->Pos.Y;
			TriangleRect.LowerRightCorner.Y = v3->Pos.Y;

			if (!TriangleRect.isRectCollided(ViewPortRect))
				continue;


			// h�he des dreiecks berechnen
			height = v3->Pos.Y - v1->Pos.Y;
			if (!height)
				continue;

			// calculate longest span

			longest = (v2->Pos.Y - v1->Pos.Y) / (f32)height * (v3->Pos.X - v1->Pos.X) + (v1->Pos.X - v2->Pos.X);

			spanEnd = v2->Pos.Y;
			span = v1->Pos.Y;
			leftxf = (f32)v1->Pos.X;
			rightxf = (f32)v1->Pos.X;

			leftZValue = v1->ZValue;
			rightZValue = v1->ZValue;

			leftR = rightR = video::getRed(v1->Color)<<8;
			leftG = rightG = video::getGreen(v1->Color)<<8;
			leftB = rightB = video::getBlue(v1->Color)<<8;

			targetSurface = lockedSurface + span * SurfaceWidth;
			zTarget = lockedZBuffer + span * SurfaceWidth;

			if (longest < 0.0f)
			{
				tmpDiv = 1.0f / (f32)(v2->Pos.Y - v1->Pos.Y);
				rightdeltaxf = (v2->Pos.X - v1->Pos.X) * tmpDiv;
				rightZStep = (s32)((v2->ZValue - v1->ZValue) * tmpDiv);
				rightStepR = (s32)(((video::getRed(v2->Color)<<8) - rightR) * tmpDiv);
				rightStepG = (s32)(((video::getGreen(v2->Color)<<8) - rightG) * tmpDiv);
				rightStepB = (s32)(((video::getBlue(v2->Color)<<8) - rightB) * tmpDiv);

				tmpDiv = 1.0f / (f32)height;
				leftdeltaxf = (v3->Pos.X - v1->Pos.X) * tmpDiv;
				leftZStep = (s32)((v3->ZValue - v1->ZValue) * tmpDiv);
				leftStepR = (s32)(((video::getRed(v3->Color)<<8) - leftR) * tmpDiv);
				leftStepG = (s32)(((video::getGreen(v3->Color)<<8) - leftG) * tmpDiv);
				leftStepB = (s32)(((video::getBlue(v3->Color)<<8) - leftB) * tmpDiv);
			}
			else
			{
				tmpDiv = 1.0f / (f32)height;
				rightdeltaxf = (v3->Pos.X - v1->Pos.X) * tmpDiv;
				rightZStep = (s32)((v3->ZValue - v1->ZValue) * tmpDiv);
				rightStepR = (s32)(((video::getRed(v3->Color)<<8) - rightR) * tmpDiv);
				rightStepG = (s32)(((video::getGreen(v3->Color)<<8) - rightG) * tmpDiv);
				rightStepB = (s32)(((video::getBlue(v3->Color)<<8) - rightB) * tmpDiv);

				tmpDiv = 1.0f / (f32)(v2->Pos.Y - v1->Pos.Y);
				leftdeltaxf = (v2->Pos.X - v1->Pos.X) * tmpDiv;
				leftZStep = (s32)((v2->ZValue - v1->ZValue) * tmpDiv);
				leftStepR = (s32)(((video::getRed(v2->Color)<<8) - leftR) * tmpDiv);
				leftStepG = (s32)(((video::getGreen(v2->Color)<<8) - leftG) * tmpDiv);
				leftStepB = (s32)(((video::getBlue(v2->Color)<<8) - leftB) * tmpDiv);
			}


			// do it twice, once for the first half of the triangle,
			// end then for the second half.

			for (s32 triangleHalf=0; triangleHalf<2; ++triangleHalf)
			{
				if (spanEnd > ViewPortRect.LowerRightCorner.Y)
					spanEnd = ViewPortRect.LowerRightCorner.Y;

				// if the span <0, than we can skip these spans, 
				// and proceed to the next spans which are really on the screen.
				if (span < ViewPortRect.UpperLeftCorner.Y)
				{
					// we'll use leftx as temp variable
					if (spanEnd < ViewPortRect.UpperLeftCorner.Y)
					{
						leftx = spanEnd - span;
						span = spanEnd;
					}
					else
					{
						leftx = ViewPortRect.UpperLeftCorner.Y - span; 
						span = ViewPortRect.UpperLeftCorner.Y;
					}

					leftxf += leftdeltaxf*leftx;
					rightxf += rightdeltaxf*leftx;
					targetSurface += SurfaceWidth*leftx;
					zTarget += SurfaceWidth*leftx;
					leftZValue += leftZStep*leftx;
					rightZValue += rightZStep*leftx;

					leftR += leftStepR*leftx;
					leftG += leftStepG*leftx;
					leftB += leftStepB*leftx;
					rightR += rightStepR*leftx;
					rightG += rightStepG*leftx;
					rightB += rightStepB*leftx;
				}


				// the main loop. Go through every span and draw it.

				while (span < spanEnd)
				{
					leftx = (s32)(leftxf);
					rightx = (s32)(rightxf + 0.5f);

					// perform some clipping

					if (leftx>=ViewPortRect.UpperLeftCorner.X &&
						leftx<=ViewPortRect.LowerRightCorner.X)
					{
						if (leftZValue > *(zTarget + leftx))
						{
							*(zTarget + leftx) = leftZValue;
							*(targetSurface + leftx) = (((leftR>>8) & 0x1F)<<10) | (((leftG>>8) & 0x1F)<<5) | ((leftB>>8) & 0x1F);
						}
					}


					if (rightx>=ViewPortRect.UpperLeftCorner.X &&
						rightx<=ViewPortRect.LowerRightCorner.X)
					{
						if (rightZValue > *(zTarget + rightx))
						{
							*(zTarget + rightx) = rightZValue;
							*(targetSurface + rightx) = (((rightR>>8) & 0x1F)<<10) | (((rightG>>8) & 0x1F)<<5) | ((rightB>>8) & 0x1F);
						}

					}

					leftxf += leftdeltaxf;
					rightxf += rightdeltaxf;
					++span;
					targetSurface += SurfaceWidth;
					zTarget += SurfaceWidth;
					leftZValue += leftZStep;
					rightZValue += rightZStep;

					leftR += leftStepR;
					leftG += leftStepG;
					leftB += leftStepB;
					rightR += rightStepR;
					rightG += rightStepG;
					rightB += rightStepB;
				}

				if (triangleHalf>0) // break, we've gout only two halves
					break;


				// setup variables for second half of the triangle.

				if (longest < 0.0f)
				{
					tmpDiv = 1.0f / (v3->Pos.Y - v2->Pos.Y);

					rightdeltaxf = (v3->Pos.X - v2->Pos.X) * tmpDiv;
					rightxf = (f32)v2->Pos.X;

					rightZValue = v2->ZValue;
					rightZStep = (s32)((v3->ZValue - v2->ZValue) * tmpDiv);

					rightR = video::getRed(v2->Color)<<8;
					rightG = video::getGreen(v2->Color)<<8;
					rightB = video::getBlue(v2->Color)<<8;
					rightStepR = (s32)(((video::getRed(v3->Color)<<8) - rightR) * tmpDiv);
					rightStepG = (s32)(((video::getGreen(v3->Color)<<8) - rightG) * tmpDiv);
					rightStepB = (s32)(((video::getBlue(v3->Color)<<8) - rightB) * tmpDiv);
				}
				else
				{
					tmpDiv = 1.0f / (v3->Pos.Y - v2->Pos.Y);

					leftdeltaxf = (v3->Pos.X - v2->Pos.X) * tmpDiv;
					leftxf = (f32)v2->Pos.X;

					leftZValue = v2->ZValue;
					leftZStep = (s32)((v3->ZValue - v2->ZValue) * tmpDiv);

					leftR = video::getRed(v2->Color)<<8;
					leftG = video::getGreen(v2->Color)<<8;
					leftB = video::getBlue(v2->Color)<<8;
					leftStepR = (s32)(((video::getRed(v3->Color)<<8) - leftR) * tmpDiv);
					leftStepG = (s32)(((video::getGreen(v3->Color)<<8) - leftG) * tmpDiv);
					leftStepB = (s32)(((video::getBlue(v3->Color)<<8) - leftB) * tmpDiv);
				}


				spanEnd = v3->Pos.Y;
			}

		}

		RenderTarget->unlock();
		ZBuffer->unlock();
	}

};


//! creates a flat triangle renderer
IK3DTriangleRenderer* createTriangleRendererGouraudWire(IZBuffer* zbuffer)
{
	return new CTRGouraudWire(zbuffer);
}

} // end namespace video
} // end namespace irr
