#include "Biome.h"

Biome::Biome(Ogre::SceneManager* smgr, BiomeType type, int x, int y, int r) : mSceneManager(smgr), biomeType(type), centerX(x), centerY(y), radius(r) {
	
	grassCube = mSceneManager->createEntity("Cube-Grass.mesh");
	snowCube = mSceneManager->createEntity("Cube-Snow.mesh");
	iceCube = mSceneManager->createEntity("Cube-Ice.mesh");
	sandCube = mSceneManager->createEntity("Cube-Sand.mesh");
	rockCube = mSceneManager->createEntity("Cube-Rock.mesh");
	dirtCube = mSceneManager->createEntity("Cube-Dirt.mesh");
	woodCube = mSceneManager->createEntity("Cube-Wood.mesh");
	leafCube = mSceneManager->createEntity("Cube-Leaves.mesh");
	cactusCube = mSceneManager->createEntity("Cube-Cactus.mesh");
	torchCube = mSceneManager->createEntity("Cube-Torch.mesh");
	plankCube = mSceneManager->createEntity("Cube-Plank.mesh");
	glassCube = mSceneManager->createEntity("Cube-Glass.mesh");
	cloudCube = mSceneManager->createEntity("Cube-Cloud.mesh");
}


bool Biome::inBiome(int x, int y) {
	return (abs((x < 0 ? -x : x) - (centerX < 0 ? -centerX : centerX)) < radius || abs((y < 0 ? -y : y) - (centerY < 0 ? -centerY : centerY)) < radius);
}

Ogre::Entity* Biome::getCubeEntity(int x, int y, int h, BiomeType& type) {
	int dSq = ((centerX - x) * (centerX - x)) + ((centerY - y) * (centerY - y));
	float rSQoN = radius * radius / 9;
	int rnd = Rand::rand()%20;
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

Ogre::Entity* Biome::getEntity(BiomeType type) {
	switch ( type ) {
		case GRASS: return grassCube;
		case SNOW: return snowCube;
		case ICE: return iceCube;
		case SAND: return sandCube;
		case ROCK: return rockCube;
		case DIRT: return dirtCube;
		case WOOD: return woodCube;
		case LEAF: return leafCube;
		case CACTUS: return cactusCube;
		case TORCH: return torchCube;
		case PLANK: return plankCube;
		case GLASS: return glassCube;
		case CLOUD: return cloudCube;
		default: return nullptr;
	}
}