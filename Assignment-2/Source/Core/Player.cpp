#include "Player.h"

Player::Player(Ogre::Camera* camera, GameObject* body, Ogre::SceneManager* sm) : _body(body), _playerCam(camera), _sceneManager(sm) {
	_body->getNode()->setVisible(false);
	_body->getBody()->setAngularFactor(btVector3(0, 0, 0));

	for(int i = 0; i < averageSize; i++) {
		camAvg[i] = Ogre::Vector3::ZERO;
	}

	_pickaxe = _sceneManager->createEntity("Pickaxe", "Mynecraft-Pickaxe.mesh");
	_pickaxe->setCastShadows(true);
	_axeNode = _sceneManager->getRootSceneNode()->createChildSceneNode("Pickaxe");
	_axeNode->attachObject(_pickaxe);
	_axeNode->setDirection(Ogre::Vector3(0, 1 ,0));
	_axeNode->setScale(5, 5, 5);
}

void Player::update(OISManager* ois) {
	static int speed = 1000;
	static int camAvgIndex = 0;

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
		_body->setVelocity(_body->getBody()->getLinearVelocity().x(), 2500, _body->getBody()->getLinearVelocity().z());
		moved = true;
		_body->canJump = false;
	}
	if (ois->isKeyDown(OIS::KC_LSHIFT)) {
		speed = 2500;
	}
	else {
		speed = 1000;
	}
	if (!moved) {
		_body->setVelocity(0, currentY, 0);
	}

	camAvg[camAvgIndex % averageSize] = _body->getNode()->getPosition() + Ogre::Vector3(0, 170, 0);
	camAvgIndex++;

	Ogre::Vector3 total(0,0,0);
	int n = 0;

	for(int i = 0; i < averageSize; i++) {
		if(camAvg[i] != Ogre::Vector3::ZERO) {
			total += camAvg[i];
			n++;
		}
	}

	if(n > 0)
		total /= n;

	_playerCam->setPosition(total);

	Ogre::Vector3 fn = _playerCam->getDirection();
	Ogre::Vector3 norm = Ogre::Vector3(fn.x, 0, fn.z).normalisedCopy();
	Ogre::Degree theta = norm.angleBetween(Ogre::Vector3::UNIT_X);
	Ogre::Degree phi = fn.angleBetween(Ogre::Vector3::UNIT_Y);

	Ogre::Real rx, ry, rz;
	int sign = (norm.z < 0) ? -1 : 1;
	
	theta = theta + sign*angleOffset;
	rx = axeDistance*Ogre::Math::Cos(theta)*Ogre::Math::Sin(phi);
	ry = axeDistance*Ogre::Math::Cos(phi);
	rz = sign*axeDistance*Ogre::Math::Sin(theta)*Ogre::Math::Sin(phi);

	_axeNode->setPosition(total + Ogre::Vector3(rx, ry, rz));

	Ogre::Quaternion quat(norm.crossProduct(Ogre::Vector3::NEGATIVE_UNIT_Y), norm, Ogre::Vector3::NEGATIVE_UNIT_Y);
	_axeNode->setOrientation(quat);	
	if ( fn.y < 0 )
		_axeNode->pitch(phi - Ogre::Degree(70));
	else _axeNode->pitch(Ogre::Degree(20));
	_axeNode->roll(Ogre::Degree(90));
}