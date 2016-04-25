#pragma once

#include <random>
#include <math.h>
#include <Ogre.h>

#include "CubeManager.h"

class Biome {
public:
	Biome(Ogre::SceneManager*, CubeManager::CubeType, int, int, int);
	~Biome(void);

	bool inBiome(int, int);
	Ogre::Entity* getCubeEntity(int, int, int, CubeManager::CubeType&);
	CubeManager::CubeType getType() { return biomeType; }

protected:
	Ogre::SceneManager* mSceneManager;
	CubeManager::CubeType biomeType;
	int centerX;
	int centerY;
	int radius;
};