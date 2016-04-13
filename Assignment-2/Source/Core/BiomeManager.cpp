#include "BiomeManager.h"

BiomeManager::BiomeManager(Ogre::SceneManager* smgr) : mSceneManager(smgr) {
	worldBiomes.push_back(createBiome(Biome::SAND, 500, 500, 400));
	grassMesh = worldBiomes.at(0)->getEntity(Biome::GRASS);
	snowMesh = worldBiomes.at(0)->getEntity(Biome::SNOW);
	sandMesh = worldBiomes.at(0)->getEntity(Biome::SAND);
}


Biome* BiomeManager::inBiome(int x, int y) {
	std::pair<int, int> pair = std::pair<int, int>(x/biomeGridSize, y/biomeGridSize);

	if ( biomeGrid[pair] )
		return biomeGrid[pair];
	else {
		if ( true ) {
			int cX = (rand()%400) - 200 + ((int)(x/biomeGridSize))*biomeGridSize + biomeGridSize/2;
			int cY = (rand()%400) - 200 + ((int)(y/biomeGridSize))*biomeGridSize + biomeGridSize/2;
			int rad = rand()%biomeRadiusVariance + minBiomeRadius;
			int rndType = rand()%2+1;
			biomeGrid[pair] = createBiome((Biome::BiomeType)(rndType), cX, cY, rad);
			return biomeGrid[pair];
		}
	}
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

Ogre::Entity* BiomeManager::getTreeEntity(Biome::BiomeType type) {
	switch ( type ) {
		case Biome::GRASS: return grassTree;
		case Biome::SNOW: return snowTree;
		case Biome::SAND: return sandTree;
	}
	return nullptr;
}