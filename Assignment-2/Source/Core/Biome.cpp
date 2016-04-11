#include "Biome.h"

Biome::Biome(Ogre::SceneManager* smgr, BiomeType type, int x, int y, int r) : mSceneManager(smgr), biomeType(type), centerX(x), centerY(y), radius(r) {
	
	grassCube = mSceneManager->createEntity("Cube-Grass.mesh");
	snowCube = mSceneManager->createEntity("Cube-Snow.mesh");	
	sandCube = mSceneManager->createEntity("Cube-Sand.mesh");
}


bool Biome::inBiome(int x, int y) {
	return (abs(x - centerX) < radius || abs(y - centerY) < radius);
}

Ogre::Entity* Biome::getCubeEntity(int x, int y) {
	int dSq = ((centerX - x) * (centerX - x)) + ((centerY - y) * (centerY - y));
	float rSQoN = radius * radius / 9;
	int rnd = rand()%20;
	if ( dSq < 2 * rSQoN ) {
		return getType(biomeType);	
	}
	else if ( dSq < 4 * rSQoN && rnd < 17 ) {
		return getType(biomeType);
	}
	else if ( dSq < 6 * rSQoN && rnd < 14 ) {
		return getType(biomeType);
	}
	else if ( dSq < 7 * rSQoN && rnd < 9 ) {
		return getType(biomeType);
	}
	else if ( dSq < 8 * rSQoN && rnd < 4 ) {
		return getType(biomeType);
	}
	else if ( dSq < 9 * rSQoN && rnd < 1 ) {
		return getType(biomeType);
	}
	else {
		return getType(GRASS);
	}	
}

Ogre::Entity* Biome::getType(BiomeType type) {
	switch ( type ) {
		case GRASS: return grassCube;
		case SNOW: return snowCube;
		case SAND: return sandCube;
	}
}