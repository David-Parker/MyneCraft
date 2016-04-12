#pragma once
#include <random>
#include <vector>
#include <math.h>
#include <Ogre.h>
#include "Biome.h"
#include "StaticObject.h"

static Ogre::Entity* grassTree = nullptr;
static Ogre::Entity* snowTree = nullptr;
static Ogre::Entity* sandTree = nullptr;

static int biomeSeparation = 1500;
static int minBiomeRadius = 400;
static int biomeRadiusVariance = 400;

class BiomeManager {
public:
	BiomeManager(Ogre::SceneManager*);
	~BiomeManager(void);

	Biome* inBiome(int, int);
	Ogre::Entity* getTerrain(Biome::BiomeType);
	Ogre::Entity* getTreeEntity(Biome::BiomeType);

protected:
	Biome* createBiome(Biome::BiomeType, int, int, int);
	std::vector<Biome*> worldBiomes;
	Ogre::SceneManager* mSceneManager;
	Ogre::Entity* grassMesh;
	Ogre::Entity* snowMesh;
	Ogre::Entity* sandMesh;
};