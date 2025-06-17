// 
// stereogram.h
//
// header file for stereogram.cpp
//

#ifndef Stereogram_h
#define Stereogram_h

#include "dimension2d.h"
#include "IVideoDriver.h"

class GlyphSet;

namespace irr
{
namespace stereogram
{
typedef int stereogram_t;

class Stereogram 
{
public:

	// Most general constructor
	Stereogram(video::IVideoDriver *driver);

	// Deallocate the data structures
	virtual ~Stereogram();

	// Draw stereogram on the screen using the provided driver
	// Checks for screen resize and calls resize() as necessary
	virtual void draw();

	// Produce a glyph set from file
	virtual GlyphSet* createGlyphSet(const char* filename, core::dimension2d<s32>glyphSize);
	// {
	//	GlyphSet* r = new GlyphSet(filename, glyphSize.Width, glyphSize.Height);
	//	return r;
	// }

protected:

	// METHODS

	// Set the dimensions of the stereogram and reset it
	virtual void resize();

	// Initialize/clear the data structures
	virtual void reset();

	// Various buffer types
	typedef int buffer_t;
	enum
	{
		BUFFER_NONE					= 0x0,
		BUFFER_DEPTH				= 0x1,
		BUFFER_Z					= 0x2,
		BUFFER_LINK					= 0x4,
		BUFFER_BRIGHTNESS			= 0x8,
		BUFFER_DISPLAYBRIGHTNESS	= 0x10,
		BUFFER_GLYPH				= 0x12,
		BUFFER_OUTPUT				= 0x14,
		BUFFER_HARDWARE				= 0x18,
		BUFFER_MIN					= 0x20,	// All zeroes
		BUFFER_MAX					= 0x22		// All 255 or 1.0f, etc.
	};

	// Read input depth data from hardware depth buffer onto m_pDepth
	virtual void fillDepthBuffer( buffer_t source );

	// Perform depth adjustment to maximize depth contrast
	virtual void fillZBuffer( buffer_t source );

	// Using m_pDepth, perform hidden surface removal and put links onto the m_pLink
	virtual void fillLinkBuffer( buffer_t source );

	// Slide m_pBrightness buffer downwards (gives falling code effect)
	virtual void fillBrightnessBuffer( buffer_t source );

	// Using m_pGlyph and m_pLink, find a suitable glyph assignment		
	virtual void fillGlyphBuffer( buffer_t source );

	// Using m_pBrightness and m_pLink, perform brightness smoothing (handicap view)
	virtual void fillDisplayBrightnessBuffer( buffer_t source );

	// Rasterize the glyphs into a large color output buffer
	virtual void fillOutputBuffer( buffer_t source );

	// Write output data to hardware color buffer
	virtual void fillHardwareBuffer( buffer_t source );
	
	// ATTRIBUTES
	
	// Actual source/target image dimensions
	core::dimension2d<s32>	m_dScreenSize;
	// Stereogram glyph size (can be set to 1 for pixel stereogram)
	core::dimension2d<s32>	m_dGlyphSize;
	// Lower resolution glyph buffer size
	core::dimension2d<s32>	m_dBufferSize;
	// Number of screen pixels
	int						m_nScreenPixels;
	// Number of buffer pixels (one per screen glyph)
	int						m_nBufferPixels;
	// Number of glyphs types
	int						m_nNumGlyphs;
	// Screen resolution approximation
	float					m_nDPI;	
	// Eye separation approximation
	float					m_nE;
	// Field of view
	float					m_nMu;
	// Odds of changing a pixel
	float					m_nChangeOdds;
	// Odds of producing a new string at the top
	float					m_nProcOdds;
	// Amount that code streaming is compromised in favor of stereogram
	float					m_nBrightnessSmoothing;
	// Option to switch black/white in depth buffer
	bool					m_nFlipDepth;
	// Whether the stereogram is drawn with blocks or glyphs
	bool					m_nShowGlyphs;
	// Whether the code is fixed or streaming (streaming looks better)
	bool					m_nStreamDown;
	// Whether to show a real stereogram or just brightness-modified glyphs
	bool					m_nHandicapMode;
	// Whether to use the focus slider or attempt to focus automatically
	bool					m_nAutoFocus;
	// Control on the minimum brightness of a glyph before a new one can be procced
	float					m_nBrightness;
	// Control on the maximum brightness of a newly-procced glyph
	float					m_nBrightnessMax;
	// Odds of proccing a new glyph, resulting in dense code look
	float					m_nCodeDensity;
	// Brightness dropoff factor (affects code string length)
	float					m_nBrightnessDecay;
	// Focus depth for nonlinear brightness mapping
	float					m_nFocusDepth;



	// Display driver to use as hardware buffer
	video::IVideoDriver*	m_pDriver;

	// Library of glyphs to draw from
	GlyphSet*				m_pGlyphSet;
	
	// Various buffers in the stereogram rendering pipeline
	f32*					m_fDepthBuffer;				// overwritten each cycle. BIG
	u8*						m_ucZBuffer;					// overwritten each cycle.
	u16*					m_usLinkBuffer;					// overwritten each cycle.
	f32*					m_fBrightnessBuffer;			// contains persistent elements.
	f32*					m_fDisplayBrightnessBuffer;		// overwritten each cycle.
	u16*					m_usGlyphBuffer;				// contains persistent elements.
	u8*						m_ucOutputBuffer;				// overwritten each cycle. VERY BIG

	// Nifty utilities
	inline int round(float X) { return (int)((X)+0.5); }
	inline int separation(float Z) { return round((1-m_nMu*Z)*m_nE/(2-m_nMu*Z)); }

public:
	// Accessors
	GlyphSet *getGlyphSet() { return m_pGlyphSet; }
	void setGlyphSet(GlyphSet *glyphSet) { m_pGlyphSet = glyphSet; resize(); } 

	float getDPI() { return m_nDPI;	} 
	void setDPI(float value) { m_nDPI = value; }
	
	float getEyeSeparation() { return m_nE;	} 
	void setEyeSeparation(float value) { m_nE = value; }
	
	float getMu() { return m_nMu;	} 
	void setMu(float value) { m_nMu = value; }
	
	float getChangeOdds() { return m_nChangeOdds; } 
	void setChangeOdds(float value) { m_nChangeOdds = value; }
	
	float getProcOdds() { return m_nProcOdds;	} 
	void setProcOdds(float value) { m_nProcOdds = value; }
	
	float getBrightnessSmoothing() { return m_nBrightnessSmoothing;	} 
	void setBrightnessSmoothing(float value) { m_nBrightnessSmoothing = value; }
	
	bool getFlipDepth() { return m_nFlipDepth;	} 
	void setFlipDepth(bool value) { m_nFlipDepth = value; }

	bool getShowGlyphs() { return m_nShowGlyphs; }
	void setShowGlyphs(bool value) { m_nShowGlyphs = value; }

	bool getStreamDown() { return m_nStreamDown; }
	void setStreamDown(bool value) { m_nStreamDown = value; }

	bool getAutoFocus() { return m_nAutoFocus; }
	void setAutoFocus(bool value) { m_nAutoFocus = value; }

	bool getHandicapMode() { return m_nHandicapMode; }
	void setHandicapMode(bool value) { m_nHandicapMode = value; }

	float getBrightness() { return m_nBrightness; }
	void setBrightness(float value) { m_nBrightness = value; }

	float getBrightnessMax() { return m_nBrightnessMax; }
	void setBrightnessMax(float value) { m_nBrightnessMax = value; }

	float getCodeDensity() { return m_nCodeDensity; }
	void setCodeDensity(float value) { m_nCodeDensity = value; }

	float getBrightnessDecay() { return m_nBrightnessDecay; }
	void setBrightnessDecay(float value) { m_nBrightnessDecay = value; }

	float getFocusDepth() { return m_nFocusDepth; }
	void setFocusDepth(float value) { m_nFocusDepth = value; }

};

} // namespace stereogram
} // namespace irr

#endif
