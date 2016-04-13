#include "BiomeManager.h"

BiomeManager::BiomeManager(Ogre::SceneManager* smgr) : mSceneManager(smgr) {
	worldBiomes.push_back(createBiome(Biome::SAND, 0, 0, 0));
	grassMesh = worldBiomes.at(0)->getEntity(Biome::GRASS);
	snowMesh = worldBiomes.at(0)->getEntity(Biome::SNOW);
	sandMesh = worldBiomes.at(0)->getEntity(Biome::SAND);
}


Biome* BiomeManager::inBiome(int x, int y) {
	std::pair<int, int> pair = std::pair<int, int>(x/biomeGridSize, y/biomeGridSize);

	pair.first = x < 0 ? -(pair.first+1) : pair.first;
	pair.second = y < 0 ? -(pair.second+1) : pair.second;

	if ( biomeGrid[pair] )
		return biomeGrid[pair];
	else {
		if ( true ) {
			int cX = pair.first * biomeGridSize;
			int cY = pair.second * biomeGridSize;
			if ( cX >= 0 ) {
				cX += biomeGridSize/2;
			}
			else {
				cX += biomeGridSize/2;
			}
			if ( cY >= 0 ) {
				cY += biomeGridSize/2;
			}
			else {
				cY += biomeGridSize/2;
			}
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