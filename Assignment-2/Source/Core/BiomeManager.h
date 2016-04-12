#pragma once
#include <random>
#include <vector>
#include <math.h>
#include <Ogre.h>
#include "Biome.h"
#include "StaticObject.h"

static int biomeSeparation = 1500;
static int minBiomeRadius = 400;
static int biomeRadiusVariance = 400;

class BiomeManager {
public:
	BiomeManager(Ogre::SceneManager*);
	~BiomeManager(void);

	Biome* inBiome(int, int);
	Ogre::Entity* getGrassTerrain();
	Ogre::Entity* getSnowTerrain();

protected:
	Biome* createBiome(Biome::BiomeType, int, int, int);
	std::vector<Biome*> worldBiomes;
	Ogre::SceneManager* mSceneManager;
	Ogre::Entity* grassMesh;
	Ogre::Entity* snowMesh;
};