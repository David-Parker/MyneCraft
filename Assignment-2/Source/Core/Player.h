#pragma once

#include <vector>
#include <cmath>

#include "GameObject.h"
#include "OISManager.h"

const static int averageSize = 10;
const static int axeDistance = 70;
static Ogre::Degree angleOffset = Ogre::Degree(20);

class Player {
private:
	Ogre::Vector3 camAvg[averageSize];
	Ogre::SceneManager* _sceneManager;
	int equippedItem;
	std::vector<Ogre::SceneNode*> inventory;

public:
	Player(Ogre::Camera*, GameObject* body, Ogre::SceneManager* sm);
	Ogre::Camera* _playerCam;
	GameObject* _body;

	enum BiomeType{ PICKAXE, SWORD, CANDLE };

	void setWeapon(int);
	Ogre::SceneNode* getWeaponNode();
	void update(OISManager*);
};