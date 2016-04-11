#pragma once
#include <random>
#include <math.h>
#include <Ogre.h>
#include "StaticObject.h"

class Biome {
public:
	enum BiomeType{ GRASS, SNOW, SAND, NUM_BIOMES };

	Biome(Ogre::SceneManager*, BiomeType, int, int, int);
	~Biome(void);

	bool inBiome(int, int);
	Ogre::Entity* getCubeEntity(int, int);

protected:
	std::string getTypeString(BiomeType);
	Ogre::SceneManager* mSceneManager;
	BiomeType biomeType;
	int centerX;
	int centerY;
	int radius;
};