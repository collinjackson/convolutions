// Concrete operative class
//

#ifndef __C_OPERATIVE_PERSON_H_INCLUDED__
#define __C_OPERATIVE_PERSON_H_INCLUDED__

#include "IPerson.h"

class IScenario;

class COperativePerson : public IPerson
{
public:

	// Instantiate the operative in a scenario and controlled by a keymap
	COperativePerson(IScenario* scenario, SKeyMap* km);

	~COperativePerson() {}

	virtual bool OnEvent(SEvent event);

	SKeyMap* KeyMap;

	bool CursorKeys[EKA_COUNT];

	void allKeysUp();

	virtual void faceAwayFromCamera();

	// Overrides IPerson version
	virtual void animate(u32 now);
	virtual void die();

	virtual core::vector3df bulletTimeScale() 
	{ 
		f32 jumpBonus = currentForwardSpeed ? 2.5f : 0.1f;
		return core::vector3df(1.0f,jumpBonus,1.0f);
	}

	virtual void damage(core::triangle3df where, f32 howMuch);

};

#endif