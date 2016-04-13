#pragma once
#include <random>
#include <math.h>
#include <Ogre.h>

static Ogre::Entity* grassCube = nullptr;
static Ogre::Entity* snowCube = nullptr;
static Ogre::Entity* iceCube = nullptr;
static Ogre::Entity* sandCube = nullptr;
static Ogre::Entity* woodCube = nullptr;
static Ogre::Entity* leafCube = nullptr;
static Ogre::Entity* cactusCube = nullptr;

class Biome {
public:
	enum BiomeType{ GRASS, SNOW, SAND, ICE, WOOD, LEAF, CACTUS, NUM_BIOMES };

	Biome(Ogre::SceneManager*, BiomeType, int, int, int);
	~Biome(void);

	bool inBiome(int, int);
	Ogre::Entity* getCubeEntity(int, int, int, BiomeType*);
	Ogre::Entity* getTreeEntity(BiomeType type);
	Ogre::Entity* getEntity(BiomeType);
	BiomeType getType() { return biomeType; }

protected:
	Ogre::SceneManager* mSceneManager;
	BiomeType biomeType;
	int centerX;
	int centerY;
	int radius;
};