#pragma once

#include "GameObject.h"
#include "OISManager.h"

class Player {
private:
	const static int averageSize = 10;
	Ogre::Vector3 camAvg[averageSize];
public:
	Player(Ogre::Camera*, GameObject* body);
	Ogre::Camera* _playerCam;
	GameObject* _body;
	Ogre::SceneManager* _sceneManager;

	void update(OISManager*);
};