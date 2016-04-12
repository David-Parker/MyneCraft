#include "BiomeManager.h"

BiomeManager::BiomeManager(Ogre::SceneManager* smgr) : mSceneManager(smgr) {
	worldBiomes.push_back(createBiome(Biome::SAND, 500, 500, 400));
	grassMesh = worldBiomes.at(0)->getEntity(Biome::GRASS);
	snowMesh = worldBiomes.at(0)->getEntity(Biome::SNOW);
	sandMesh = worldBiomes.at(0)->getEntity(Biome::SAND);
}


Biome* BiomeManager::inBiome(int x, int y) {
	std::string nme = getBiomeName(x, y);

	if ( biomeGrid[nme] )
		return biomeGrid[nme];
	else {
		if ( true ) {//rand()%2 == 0 ) {
			int cX = rand()%400 - 200 + ((int)(x/biomeGridSize))*biomeGridSize + biomeGridSize/2;
			int cY = rand()%400 - 200 + ((int)(y/biomeGridSize))*biomeGridSize + biomeGridSize/2;
			int rad = rand()%biomeRadiusVariance + minBiomeRadius;
			biomeGrid[nme] = createBiome((Biome::BiomeType)(rand()%2+1), cX, cY, rad);
			return biomeGrid[nme];
		}
		else
			return nullptr;
	}
}

Biome* BiomeManager::createBiome(Biome::BiomeType type, int x, int y, int r) {
	return new Biome(mSceneManager, type, x, y, r);
}

std::string BiomeManager::getBiomeName(int x, int z) {
	char buf[64];
	sprintf(buf, "Biome_%d_%d", x / biomeGridSize, z / biomeGridSize);
	return std::string(buf);
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