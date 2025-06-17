// 
// glyph.cpp
//
// Contains matrix glyph code
//

#include "glyph.h"
#include <assert.h>
#include <stdio.h>
#include "bitmap.h"

#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )

Glyph::Glyph( int width, int height )
{
	m_nWidth = width;
	m_nHeight = height;
	m_nScale = 1;

	m_pData = new unsigned char [m_nHeight * m_nWidth * 3];

	for(int i = 0; i < m_nHeight * m_nWidth * 3; i++)
		m_pData[i] = 255;		// default glyph color

}

inline unsigned char *Glyph::getColor(int x, int y) 
{
	if(x >= 0 && x < m_nWidth && y >= 0 && y < m_nHeight)
		return &m_pData[(y*m_nHeight+x)*3];
	else
	{
		printf("Glyph read outside of bounds (%d, %d)\n", x, y);
		return 0;
	}	
}

void Glyph::setColor(int x, int y, const unsigned char *color)
{
	if(x >= 0 && x < m_nWidth && y >= 0 && y < m_nHeight)
		for(int i = 0; i < 3; i++) m_pData[(y*m_nHeight+x)*3+i]=color[i];
	else
		printf("Glyph modified outside of bounds (%d, %d)\n", x, y);
}

void Glyph::draw(int xpos, int ypos, unsigned char *destBuffer, int destBufferWidth)
{
	for( int realx = xpos; realx < xpos + m_nWidth; realx++)
		for( int realy = ypos; realy < ypos + m_nHeight; realy++)
			{							
				unsigned char *color = getColor(realx - xpos, realy - ypos);
				assert(color != 0);
				destBuffer[3*(realy*destBufferWidth+realx)+0] = min((int)((float)color[0] * m_nScale), 255);
				destBuffer[3*(realy*destBufferWidth+realx)+1] = min((int)((float)color[1] * m_nScale), 255);
				destBuffer[3*(realy*destBufferWidth+realx)+2] = min((int)((float)color[2] * m_nScale), 255);
			}
}

void Glyph::scale(float amount)
{
	m_nScale = amount;
}

GlyphSet::GlyphSet(const char* glyphFile, int glyphWidth, int glyphHeight)
{
	// try to open the image to read
	unsigned char*	data;
	int				width, 
					height;

	if ( (data=readBMP(glyphFile, width, height))==NULL ) 
	{
		printf("Can't load glyph bitmap file\n");
		return;
	}

	m_nWidth		= width;
	m_nGlyphWidth	= glyphWidth;
	m_nHeight		= height;
	m_nGlyphHeight	= glyphHeight;
	m_nNumGlyphsVertical = height / glyphHeight;
	m_nNumGlyphsHorizontal = width / glyphWidth;
	m_nNumGlyphs	= m_nNumGlyphsHorizontal * m_nNumGlyphsVertical;

	m_pGlyphs = new Glyph * [m_nNumGlyphs];

	// Read in glyph data from top down and left to right
	for( int i = 0; i < m_nNumGlyphs; i++)
	{
		m_pGlyphs[i] = new Glyph(m_nGlyphWidth, m_nGlyphHeight);
		for( int xOffset = 0; xOffset < m_nGlyphWidth; xOffset++ ) 
			for( int yOffset = 0; yOffset < m_nGlyphHeight; yOffset++ )
			{
				int x = i / m_nNumGlyphsVertical * m_nGlyphWidth + xOffset;
				int y = i % m_nNumGlyphsVertical * m_nGlyphHeight + yOffset;
				assert(x >= 0 && x < m_nWidth);
				assert(y >= 0 && y < m_nHeight);
				assert(i >= 0 && i < m_nNumGlyphs);
				assert(data);
				m_pGlyphs[i]->setColor(xOffset, yOffset, &data[3*(y*m_nWidth+x)]);
			}
	}
}
