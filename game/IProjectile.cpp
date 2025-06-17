// Abstract person class code
// 

#include <assert.h>
#include "IProjectile.h"
#include "IScenario.h"

IProjectile::IProjectile(IScenario* s, core::line3d<f32> path, ITriangleSelector* movable)
:	scenario(s), 
	maxPath(path),
	projectileNode(0),
	currentForwardSpeed(0.4f),
	movableObjects(movable),
	range(0), damage(0)
{
	movableObjects->grab();
}

void IProjectile::fire(u32 when)
{
	firstUpdateTime = lastUpdateTime = when;

	direction = maxPath.end - maxPath.start;
	direction.normalize();

	maxPath.end = maxPath.start + direction * range;

	scene::ISceneManager* sm = scenario->getDevice()->getSceneManager();

	core::triangle3df triangle;
	imp.exists = false;

	if (sm->getSceneCollisionManager()->getCollisionPoint(maxPath, scenario->getStationaryTangibleSelector(), maxPath.end, triangle))
	{
		// collides with wall
		core::vector3df out = triangle.getNormal();
		out.setLength(0.03f);

		imp.exists = true;
		imp.outVector = out;
		imp.pos = maxPath.end;
	}

	finalUpdateTime = firstUpdateTime + (f32)(maxPath.start - maxPath.end).getLength() / currentForwardSpeed;

	scenario->getAnimatableRegistry()->add(this);
}

IProjectile::~IProjectile()
{
	scenario->getAnimatableRegistry()->remove(this);
	movableObjects->drop();
	scenario->getDevice()->getSceneManager()->addToDeletionQueue(projectileNode);
}

core::vector3df IProjectile::interpolate(u32 when)
{
	s32 progress = (s32)when - (s32)firstUpdateTime;
	assert(currentForwardSpeed > 0);
	return maxPath.start + direction * progress * currentForwardSpeed;
}

void IProjectile::animate(u32 now)
{
	scene::ISceneManager* sm = scenario->getDevice()->getSceneManager();
	core::line3d<f32> delta(interpolate(lastUpdateTime), interpolate(now));
	core::triangle3df triangle;

	if(projectileNode) projectileNode->setPosition(interpolate(now));
	lastUpdateTime = now;

	if(now >= finalUpdateTime)
	{
		wallhit();
	}
	else if (sm->getSceneCollisionManager()->getCollisionPoint(delta, movableObjects, delta.end, triangle))
	{
		// collides with movable object! ouch!
		core::vector3df out = triangle.getNormal();
		out.setLength(0.03f);

		imp.exists = true;
		imp.outVector = out;
		imp.pos = delta.end;
		hit(triangle);
	}
	
}

void IProjectile::hit(core::triangle3df where) 
{ 
	createParticleImpact(imp); 
	scenario->getDamagableRegistry()->damage(where, damage); 
	delete this; 
}

void IProjectile::wallhit() { 
	
	if(imp.exists)
	{
//		OutputStreamPtr impactSound = OpenSound(scenario->audiereDevice.get(), "media\\sound\\smash.wav", false);
		scenario->impactSound->play();
		createParticleImpact(imp); 
	}
	delete this; 
}

void IProjectile::createParticleImpact(SParticleImpact imp)
{
	if(!imp.exists) return;

	scene::ISceneManager* sm = scenario->getDevice()->getSceneManager();

	// create smoke particle system
	scene::IParticleSystemSceneNode* pas = 0;

	pas = sm->addParticleSystemSceneNode(false, 0, -1, imp.pos);

	pas->setParticleSize(core::dimension2d<f32>(5.0f, 5.0f));

	scene::IParticleEmitter* em = pas->createBoxEmitter(
		core::aabbox3d<f32>(-2,-2,-2,2,2,2),
		imp.outVector, 20, 40, video::SColor(0,255,255,255), video::SColor(0,255,255,255),
		1200, 1600, 20);

	pas->setEmitter(em);
	em->drop();

	scene::IParticleAffector* paf = scenario->getSmokeParticleSystem()->createFadeOutParticleAffector(video::SColor(0,0,0,0), 500);
	pas->addAffector(paf);
	paf->drop();

	pas->setMaterialFlag(video::EMF_LIGHTING, false);
	pas->setMaterialTexture(0, scenario->getDevice()->getVideoDriver()->getTexture("media/smoke.bmp"));
	pas->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);

	scene::ISceneNodeAnimator* anim = sm->createDeleteAnimator(500);
	pas->addAnimator(anim);
	anim->drop();

}
