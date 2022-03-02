#include "FOV.h"
#include "EventFOV.h"
#include "EventCollision.h"
#include "WorldManager.h"
#include "LogManager.h"

FOV::FOV(Enemy* ene)
{
	setAltitude(3);
	myEnemy = ene;
	setType("FOV");
	setSprite("fov");
	setSolidness(df::SOFT);
	setPosition(ene->getPosition());
	obstacleCollision = false;
}

FOV::FOV() {
	FOV(new Enemy);
}

FOV::~FOV()
{
	WM.removeObject(this);
}

int FOV::eventHandler(const df::Event* p_e) {
	if ((p_e->getType() == df::COLLISION_EVENT)) {
		const df::EventCollision* p_collision_event = dynamic_cast <const df::EventCollision*> (p_e);
		bool FOV = ((p_collision_event->getObject1()->getType() == "FOV") || (p_collision_event->getObject2()->getType() == "FOV"));
		bool player = ((p_collision_event->getObject1()->getType() == "Player") || (p_collision_event->getObject2()->getType() == "Player"));
		bool obstacle = ((p_collision_event->getObject1()->getType() == "Obstacle") || (p_collision_event->getObject2()->getType() == "Obstacle"));
		if ((FOV && player)) {
			if ((p_collision_event->getObject1()->getType() == "Player")) {
				myEnemy->shoot();
			}
			else {
				myEnemy->shoot();
			}
			return 1;
		}
		else if (FOV && obstacle && ((myEnemy->getMovement()==horizontal )|| (myEnemy->getMovement() == vertical))) {
			LM.writeLog("FOV Found Obstacle");
			//setObstacleCollision(true);
			//setVelocity(df::Vector(-1 * getVelocity().getX(), -1 * getVelocity().getY()));
			
		}
	}
	return 0;//Ignores Event
}

void FOV::setObstacleCollision(bool c)
{
	obstacleCollision = c;
}

bool FOV::getObstacleCollision() {
	return obstacleCollision;
}
void FOV::move(float x, float y)
{
	//Move if in window
	/*df::Vector new_pos(getPosition().getX() + x, getPosition().getY() + y);
	setVelocity(new_pos);
	WM.moveObject(this, new_pos);*/
}

/*
float FOV::getVisionX()
{
	return vision_x;
}

void FOV::setVisionX(float new_vision_x)
{
	vision_x = new_vision_x;
}

float FOV::getVisionY()
{
	return vision_y;
}

void FOV::setVisionY(float new_vision_y)
{
	vision_y = new_vision_y;
}
*/