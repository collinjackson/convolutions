// 
// glyph.h
//
// header file for matrix glyphs
//

#ifndef Glyph_h
#define Glyph_h


class Glyph
{
public:
	Glyph( int width, int height );
	~Glyph() { delete m_pData; }

	void setColor(int x, int y, const unsigned char *color);
	inline unsigned char *getColor(int x, int y);

	// Insert the glyph into the target buffer
	void draw(int xpos, int ypos, unsigned char *destBuffer, int destBufferWidth);

	// Modify the brightness by some multiplier
	void scale(float amount);

private:
	int m_nWidth;
	int m_nHeight;
	float m_nScale;

	// Color data for the glyph
	unsigned char *m_pData;

};

class GlyphSet
{
public:
	GlyphSet::GlyphSet(const char* glyphFile, int glyphWidth, int glyphHeight);

private:
	int m_nWidth;
	int m_nGlyphWidth;
	int m_nHeight;
	int m_nGlyphHeight;
	int m_nNumGlyphsVertical;
	int m_nNumGlyphsHorizontal;
	int m_nNumGlyphs;
	Glyph **m_pGlyphs;

public:
	Glyph *getGlyph(int which) { if(which >= 0 && which < m_nNumGlyphs) return m_pGlyphs[which]; return 0; }
	int getWidth() { return m_nWidth; }
	int getGlyphWidth() { return m_nGlyphWidth; }
	int getHeight() { return m_nHeight; }
	int getGlyphHeight() { return m_nGlyphHeight; }
	int getNumGlyphsVertical() { return m_nNumGlyphsVertical; }
	int getNumGlyphsHorizontal() { return m_nNumGlyphsHorizontal; }
	int getNumGlyphs() { return m_nNumGlyphs; }

};


#endif
