#pragma once
#include <random>
#include <vector>
#include <math.h>
#include <utility>
#include <map>

#include <Ogre.h>

#include "Biome.h"
#include "StaticObject.h"


static Ogre::Entity* grassTree = nullptr;
static Ogre::Entity* snowTree = nullptr;
static Ogre::Entity* sandTree = nullptr;

static int biomeGridSize = 1500;
static int minBiomeRadius = 500;
static int biomeRadiusVariance = 200;

class BiomeManager {
public:
	BiomeManager(Ogre::SceneManager*);
	~BiomeManager(void);

	Biome* inBiome(int, int);
	Ogre::Entity* getTerrain(Biome::BiomeType);
	Ogre::Entity* getTreeEntity(Biome::BiomeType);

protected:
	Biome* createBiome(Biome::BiomeType, int, int, int);
	std::string getBiomeName(int, int);
	std::map<std::pair<int, int>, Biome*> biomeGrid;
	std::vector<Biome*> worldBiomes;
	Ogre::SceneManager* mSceneManager;
	Ogre::Entity* grassMesh;
	Ogre::Entity* snowMesh;
	Ogre::Entity* sandMesh;
};