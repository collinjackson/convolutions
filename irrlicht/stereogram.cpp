// 
// stereogram.cpp
//
// Contains stereogram drawing code.
//


#include <algorithm>
#include <math.h>
#include <limits.h>
#include <assert.h>

#include "stereogram.h"
#include "glyph.h"


// Default values for the huge number of variables

#define CHANGE_ODDS (1.0/4.0f)
#define PROCODDS_MIN (0.003f)
#define PROCODDS_MAX (0.2f)
#define FLIP_DEPTH (false)
#define BRIGHTNESS_SMOOTHING (0.3f)
#define SHOW_GLYPHS (false)
#define STREAM_DOWN (true)
#define HANDICAP_MODE (false)
#define BRIGHTNESS_MAX (0.99f)
#define BRIGHTNESS_DECAY (0.95f)
#define BRIGHTNESS_MIN (0.4f)
#define FOCUS_DEPTH (0.5f)
#define AUTO_FOCUS (false);

#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#define max(a, b)	( ( (a)>(b) ) ? (a) : (b) )
#define DESTROY(p)	{  if ((p)!=NULL) {delete [] p; p=NULL; } }

namespace irr
{
namespace stereogram
{

Stereogram::Stereogram(video::IVideoDriver *driver)
	: m_pDriver(driver), m_pGlyphSet(0)
{
	m_nNumGlyphs = 0;
	m_nChangeOdds = CHANGE_ODDS;		// Odds of pixel changing randomly
	m_nProcOdds = PROCODDS_MIN;
	m_nBrightnessSmoothing = BRIGHTNESS_SMOOTHING;
	m_nFlipDepth = FLIP_DEPTH;
	m_nShowGlyphs = SHOW_GLYPHS;
	m_nStreamDown = STREAM_DOWN;
	m_nHandicapMode = HANDICAP_MODE;
	m_nBrightness = BRIGHTNESS_MIN;
	m_nBrightnessMax = BRIGHTNESS_MAX;
	m_nCodeDensity = PROCODDS_MAX;
	m_nBrightnessDecay = BRIGHTNESS_DECAY;
	m_nFocusDepth = FOCUS_DEPTH;
	m_nAutoFocus = AUTO_FOCUS;

	m_ucZBuffer = m_ucOutputBuffer = 0;
	m_fDepthBuffer = m_fBrightnessBuffer = m_fDisplayBrightnessBuffer = 0;
	m_usLinkBuffer = m_usGlyphBuffer = 0;
}

// Deallocate the data structures
Stereogram::~Stereogram()
{
	DESTROY(m_fDepthBuffer);
	DESTROY(m_ucZBuffer);
	DESTROY(m_usLinkBuffer);
	DESTROY(m_fBrightnessBuffer);
	DESTROY(m_fDisplayBrightnessBuffer);
	DESTROY(m_usGlyphBuffer);
	DESTROY(m_ucOutputBuffer);
}

GlyphSet* Stereogram::createGlyphSet(const char* filename, core::dimension2d<s32>glyphSize)
{
	return new GlyphSet(filename, glyphSize.Width, glyphSize.Height);
}

// Draw stereogram on the screen using the provided driver
// Checks for screen resize and calls resize() as necessary
void Stereogram::draw()
{
	if(!m_nShowGlyphs)	// stereogram is disabled
	{
		// Optional testing code here...
		return;
	}

	if(m_pDriver->getScreenSize() != m_dScreenSize) resize();

	fillDepthBuffer( BUFFER_HARDWARE );
	fillZBuffer( BUFFER_DEPTH );

	if(m_nStreamDown) fillBrightnessBuffer( BUFFER_BRIGHTNESS );
	else fillBrightnessBuffer( BUFFER_MAX );
		
	if(m_nHandicapMode) 
	{
		fillGlyphBuffer( BUFFER_GLYPH | BUFFER_NONE );
		fillDisplayBrightnessBuffer( BUFFER_BRIGHTNESS | BUFFER_Z );
	}
	else 
	{
		fillLinkBuffer( BUFFER_Z );
		fillGlyphBuffer( BUFFER_GLYPH | BUFFER_LINK );
		fillDisplayBrightnessBuffer( BUFFER_BRIGHTNESS | BUFFER_LINK );
	}

	fillOutputBuffer( BUFFER_GLYPH | BUFFER_DISPLAYBRIGHTNESS );
	fillHardwareBuffer( BUFFER_OUTPUT );
	
};

// Recalculate stereogram dimensions
void Stereogram::resize()
{
	m_dScreenSize = m_pDriver->getScreenSize();
	if(m_pGlyphSet)
	{
		m_dGlyphSize = core::dimension2d<s32>(m_pGlyphSet->getGlyphWidth(), m_pGlyphSet->getGlyphHeight());
		m_nNumGlyphs = m_pGlyphSet->getNumGlyphs();

	}
	else 
	{
		m_dGlyphSize = core::dimension2d<s32>(1, 1);
		m_nNumGlyphs = 0;
	}

	m_dBufferSize.Width = m_dScreenSize.Width / m_dGlyphSize.Width;
	m_dBufferSize.Height = m_dScreenSize.Height / m_dGlyphSize.Height;

	m_nDPI = 72.0 / (float)m_dGlyphSize.Width;
	m_nE = round(2.5f*m_nDPI); 
	m_nMu = (1/4.0f); 

	// Data structures are probably the wrong size, so start over
	reset();

}

// Initialize/clear the data structures
void Stereogram::reset()
{
	// Destroy storage
	DESTROY(m_fDepthBuffer);
	DESTROY(m_ucZBuffer);
	DESTROY(m_usLinkBuffer);
	DESTROY(m_fBrightnessBuffer);
	DESTROY(m_fDisplayBrightnessBuffer);
	DESTROY(m_usGlyphBuffer);
	DESTROY(m_ucOutputBuffer);

	// Allocate storage
	m_nScreenPixels = m_dScreenSize.Width * m_dScreenSize.Height;
	m_nBufferPixels = m_dBufferSize.Width * m_dBufferSize.Height;
	int i;

	if(m_nScreenPixels == 0 || m_nBufferPixels == 0)
	{
		m_ucZBuffer = m_ucOutputBuffer = 0;
		m_fDepthBuffer = m_fBrightnessBuffer = m_fDisplayBrightnessBuffer = 0;
		m_usLinkBuffer = m_usGlyphBuffer = 0;
		return;
	}

	m_fDepthBuffer = new f32 [m_nScreenPixels];		// Not initialized
	m_ucZBuffer = (u8*)memset(new u8 [m_nBufferPixels], 255, m_nBufferPixels);
	m_usLinkBuffer = (u16*)memset(new u16 [m_nBufferPixels], 0, m_nBufferPixels);
	m_fBrightnessBuffer = new f32 [m_nBufferPixels];
	m_fDisplayBrightnessBuffer = new f32 [m_nBufferPixels];

	m_usGlyphBuffer = new u16 [m_nBufferPixels];
	for(i = 0; i < m_nBufferPixels; i++) 
	{
		m_usGlyphBuffer[i] = rand();	
		m_fBrightnessBuffer[i] = m_fDisplayBrightnessBuffer[i] = 0;
	}

	m_ucOutputBuffer = (u8*)memset(new u8 [m_nScreenPixels * 3], 0, m_nScreenPixels * 3);

	assert(m_fDepthBuffer);
	assert(m_ucZBuffer);
	assert(m_usLinkBuffer);
	assert(m_fBrightnessBuffer);
	assert(m_fDisplayBrightnessBuffer);
	assert(m_usGlyphBuffer);
	assert(m_ucOutputBuffer);

}

// Read input depth data from hardware depth buffer onto m_pDepth
void Stereogram::fillDepthBuffer( buffer_t source )
{
	assert(source == BUFFER_HARDWARE);   // Only know how to read the hardware buffer
	m_pDriver->readDepth(m_fDepthBuffer);	
}

// Lower resolution; perform depth adjustment to maximize depth contrast
void Stereogram::fillZBuffer( int source )
{
	assert(source == BUFFER_DEPTH); 	// Only know how to read the depth buffer

	// This was the old way of doing depth focusing
	float min = 255, max = 0;
	int i;
	for(i = 0; i < m_nScreenPixels; i++)
	{
		if(m_fDepthBuffer[i] < 1 && m_fDepthBuffer[i] < min) min = m_fDepthBuffer[i];
		if(m_fDepthBuffer[i] < 1 && m_fDepthBuffer[i] > max) max = m_fDepthBuffer[i];
	}
	if(max <= min) return;

	// Here is the new way
	float focusDepth = m_nFocusDepth;
	if(m_nAutoFocus)
	{
		float sum = 0;
		for(int i = 0; i < m_nScreenPixels; i++) sum += m_fDepthBuffer[i];
		min = (sum / (float)m_nScreenPixels);
	}
	
	for(int x = 0; x < m_dBufferSize.Width; x++)
		for(int y = 0; y < m_dBufferSize.Height; y++)
		{
			f32 pixel = m_fDepthBuffer[y * m_dGlyphSize.Height * m_dScreenSize.Width + x * m_dGlyphSize.Width];
			
			// Perform adjustment (optional)
			pixel = (float)(pixel-min) / (float)(max-min);

			// Pixels centered on the focus depth should be exactly average
			if(pixel > focusDepth) pixel = 0.5f + (pixel - focusDepth) / (1.0f - focusDepth) * 0.5f;
			if(pixel <= focusDepth) pixel = 0.5f + (pixel - focusDepth) / (focusDepth) * 0.5f;

			// I also tried a cube root function; the transition is too sharp
			// pixel = pow(pixel - focusDepth,1.0/3.0) * 0.5f + 1.0f;
			
			// Perform clamping to avoid roundoff errors
			pixel = max(0.0,min(1.0,pixel));
			
			if(!m_nFlipDepth) pixel = 1.0 - pixel;
			m_ucZBuffer[y * m_dBufferSize.Width + x] = (int)(pixel * 255.0f);
		}
}

// Using m_pDepth, perform hidden surface removal and put links onto the m_pLink
void Stereogram::fillLinkBuffer( buffer_t source )
{
/*
	int x, y;               
	for( y = 0; y < m_dBufferSize.Height; y++ ) {
		int s;             
		int left, right;   

	    // initialize the links
		u16 *same = &m_usLinkBuffer[y * m_dBufferSize.Width];
		u8 *Z = &m_ucZBuffer[y * m_dBufferSize.Width];
		for( x = 0; x < m_dBufferSize.Width; x++ )
			same[x] = x;

	    // calculate the links for the Z[][] object 
		for( x = 0; x < m_dBufferSize.Width; x++ ) {

			s = separation( Z[x] / 255.0f );
			left = x - (s/2);
			right = left + s;        
			if( 0 <= left && right < m_dBufferSize.Width ) 
			{       
				int k;                
				for(k=same[left]; k!=left && k!=right; k=same[left])
				{
					if( k < right ) left = k;
					else 
					{
						left = right;
						right = k;
				    }
				}
				same[left] = right; 
			}
		}

	}
*/
	// Only know how to construct a link buffer from a z buffer
	if(source == BUFFER_NONE) return;
	assert(source == BUFFER_Z);
	
	int x, y;               
	for( y = 0; y < m_dBufferSize.Height; y++ ) 
	{

		u16 *same = &m_usLinkBuffer[y * m_dBufferSize.Width];
		u8 *Z = &m_ucZBuffer[y * m_dBufferSize.Width];

		for( x = 0; x < m_dBufferSize.Width; x++ )
		{
			assert(Z[x] >= 0 && Z[x] <= 255);
			same[x] = x;
		}

	    // calculate the links for the Z[][] object 
		for( x = 0; x < m_dBufferSize.Width; x++ ) 
		{
			int s = separation(Z[x] / 255.0f);
			int left = x - (s/2);
			int right = left + s;
			if( 0 <= left && right < m_dBufferSize.Width ) 
			{       
				bool visible;
				
//				int t=1;
//				float zt;
//				do {
//					zt = Z[x] + 2*(2 - m_nMu*Z[x])*t/(m_nMu*m_nE);
//					visible = Z[x-t]<zt && Z[x+t]<zt;
//					t++;
//				} while(visible && zt < 1);
				
				visible = true;

				if(visible) {
					int l = same[left];
					while(l != left && l != right)
						if (l < right) {
						left = l;
						l = same[left];
						}
						else {
							same[left] = right;
							left = right;
							l = same[left];
							right = l;
						}
					same[left] = right;
				}
			}
		}
	}
}

// Slide m_pBrightness buffer downwards (gives falling code effect)
void Stereogram::fillBrightnessBuffer( buffer_t source )
{
	if(source & BUFFER_MIN)
		for(int i = 0; i < m_nBufferPixels; i++) m_fBrightnessBuffer[i] = 0.0f;
	if(source & BUFFER_MAX)
		for(int i = 0; i < m_nBufferPixels; i++) m_fBrightnessBuffer[i] = 1.0f;
	if(source & BUFFER_BRIGHTNESS)
	{
		for( int y = 0; y < m_dBufferSize.Height; y++ )
			for( int x = m_dBufferSize.Width-1; x >= 0; x-- ) 
			{
				f32 *brightness = &m_fBrightnessBuffer[y * m_dBufferSize.Width + x];

				// Proc new glyphs at the top
				if(	y == m_dBufferSize.Height-1 && 
					*brightness < m_nBrightness && 
					rand() % 1000 < m_nProcOdds * 1000)
				{
					*brightness = m_nBrightnessMax;
					if(m_nProcOdds < m_nCodeDensity) m_nProcOdds += 0.001f;
				}

				// Get brightness from above, decay if you're at the top
				else if(y == m_dBufferSize.Height-1) *brightness *= m_nBrightnessDecay;
				else *brightness = *(brightness + m_dBufferSize.Width);
				assert(*brightness >= 0);
			}
	}

}

// Using m_pGlyph and m_pLink, find a suitable glyph assignment		
void Stereogram::fillGlyphBuffer( buffer_t source )
{
	for(int y = m_dBufferSize.Height - 1; y >= 0; y--)
	{
		u16 *same = &m_usLinkBuffer[y * m_dBufferSize.Width];
		u16 *glyph = &m_usGlyphBuffer[y * m_dBufferSize.Width];
		for(int x = m_dBufferSize.Width - 1; x >= 0; x--)
		{
			// Change glyphs randomly
			if( same[x] == x || !(source & BUFFER_LINK) || x < m_dBufferSize.Width - 1 && glyph[x+1] == glyph[x]) {
				if(!(source & BUFFER_GLYPH)
				   || rand() % 1000 < 1000 * m_nChangeOdds 
				   ) 
					glyph[x] = rand();
			}
			// Propagate stereogram match if necessary
			else glyph[x] = glyph[same[x]];
		}
	}
}

// Using m_pBrightness and m_pLink, perform brightness smoothing (handicap view)
void Stereogram::fillDisplayBrightnessBuffer( buffer_t source )
{
	if(source & BUFFER_MIN)
		for(int i = 0; i < m_nBufferPixels; i++) m_fDisplayBrightnessBuffer[i] = 0.0f;
	if(source & BUFFER_MAX)
		for(int i = 0; i < m_nBufferPixels; i++) m_fDisplayBrightnessBuffer[i] = 1.0f;
	if(source & BUFFER_BRIGHTNESS)
		for(int i = 0; i < m_nBufferPixels; i++) m_fDisplayBrightnessBuffer[i] = min(1.0f,m_fBrightnessBuffer[i]);
	if(source & BUFFER_Z)
		for(int i = 0; i < m_nBufferPixels; i++) m_fDisplayBrightnessBuffer[i] = max(0.0,m_fDisplayBrightnessBuffer[i] + m_ucZBuffer[i] / 255.0f - 1.0f);
	if(source & BUFFER_LINK)
		for(int y = 0; y < m_dBufferSize.Height; y++)
		{
			u8 *scratch = (u8*)memset(new u8 [m_dBufferSize.Width], 0, m_dBufferSize.Width);
			u16 *same = &m_usLinkBuffer[y * m_dBufferSize.Width];
			f32 *intensity = &m_fDisplayBrightnessBuffer[y * m_dBufferSize.Width];
			f32 *remember = &m_fBrightnessBuffer[y * m_dBufferSize.Width];


			u8 *Z = &m_ucZBuffer[y * m_dBufferSize.Width];
			for(int x = m_dBufferSize.Width-1; x >= 0; x--) 
			{
				int s = separation(Z[x] / 255.0f);
				int left = x - (s/2);
				int right = left + s;
				if( 0 <= left && right < m_dBufferSize.Width ) 
				{
					intensity[left] += remember[right] * m_nBrightnessSmoothing;
					intensity[right] += remember[left] * m_nBrightnessSmoothing;
					scratch[left]++;
					scratch[right]++;
				}
			}
			for(x = m_dBufferSize.Width-1; x >= 0; x--) intensity[x] /= (1 + scratch[x] * m_nBrightnessSmoothing);
			/*
			// Perform brightness smoothing
			
			for(int x = m_dBufferSize.Width-1; x >= 0; x--) 
				if(same[x] != x)
				{
					int other = intensity[same[x]];
					intensity[same[x]] += intensity[x] * m_nBrightnessSmoothing;
					intensity[x] += other * m_nBrightnessSmoothing;
					scratch[x]++;
					scratch[same[x]]++;
				}
			*/

//			intensity[x] = (intensity[x] + intensity[same[x]] * ) / (1.0f + m_nBrightnessSmoothing);		
//				Optional time-lapse brightness smoothing
//				remember[x] = (remember[x] + intensity[x]) / 2.0f;
		}
}

// Rasterize the glyphs into a large color output buffer
void Stereogram::fillOutputBuffer( buffer_t source )
{
	assert(source == (BUFFER_GLYPH | BUFFER_DISPLAYBRIGHTNESS));

	for(int x = 0; x < m_dBufferSize.Width; x++)
		for(int y = 0; y < m_dBufferSize.Height; y++)
		{
			f32 intensity = m_fDisplayBrightnessBuffer[y * m_dBufferSize.Width + x];
			assert(intensity >= 0);
			u16 glyph = m_usGlyphBuffer[y * m_dBufferSize.Width + x];
			if(m_pGlyphSet)								// Glyphs available, so render them normally
			{
				Glyph *g = m_pGlyphSet->getGlyph(glyph % m_pGlyphSet->getNumGlyphs());
				g->scale(intensity);
				g->draw(x * m_dGlyphSize.Width, y * m_dGlyphSize.Height, m_ucOutputBuffer, m_dScreenSize.Width);
			}
			else										// No glyphs available; use rainbow color instead
			{
				for(int subx = 0; subx < m_dGlyphSize.Width; subx++)
					for(int suby = 0; suby < m_dGlyphSize.Height; suby++)
					{
						u8 *pixel = &m_ucOutputBuffer[((y * m_dGlyphSize.Width + suby) * m_dScreenSize.Width + (x * m_dGlyphSize.Width + subx))*3];
						pixel[0] = (glyph % 256) * intensity;
						pixel[1] = (glyph * 17 % 256) * intensity;
						pixel[2] = (glyph * 29 % 256) * intensity;
					}
			}
		}
}

// Write output data to hardware color buffer
void Stereogram::fillHardwareBuffer( buffer_t source )
{
	// Only know how to fill from output buffer
	assert(source == BUFFER_OUTPUT);
	m_pDriver->writeColor(m_ucOutputBuffer);	
}


} // namespace stereogram
} // namespace irr