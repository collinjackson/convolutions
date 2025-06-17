// Abstract Projectile class header
//

#ifndef __C_SLOWMOTIONBULLETPROJECTILE_H_INCLUDED__
#define __C_SLOWMOTIONBULLETPROJECTILE_H_INCLUDED__

#include "IProjectile.h"
#include <Irrlicht.h>

class CSlowMotionBulletProjectile : public IProjectile
{
public:

	CSlowMotionBulletProjectile(IScenario* s, core::line3d<f32> path, ITriangleSelector* targets)
		: IProjectile(s, path, targets)
	{
		currentForwardSpeed = 0.1f;	
		range = 1000.0f;
		damage = 0.1f;

		scene::ISceneManager* sm = scenario->getDevice()->getSceneManager();

/*
		scene::IParticleSystemSceneNode* pas = 0;
		pas = sm->addParticleSystemSceneNode(false, 0, -1, path.start);
		pas->setParticleSize(core::dimension2d<f32>(0.5f, 0.5f));

		scene::IParticleEmitter* em = pas->createBoxEmitter(
			core::aabbox3d<f32>(-1,-1,-1,1,1,1),
			direction * -1, 20, 40, video::SColor(0,255,255,255), video::SColor(0,255,255,255),
			1200, 2000, 50);

		pas->setEmitter(em);
		em->drop();

		scene::IParticleAffector* paf = scenario->getSmokeParticleSystem()->createFadeOutParticleAffector(video::SColor(0,0,0,0), 1000);
		pas->addAffector(paf);
		paf->drop();

		pas->setMaterialFlag(video::EMF_LIGHTING, false);
		pas->setMaterialTexture(0, scenario->getDevice()->getVideoDriver()->getTexture("media/smoke.bmp"));
		pas->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
	
		projectileNode = pas;
*/
		projectileNode = sm->addTestSceneNode(1, 0, -1, path.start, core::vector3df(0,0,0), core::vector3df(1,0.5f,0.5f));
		//video::SMaterial material = projectileNode->getMaterial(1);
		// projectileNode->setMaterialFlag(video::EMF_WIREFRAME, true);
		//material.SpecularColor = video::SColor(255,100,100,100);
		//material.DiffuseColor = video::SColor(255,0,0,0);
		//material.Shininess = 4;
		//material.Wireframe = true;
		projectileNode->setMaterialTexture(0, scenario->getDevice()->getVideoDriver()->getTexture("media\\bullet.bmp"));
		projectileNode->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	void CSlowMotionBulletProjectile::animate(u32 now)
	{
		scene::ISceneManager* sm = scenario->getDevice()->getSceneManager();

		ISceneNode* bulletTrail = 0;
		bulletTrail = sm->addBillboardSceneNode(0, core::dimension2d<f32>(1.5f,1.5f), interpolate(lastUpdateTime - 10));
		bulletTrail->setMaterialFlag(video::EMF_LIGHTING, false);
		bulletTrail->setMaterialTexture(0, scenario->getDevice()->getVideoDriver()->getTexture("media\\weirdring.bmp"));
		bulletTrail->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

		scene::ISceneNodeAnimator* anim = sm->createDeleteAnimator(300);
		bulletTrail->addAnimator(anim);
		anim->drop();

		IProjectile::animate(now);
	}

	~CSlowMotionBulletProjectile();


};

#endif