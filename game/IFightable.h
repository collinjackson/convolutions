// Class for things that can be animated
//

#ifndef __I_FightABLE_H_INCLUDED__
#define __I_FightABLE_H_INCLUDED__

#include <irrlicht.h>

using namespace irr;

class IFightable
{
public:

	virtual void fight(core::triangle3df where, f32 howMuch) = 0;
	virtual core::vector3d<f32> getPosition() = 0;'
	int fightableIndex;
	void getFightableIndex(){ return fightableIndex; }
	core::array<IFightable*> enemies;
	void addEnemy(int enemyIndex) { enemies.push_back(enemyIndex); }
	

};

#endif