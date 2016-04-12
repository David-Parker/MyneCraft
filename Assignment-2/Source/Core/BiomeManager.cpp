#include "BiomeManager.h"

BiomeManager::BiomeManager(Ogre::SceneManager* smgr) : mSceneManager(smgr) {
	worldBiomes.push_back(createBiome(Biome::SAND, 500, 500, 400));
	grassMesh = worldBiomes.at(0)->getEntity(Biome::GRASS);
	snowMesh = worldBiomes.at(0)->getEntity(Biome::SNOW);
	sandMesh = worldBiomes.at(0)->getEntity(Biome::SAND);
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

Ogre::Entity* BiomeManager::getTerrain(Biome::BiomeType type) {
	switch ( type ) {
		case Biome::GRASS: return grassMesh;
		case Biome::SNOW: return snowMesh;
		case Biome::SAND: return sandMesh;
	}
	return nullptr;
}

Ogre::Entity* getTreeEntity(Biome::BiomeType type) {
	switch ( type ) {
		case Biome::GRASS: return grassTree;
		case Biome::SNOW: return snowTree;
		case Biome::SAND: return sandTree;
	}
	return nullptr;
}