// Class for things that can be animated
//

#ifndef __I_ANIMATABLE_H_INCLUDED__
#define __I_ANIMATABLE_H_INCLUDED__

class IAnimatable
{
public:

	virtual void animate(unsigned int now) = 0;

};

#endif