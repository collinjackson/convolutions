// Abstract person class code
// 

#include <audiere.h>
#include "CMainMenu.h"
#include "IPerson.h"
#include "IScenario.h"
#include "IProjectile.h"
#include "CTangibleRegistry.h"
#include "CBulletProjectile.h"
#include "CFireballProjectile.h"
#include "CSlowMotionBulletProjectile.h"
#include <assert.h>

IPerson::IPerson(IScenario* s)
:	scenario(s),
	model(0), collider(0),
	defaultUpSpeed(2.0f), defaultDownSpeed(0.1f),
	defaultForwardSpeed(0.1f), defaultBackwardSpeed(0.1f),
	defaultRightSpeed(0.1f), defaultLeftSpeed(0.1f),
	currentUpSpeed(0.0f),
	currentRightSpeed(0.0f),
	currentForwardSpeed(0.0f),
	health(1.0f),
	permanentAnimation(scene::EMAT_STAND),
	temporaryAnimation(scene::EMAT_STAND),
	temporaryAnimationTimeLeft(-1),
	shooting(false), shootTimeRemaining(-1), refireTime(1000), shootingRange(500),
	lastUpdateTime(scenario->getDevice()->getTimer()->getTime())
{}

void IPerson::startPhysics()
{
	if(model)
	{
		model->setMD2Animation(permanentAnimation);
		model->setAnimationSpeed(20);
		model->addShadowVolumeSceneNode();
		
		personSelector = scenario->getDevice()->getSceneManager()->createTriangleSelectorFromBoundingBox(model);
		movableSelector = scenario->getDevice()->getSceneManager()->createMetaTriangleSelector();
		metaSelector = scenario->getDevice()->getSceneManager()->createMetaTriangleSelector();
		scenario->getMovableTangibleRegistry()->add(personSelector, movableSelector);
		scenario->getStationaryTangibleRegistry()->add(0, metaSelector);
		metaSelector->addTriangleSelector(movableSelector);
		scenario->getDamagableRegistry()->add(this);
		scenario->getAnimatableRegistry()->add(this);

		core::vector3df translation = core::vector3df(0,0,0);
		core::vector3df  gravity = core::vector3df(0,0,0);  // Placeholder; gravity not enabled yet
		core::aabbox3d<f32> box = model->getBoundingBox();
		core::vector3df radius = box.MaxEdge - box.getCenter();
		radius.X += 5;
		radius.Y += -10;
		radius.Z += 5;  //tweak for animating
		collider = scenario->getDevice()->getSceneManager()->
			createCollisionResponseAnimator(metaSelector, model, radius, gravity, 100.0f, translation, 0.0005f);
		model->addAnimator(collider);
	}
	gravitate(1);  // This sets the real gravity
}

IPerson::~IPerson() 
{
	scenario->getAnimatableRegistry()->remove(this);
	scenario->getDamagableRegistry()->remove(this);
	scenario->getMovableTangibleRegistry()->removeThing(personSelector);
	scenario->getMovableTangibleRegistry()->removeWorld(movableSelector);
	scenario->getStationaryTangibleRegistry()->removeWorld(metaSelector);
	metaSelector->drop();
	movableSelector->drop();
	personSelector->drop();

	// Don't delete the model; the delete animator will do it
	// scenario->getDevice()->getSceneManager()->addToDeletionQueue(model);

	// Prevent the model from disappearing forever to avoid garbage collection
	// TODO: Avoid this hack
	model->grab();

}

void IPerson::face(ISceneNode* what) {}

void IPerson::gravitate(f32 scale) 
{ 
	gravitateAbsolute(scale * scenario->getMainMenu()->getGravity()); 
}
void IPerson::gravitateAbsolute(f32 value) 
{
	collider->setGravity(core::vector3df(0,(-1 * (value) ) / 15.00f, 0));
}

bool IPerson::jump(f32 upSpeed) 
{ 
//	if(scenario->getBulletTime()) upSpeed /= 2;

	// Only allow the person to jump if they aren't falling
	if(upSpeed > 0 && collider && !collider->isFalling())
	{
		setAnimation(scene::EMAT_JUMP, 700);
		currentUpSpeed = upSpeed;
		return true;
	}
	else return false;
}

bool IPerson::sprint(f32 forwardSpeed) 
{
	if(forwardSpeed == 0) printf("STOPPING!!!!!!!!!!!!!\n");
//	if(scenario->getBulletTime()) forwardSpeed /= 2;

	// Only allow the person to do this if they aren't falling
	if(collider && !collider->isFalling() || 1)  // I changed my mind (cej9)
	{
		if(forwardSpeed > 0) setAnimation(scene::EMAT_RUN, -1);
		if(forwardSpeed < 0) setAnimation(scene::EMAT_RUN, -1);
		if(!forwardSpeed && !currentRightSpeed) setAnimation(scene::EMAT_STAND, -1);
		currentForwardSpeed = forwardSpeed;
		return true;
	}
	else return false;
}

bool IPerson::strafe(f32 rightSpeed) 
{
//	if(scenario->getBulletTime()) rightSpeed /= 2;

	// Only allow the person to do this if they aren't falling
	if(collider && !collider->isFalling() || 1) // I changed my mind (cej9)
	{
		if(rightSpeed && !currentForwardSpeed) setAnimation(scene::EMAT_RUN, -1);
		if(!rightSpeed && !currentForwardSpeed) setAnimation(scene::EMAT_STAND, -1);
		currentRightSpeed = rightSpeed;
		return true;
	}
	else return false;
}

void IPerson::animate(u32 now)
{
	u32 timeElapsed = now - lastUpdateTime;
	lastUpdateTime = now;

	if(model)
	{
		if(temporaryAnimationTimeLeft >= 0)
		{
			if(temporaryAnimationTimeLeft - (s32)timeElapsed <= 0)
			{
				temporaryAnimationTimeLeft = -1;
				model->setMD2Animation(permanentAnimation);
			}
			else temporaryAnimationTimeLeft -= timeElapsed;
		}

		core::vector3df pos = model->getPosition();
		
		printf("Forwardspeed %f, position %f %f %f, health %f\n", currentForwardSpeed, model->getPosition().X, model->getPosition().Y, model->getPosition().Z, health);
		
		// Start with basic movement vector
		core::vector3df movement(currentRightSpeed,currentUpSpeed,currentForwardSpeed);
		
		// Scale by bullet time (if any)
		if(scenario->getBulletTime()) movement *= bulletTimeScale();

		// Rotate vector to point in the same direction as the operative
		movement.rotateXZBy(270 - model->getRotation().Y, core::vector3df(0,0,0));

		// Scale by time elapsed
		movement *= timeElapsed;

		model->setPosition(pos + movement);
	
		// Slow the jump
		if(collider && collider->isFalling() && currentUpSpeed > 0) currentUpSpeed += -0.2f; // collider->getGravity().Y * 300;

		// Allow player to refire
		if(shooting) shoot(); 
		if(shootTimeRemaining > 0) shootTimeRemaining -= timeElapsed;

	}

}

void IPerson::setAnimation(scene::EMD2_ANIMATION_TYPE newAnimation, s32 howLong)
{
	if(howLong < 0)
	{
		if(newAnimation != permanentAnimation && temporaryAnimationTimeLeft < 0)
		{
			model->setMD2Animation(newAnimation);
		 	permanentAnimation = newAnimation;
		}
	}
	else
	{
		temporaryAnimation = newAnimation;
		model->setMD2Animation(temporaryAnimation);
		temporaryAnimationTimeLeft = howLong;
	}
}

bool IPerson::shoot()
{
	if(shootTimeRemaining > 0) return false; // Can't shoot yet; too soon!
	else shootTimeRemaining = refireTime;

	// find out what the person's looking at
	core::vector3df start, end, direction;
	start = getPosition();
	direction = getDirection();

	start = start + direction * 5.0f; // move it forward a tiny bit
	end = start + direction;
	core::line3d<f32> line(start, end);

//	OutputStreamPtr ballSound = OpenSound(scenario->audiereDevice.get(), "media\\sound\\44.wav", false);
	scenario->ballSound->play();

	// The bullet will manage its own lifespan
	IProjectile* bullet = 0;
	if(scenario->getBulletTime()) bullet = new CSlowMotionBulletProjectile(scenario, line, movableSelector);
	else bullet = new CBulletProjectile(scenario, line, movableSelector);
	bullet->fire(lastUpdateTime);

	return true; 
}


bool IPerson::find(core::vector3df what)
{
	ITriangleSelector* selector = scenario->getStationaryTangibleSelector();
	
	core::triangle3d<f32> garbage;
	core::line3d<f32> path(model->getPosition(), what);

	if(scenario->getDevice()->getSceneManager()->getSceneCollisionManager()->getCollisionPoint(path, selector, path.end, garbage))
		return false;
	else return true;
}

void IPerson::damage(core::triangle3df where, f32 howMuch)
{ 
	if((health -= howMuch) <= 0) die();
}


void IPerson::die()
{
	setAnimation(EMAT_DEATH_FALLBACK, -1);
	temporaryAnimationTimeLeft = 0;				// Temporary animations should die prematurely
	scenario->getAnimatableRegistry()->remove(this);
}

