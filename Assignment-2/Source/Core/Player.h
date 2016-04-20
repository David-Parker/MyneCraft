#pragma once

#include <vector>
#include <cmath>

#include "GameObject.h"
#include "OISManager.h"
#include "Chunk.h"

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

	enum ITEM_TYPE{ PICKAXE, SWORD, CANDLE, GRASS_CUBE, SNOW_CUBE, SAND_CUBE };

	void setWeapon(int);
	Ogre::SceneNode* getWeaponNode();
	void update(OISManager*);
	void clickAction(StaticObject* hitObj, std::unordered_map<std::pair<int, int>, Chunk*>& chunks, std::unordered_map<std::pair<int, int>, Chunk*>& modifiedChunks);
	void pickaxeAction(StaticObject* hitObj, std::unordered_map<std::pair<int, int>, Chunk*>& chunks, std::unordered_map<std::pair<int, int>, Chunk*>& modifiedChunks);
};