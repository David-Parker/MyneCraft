#pragma once
#include <random>
#include <math.h>
#include <Ogre.h>
#include "StaticObject.h"

class Biome {
public:
	enum { SNOW, NUM_SOUNDS } biomes;

	Biome(Ogre::SceneManager*, int, int, int);
	~Biome(void);

	bool inBiome(int, int);
	Ogre::Entity* getCubeEntity(int, int);

protected:
	Ogre::SceneManager* mSceneManager;
	int centerX;
	int centerY;
	int radius;
};