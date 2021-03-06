#include "Player.h"
#include "Bullet.h"
#include "Reticle.h"
#include "GameManager.h"
#include "LogManager.h"
#include "ResourceManager.h"
#include "WorldManager.h"
#include "Event.h"
#include "EventHit.h"
#include "EventOut.h"
#include "EventStep.h"
#include "EventCollision.h"
#include "EventHealthPack.h"
#include "EventView.h"
#include "EventLevel.h"
#include "ViewObject.h"
#include "GameOver.h"
#include "LevelManager.h"

#pragma comment(lib, "proj2-Donnelly.lib")

Player::Player() {
	if (-1 == WM.setViewFollowing(this)) {
		LM.writeLog("HERE");
	}
}

Player::~Player() {
	// Create GameOver object.
	//new GameOver;
	//WM.markForDelete(this);
	// Mark Reticle for deletion.
}

void Player::start() {
	Reticle* r = new Reticle;
	setType("Player");
	setAltitude(3);
	setSolidness(df::HARD);
	setSprite("playerL");
	setIntel(0);
	setSpeed(.25);
	setPosition(df::Vector(40, 12));

	move_slowdown = 5;
	move_countdown = move_slowdown;
	setActive();
	bullets = 3;
	lives = 3;


	//Displays available bullets
	df::ViewObject* p_vo = new df::ViewObject;
	p_vo->setLocation(df::TOP_RIGHT);
	p_vo->setViewString("Bullets");
	p_vo->setValue(3);
	LM.writeLog("%d", p_vo->getValue());
	p_vo->setDrawValue(true);
	p_vo->setColor(df::YELLOW);


	//Displays lives
	df::ViewObject* p_vo2 = new df::ViewObject;
	p_vo2->setLocation(df::TOP_LEFT);
	p_vo2->setViewString("Lives");
	p_vo2->setValue(3);
	p_vo2->setDrawValue(true);
	p_vo2->setColor(df::YELLOW);


	//Displays levels
	df::ViewObject* p_vo3 = new df::ViewObject;
	p_vo3->setLocation(df::BOTTOM_LEFT);
	p_vo3->setViewString("Level");
	p_vo3->setValue(1);
	p_vo3->setDrawValue(true);
	p_vo3->setColor(df::YELLOW);

	levelM.insertProtected(this);
	levelM.insertProtected(r);
	levelM.insertProtected(p_vo);
	levelM.insertProtected(p_vo2);
	levelM.insertProtected(p_vo3);

	target = NULL;
	sneakAttack = false;
}

void Player::shutDown() {
	valueReset();
	WM.markForDelete(this);
}

Player& Player::getInstance() {
	static Player player;
	return player;
}

//Handles events
	//Returns 0 when ignored
int Player::eventHandler(const df::Event* p_e) {
	if (p_e->getType() == df::KEYBOARD_EVENT/* && active*/) {
		df::EventKeyboard* key = (df::EventKeyboard*)p_e;
		keyboard(key);
		return 1;
	}if (p_e->getType() == df::MSE_EVENT/* && active*/) {
		const df::EventMouse* p_mouse_event = dynamic_cast <df::EventMouse const*> (p_e);
		mouse(p_mouse_event);

	}
	if (p_e->getType() == df::COLLISION_EVENT) {
		const df::EventCollision* p_collision_event = dynamic_cast <df::EventCollision const*> (p_e);
		bool enemy = ((p_collision_event->getObject1()->getType() == "Enemy") || (p_collision_event->getObject2()->getType() == "Enemy"));
		bool player = ((p_collision_event->getObject1()->getType() == "Player") || (p_collision_event->getObject2()->getType() == "Player"));
		if (enemy && player) {
			if (p_collision_event->getObject1()->getType() == "Enemy") {
				target=(Enemy*) p_collision_event->getObject1();
			}
			else {
				target = (Enemy*)p_collision_event->getObject2();
			}
			sneakAttack = true;
		}
		else {
			sneakAttack = false;
		}
		return 1;
	}
	if (p_e->getType() == HIT_EVENT) {
		lives--;
		delta_lives++;
		LM.writeLog("Lives %d Delta %d", lives, delta_lives);
		if (lives >= 0) {
			df::EventView* ev = new df::EventView("Lives", -1, true);
			WM.onEvent(ev);
		}
		if (lives == 0) {
			new GameOver;
		}
		return 1;
	}
	if (p_e->getType() == HP_EVENT) {
		lives++;
		delta_lives--;
		lives--;
		delta_lives++;
		LM.writeLog("Lives %d Delta %d", lives, delta_lives);
		df::EventView* ev = new df::EventView("Lives", 1, true);
		WM.onEvent(ev);
		return 1;
	}
	return 0;
}

//Moves Player up and down
void Player::move(float x, float y) {
	if ((move_countdown < 0)) {
		return;
	}
	if (active) {
		move_countdown = move_slowdown;
		//Move if in window
		df::Vector new_pos(getPosition().getX() + x, getPosition().getY() + y);

		WM.moveObject(this, new_pos);
	}
	else {
		LM.writeLog("Not active");
		return;
	}
	
}

//Keyboard event
int Player::keyboard(const df::EventKeyboard* key) {
	switch (key->getKey()) {
	case df::Keyboard::R: //Reset Level
		if (key->getKeyboardAction() == df::KEY_PRESSED) {
			levelM.resetLevel();
		}
		break;
	case df::Keyboard::L: //Next Level
		if (key->getKeyboardAction() == df::KEY_PRESSED) {
			levelM.nextLevel();
		}
		break;
	case df::Keyboard::X: //Hit Test
		if (key->getKeyboardAction() == df::KEY_PRESSED) {
			EventHit eh;
			WM.onEvent(&eh);
		}
		break;
	case df::Keyboard::Q:			// quit
		if (key->getKeyboardAction() == df::KEY_PRESSED)
			GM.setGameOver();
			WM.markForDelete(this);
		return 1;
	case df::Keyboard::W:       // up
		if (key->getKeyboardAction() == df::KEY_DOWN)
			LM.writeLog(0, "Up");
			move(0, -1);
		return 1;
	case df::Keyboard::S:       // down
		if (key->getKeyboardAction() == df::KEY_DOWN)
			LM.writeLog(0, "Down");
			move(0, +1);
		return 1;
	case df::Keyboard::A:       // left
		if (key->getKeyboardAction() == df::KEY_DOWN)
			LM.writeLog(0, "Left");
			setSprite("playerL");
			move(-1, 0);
		return 1;
	case df::Keyboard::D:       // right
		if (key->getKeyboardAction() == df::KEY_DOWN)
			LM.writeLog(0, "Right");
			setSprite("playerR");
			move(+1, 0);
		return 1;
	default: // Key not handled.
		return 0;
	};
	return 0;
}
//Mouse event
int Player::mouse(const df::EventMouse* mouse) {
	if (mouse->getMouseAction() == df::EventMouseAction::PRESSED) {
		if (mouse->getMouseButton() == df::Mouse::LEFT) {
			return shoot(mouse);
		}
		else {
			if (sneakAttack) {
				WM.markForDelete(target);
			}
			return 1;
		}
	}
	return 0;
}

//Player shoots bullet
int Player::shoot(const df::EventMouse* mouse) {
	//Time to fire
	if (fire_countdown > 0) {
		return 0;
	}
	fire_countdown = fire_slowdown;

	if (bullets > 0) {
		bullets--;
		delta_bullets++;
		df::EventView* ev = new df::EventView("Bullets", -1, true);
		WM.onEvent(ev);

		//Fire sound
		df::Sound* p_sound = RM.getSound("fire");
		p_sound->play(false);
		df::Vector pos = mouse->getMousePosition();
		// Compute normalized vector to position, then scale by speed 
		df::Vector v(pos.getX() - getPosition().getX(), pos.getY() - getPosition().getY());
		v.normalize();
		v.scale(1);
		Bullet* b = new Bullet(getPosition(), getId());
		b->setVelocity(v);
		return 1;
	}
	return 0;
}
//If level is reset values are reset
void Player::valueReset() {

	lives = lives + delta_lives;
	bullets = bullets + delta_bullets;

	//Restores original heart and bullet counts
	df::EventView ev1("Lives", lives, false);
	df::EventView ev2("Bullets", bullets, false);

	WM.onEvent(&ev1);
	WM.onEvent(&ev2);
	resetDeltas();
}

//Sets deltas to 0
void Player::resetDeltas() {
	delta_bullets = 0;
	delta_lives = 0;
}
//True if player can move, false otherwise
void Player::setActive(bool isActive) {
	active = isActive;
}

void Player::setIntel(int ic)
{
	intelCount = ic;
}

int Player::getIntel()
{
	return intelCount;
}

int Player::getLives()
{
	return lives;
}
