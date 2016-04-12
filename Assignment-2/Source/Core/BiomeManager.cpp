#include "BiomeManager.h"

BiomeManager::BiomeManager(Ogre::SceneManager* smgr) : mSceneManager(smgr) {
	worldBiomes.push_back(createBiome(Biome::SAND, 500, 500, 400));
	grassMesh = worldBiomes.at(0)->getType(Biome::GRASS);
	snowMesh = worldBiomes.at(0)->getType(Biome::SNOW);
}


Biome* BiomeManager::inBiome(int x, int y) {
	for ( auto& biome : worldBiomes )
		if ( biome->inBiome(x, y) )
			return biome;

	return nullptr;
}

Biome* BiomeManager::createBiome(Biome::BiomeType type, int x, int y, int r) {
	return new Biome(mSceneManager, type, x, y, r);
}


Ogre::Entity* BiomeManager::getGrassTerrain() {
	return grassMesh;
}

Ogre::Entity* BiomeManager::getSnowTerrain() {
	return snowMesh;
}