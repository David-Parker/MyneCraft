#include "Player.h"

Player::Player(Ogre::Camera* camera, GameObject* body) : _body(body), _playerCam(camera) {
	_body->getNode()->setVisible(false);
}

void Player::update(OISManager* ois) {
	static int speed = 2500;

	Ogre::Vector3 movePos = _playerCam->getDirection();
	movePos = Ogre::Vector3(movePos.x, 0, movePos.z);
	movePos.normalise();
	Ogre::Vector3 strafePos = movePos.crossProduct(Ogre::Vector3(0, 1, 0));
	float currentY = _body->getBody()->getLinearVelocity().y();

	OIS::KeyCode lastkey;
	bool moved = false;

	if (ois->isKeyDown(OIS::KC_W)) {
		_body->setVelocity(movePos.x*speed, currentY, movePos.z*speed);
		moved = true;
	}
	if (ois->isKeyDown(OIS::KC_S)) {
		_body->setVelocity(movePos.x*-speed, currentY, movePos.z*-speed);
		moved = true;
	}
	if (ois->isKeyDown(OIS::KC_D)) {
		_body->setVelocity(strafePos.x*speed, currentY, strafePos.z*speed);
		moved = true;
	}
	if (ois->isKeyDown(OIS::KC_A)) {
		_body->setVelocity(strafePos.x*-speed, currentY, strafePos.z*-speed);
		moved = true;
	}
	if (ois->isKeyDown(OIS::KC_SPACE) && _body->canJump) {
		_body->applyImpulse(btVector3(0, 1, 0) * 700, btVector3(0, 0, 0));
		moved = true;
		_body->canJump = false;
	}
	if (ois->isKeyDown(OIS::KC_LSHIFT)) {
		speed = 5000;
	}
	else {
		speed = 2500;
	}
	if (!moved) {
		_body->setVelocity(0, _body->getBody()->getLinearVelocity().y(), 0);
	}

	_playerCam->setPosition(_body->getNode()->getPosition() + Ogre::Vector3(0, 200, 0));
}