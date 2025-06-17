// Abstract Projectile class header
//

#ifndef __C_BULLETPROJECTILE_H_INCLUDED__
#define __C_BULLETPROJECTILE_H_INCLUDED__

#include "IProjectile.h"
#include <Irrlicht.h>

class CBulletProjectile : public IProjectile
{
public:

	CBulletProjectile(IScenario* s, core::line3d<f32> path, ITriangleSelector* targets)
		: IProjectile(s, path, targets)
	{
		currentForwardSpeed = 5.0f;	
		range = 10000.0f;
		damage = 0.1f;

		scene::ISceneManager* sm = scenario->getDevice()->getSceneManager();

		// create fire ball
		// projectileNode = sm->addBillboardSceneNode(0, core::dimension2d<f32>(5,5), path.start);
		// projectileNode->setMaterialFlag(video::EMF_LIGHTING, false);
		// projectileNode->setMaterialTexture(0, scenario->getDevice()->getVideoDriver()->getTexture("media\\particle.bmp"));
		// projectileNode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

	}

	~CBulletProjectile();


};

#endif