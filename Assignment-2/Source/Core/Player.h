#pragma once

#include "GameObject.h"
#include "OISManager.h"

class Player {
public:
	Player(Ogre::Camera*, GameObject* body);
	Ogre::Camera* _playerCam;
	GameObject* _body;
	Ogre::SceneManager* _sceneManager;

	void update(OISManager*);
};