#pragma once
#include <random>
#include <vector>
#include <math.h>
#include <Ogre.h>
#include <unordered_map>

#include "Biome.h"
#include "StaticObject.h"


static Ogre::Entity* grassTree = nullptr;
static Ogre::Entity* snowTree = nullptr;
static Ogre::Entity* sandTree = nullptr;

static int biomeGridSize = 2000;
static int minBiomeRadius = 500;
static int biomeRadiusVariance = 300;

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
	std::unordered_map<std::string, Biome*> biomeGrid;
	std::vector<Biome*> worldBiomes;
	Ogre::SceneManager* mSceneManager;
	Ogre::Entity* grassMesh;
	Ogre::Entity* snowMesh;
	Ogre::Entity* sandMesh;
};