#include "Biome.h"

Biome::Biome(Ogre::SceneManager* smgr, BiomeType type, int x, int y, int r) : mSceneManager(smgr), biomeType(type), centerX(x), centerY(y), radius(r) {
	
	grassCube = mSceneManager->createEntity("Cube-Grass.mesh");
	snowCube = mSceneManager->createEntity("Cube-Snow.mesh");	
	sandCube = mSceneManager->createEntity("Cube-Sand.mesh");
}


bool Biome::inBiome(int x, int y) {
	return (abs(x - centerX) < radius || abs(y - centerY) < radius);
}

Ogre::Entity* Biome::getCubeEntity(int x, int y, int h) {
	int dSq = ((centerX - x) * (centerX - x)) + ((centerY - y) * (centerY - y));
	float rSQoN = radius * radius / 9;
	int rnd = rand()%20;
	if ( (dSq < 2 * rSQoN)
			|| (dSq < 4 * rSQoN && rnd < 18)
			|| (dSq < 6 * rSQoN && rnd < 14)
			|| (dSq < 7 * rSQoN && rnd < 9)
			|| (dSq < 8 * rSQoN && rnd < 4)
			|| (dSq < 9 * rSQoN && rnd < 1) )
		return getEntity(biomeType);
	else if ( h >= 15 && dSq < 7 * rSQoN )
		return getEntity(SNOW);
	else 
		return getEntity(GRASS);	
}

Ogre::Entity* Biome::getEntity(BiomeType type) {
	switch ( type ) {
		case GRASS: return grassCube;
		case SNOW: return snowCube;
		case SAND: return sandCube;
	}
}