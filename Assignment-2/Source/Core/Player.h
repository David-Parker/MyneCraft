#pragma once

#include <vector>
#include <cmath>

#include "CubeManager.h"
#include "GameObject.h"
#include "OISManager.h"
#include "Chunk.h"
#include "Animation.h"

const static int averageSize = 10;
const static int axeDistance = 70;
static Ogre::Degree angleOffset = Ogre::Degree(20);

class Player {
private:
	Ogre::Vector3 camAvg[averageSize];
	Ogre::SceneManager* _sceneManager;
	int equippedItem;
	std::vector<Ogre::SceneNode*> inventory;
	void getNeighborChunks(std::vector<Chunk*>& chunklist, int x, int z, std::unordered_map<std::pair<int, int>, Chunk*>& chunks, Chunk* chunk);
	Animation _animation;
public:
	Player(Ogre::Camera*, GameObject* body, Ogre::SceneManager* sm);
	Ogre::Camera* _playerCam;
	GameObject* _body;

	enum ITEM_TYPE{ PICKAXE, SWORD, TORCH_CUBE, GRASS_CUBE, ROCK_CUBE, SNOW_CUBE, SAND_CUBE, DIRT_CUBE, PLANK_CUBE, GLASS_CUBE, NUM_WEP };

	void setWeapon(int);
	Ogre::SceneNode* getWeaponNode();
	void update(OISManager*);
	bool clickAction(StaticObject* hitObj, const btVector3& hitnormal, std::unordered_map<std::pair<int, int>, Chunk*>& chunks, std::unordered_map<std::pair<int, int>, Chunk*>& modifiedChunks);
	void pickaxeAction(StaticObject* hitObj, std::unordered_map<std::pair<int, int>, Chunk*>& chunks, std::unordered_map<std::pair<int, int>, Chunk*>& modifiedChunks);
	void cubePlaceAction(StaticObject* hitObj, const btVector3& hitnormal, std::unordered_map<std::pair<int, int>, Chunk*>& chunks, std::unordered_map<std::pair<int, int>, Chunk*>& modifiedChunks, CubeManager::CubeType type);
	int getWeapon();
	void constrainSpeed();
};