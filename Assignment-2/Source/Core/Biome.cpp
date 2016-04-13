#include "Biome.h"

Biome::Biome(Ogre::SceneManager* smgr, BiomeType type, int x, int y, int r) : mSceneManager(smgr), biomeType(type), centerX(x), centerY(y), radius(r) {
	
	grassCube = mSceneManager->createEntity("Cube-Grass.mesh");
	snowCube = mSceneManager->createEntity("Cube-Snow.mesh");
	iceCube = mSceneManager->createEntity("Cube-Ice.mesh");
	sandCube = mSceneManager->createEntity("Cube-Sand.mesh");
	woodCube = mSceneManager->createEntity("Cube-Wood.mesh");
	leafCube = mSceneManager->createEntity("Cube-Leaves.mesh");
	cactusCube = mSceneManager->createEntity("Cube-Cactus.mesh");
}


bool Biome::inBiome(int x, int y) {
	return (abs(x - centerX) < radius || abs(y - centerY) < radius);
}

Ogre::Entity* Biome::getCubeEntity(int x, int y, int h, BiomeType& type) {
	int dSq = ((centerX - x) * (centerX - x)) + ((centerY - y) * (centerY - y));
	float rSQoN = radius * radius / 9;
	int rnd = rand()%20;
	if ( (dSq < 2 * rSQoN)
			|| (dSq < 4 * rSQoN && rnd < 18)
			|| (dSq < 6 * rSQoN && rnd < 14)
			|| (dSq < 7 * rSQoN && rnd < 9)
			|| (dSq < 8 * rSQoN && rnd < 4)
			|| (dSq < 9 * rSQoN && rnd < 1) ) {
		type = biomeType;
		return getEntity(biomeType);
	}
	else if ( h >= 15 && dSq > 6 * rSQoN ) {
		type = SNOW;
		return getEntity(SNOW);
	}
	else {
		type = GRASS;
		return getEntity(GRASS);
	}
}

Ogre::Entity* Biome::getTreeEntity(BiomeType type) {
	switch ( type ) {
		default: return nullptr;
	}
}

Ogre::Entity* Biome::getEntity(BiomeType type) {
	switch ( type ) {
		case GRASS: return grassCube;
		case SNOW: return snowCube;
		case ICE: return iceCube;
		case SAND: return sandCube;
		case WOOD: return woodCube;
		case LEAF: return leafCube;
		case CACTUS: return cactusCube;
	}
}