#include "Player.h"

Player::Player(Ogre::Camera* camera, GameObject* body) : _body(body), _playerCam(camera) {

}

void Player::update(OIS::KeyCode lastKey) {
	static int speed = 2000;

	_playerCam->setPosition(_body->getNode()->getPosition() + Ogre::Vector3(0, 200, 0));

	Ogre::Vector3 movePos = _playerCam->getDirection();
	movePos = Ogre::Vector3(movePos.x, 0, movePos.z);
	movePos.normalise();
	Ogre::Vector3 strafePos = movePos.crossProduct(Ogre::Vector3(0, 1, 0));

	btVector3 btPos = btVector3(movePos.x, 0, movePos.z);
	btVector3 btStrafePos = btVector3(strafePos.x, 0, strafePos.z);

	switch(lastKey) {
	case OIS::KC_W: _body->applyImpulse(btPos * speed, btVector3(0, 0, 0));
		break;
	case OIS::KC_S: _body->applyImpulse(btPos * -speed, btVector3(0, 0, 0));
		break;
	case OIS::KC_D: _body->applyImpulse(btStrafePos * speed, btVector3(0, 0, 0));
		break;
	case OIS::KC_A: _body->applyImpulse(btStrafePos * -speed, btVector3(0, 0, 0));
		break;
	case OIS::KC_SPACE: _body->applyImpulse(btVector3(0, 1, 0) * speed, btVector3(0, 0, 0));
		break;
	}
}