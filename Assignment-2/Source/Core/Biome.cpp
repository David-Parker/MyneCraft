#include "Biome.h"

Biome::Biome(Ogre::SceneManager* smgr, BiomeType type, int x, int y, int r) : mSceneManager(smgr), biomeType(type), centerX(x), centerY(y), radius(r) {

}


bool Biome::inBiome(int x, int y) {
	return (abs(x - centerX) < radius || abs(y - centerY) < radius);
}

Ogre::Entity* Biome::getCubeEntity(int x, int y) {
	int dSq = ((centerX - x) * (centerX - x)) + ((centerY - y) * (centerY - y));
	float rSQoN = radius * radius / 9;
	int rnd = rand()%20;
	if ( dSq < 2 * rSQoN ) {
		return mSceneManager->createEntity(getTypeString(biomeType));	
	}
	else if ( dSq < 4 * rSQoN && rnd < 17 ) {
		return mSceneManager->createEntity(getTypeString(biomeType));
	}
	else if ( dSq < 6 * rSQoN && rnd < 14 ) {
		return mSceneManager->createEntity(getTypeString(biomeType));
	}
	else if ( dSq < 7 * rSQoN && rnd < 9 ) {
		return mSceneManager->createEntity(getTypeString(biomeType));
	}
	else if ( dSq < 8 * rSQoN && rnd < 4 ) {
		return mSceneManager->createEntity(getTypeString(biomeType));
	}
	else if ( dSq < 9 * rSQoN && rnd < 1 ) {
		return mSceneManager->createEntity(getTypeString(biomeType));
	}
	else {
		return mSceneManager->createEntity(getTypeString(GRASS));
	}	
}

std::string Biome::getTypeString(BiomeType type) {
	switch ( type ) {
		case GRASS: return "Cube-Grass.mesh";
		case SNOW: return "Cube-Snow.mesh";
		case SAND: return "Cube-Sand.mesh";
	}
}