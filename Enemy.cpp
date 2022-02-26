#include "Enemy.h"
#include "WorldManager.h"
#include "Event.h"
#include "EventHit.h"
#include "EventFOV.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "Vector.h"
#include "Sound.h"
#include "Bullet.h"

Enemy::Enemy()
{
	Enemy(df::Vector(70, 40), true);
}

Enemy::Enemy(df::Vector position, bool direction)
{
	setType("Enemy");
	setAltitude(3);
	setSolidness(df::HARD);
	setSpeed(1);
	setPosition(position);
	facingLeft = direction;
	if (facingLeft) {
		setSprite("enemyL");
		LM.writeLog("the sprite loaded");
	}
	else {
		setSprite("enemyR");
		LM.writeLog("the sprite loaded");
	}
}

Enemy::~Enemy()
{
}

int Enemy::eventHandler(const df::Event* p_e)
{
	if (p_e->getType() == HIT_EVENT) {
		return 1;
	}
	if (p_e->getType() == FOV_EVENT) {
		shoot();
		return 1;
	}
	return 0;
}

void Enemy::move(int x, int y)
{
	//Move if in window
	df::Vector new_pos(getPosition().getX() + x, getPosition().getY() + y);
	setVelocity(new_pos);
	WM.moveObject(this, new_pos);
	LM.writeLog("Enemy: Old Position (%f,%f), New Position (%f,%f)", getPosition().getX(), getPosition().getY(), new_pos.getX(), new_pos.getY());
}

int Enemy::shoot()
{
	//Fire sound
	df::Sound* p_sound = RM.getSound("fire");
	p_sound->play(false);
	// Compute normalized vector to position, then scale by speed 
	// Fire Bullet towards target.
	df::Vector v = getPosition();
	if (facingLeft) {
		v.setX(0);
	}
	else {
		v.setX(100); //figure out level boundary length to make it fire to the maximum
	}
	v.normalize();
	v.scale(1);
	Bullet* p = new Bullet(getPosition(), getId());
	p->setVelocity(v);
	return 1;
}
