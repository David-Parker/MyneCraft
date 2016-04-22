#include "BiomeManager.h"

BiomeManager::BiomeManager(Ogre::SceneManager* smgr) : mSceneManager(smgr) {
	worldBiomes.push_back(createBiome(Biome::SAND, 0, 0, 0));
	grassMesh = worldBiomes.at(0)->getEntity(Biome::GRASS);
	snowMesh = worldBiomes.at(0)->getEntity(Biome::SNOW);
	sandMesh = worldBiomes.at(0)->getEntity(Biome::SAND);
	rockMesh = worldBiomes.at(0)->getEntity(Biome::ROCK);
	dirtMesh = worldBiomes.at(0)->getEntity(Biome::DIRT);
	torchMesh = worldBiomes.at(0)->getEntity(Biome::TORCH);
	plankMesh = worldBiomes.at(0)->getEntity(Biome::PLANK);
	cloudMesh = worldBiomes.at(0)->getEntity(Biome::CLOUD);
}


Biome* BiomeManager::inBiome(int x, int y) {
	std::pair<int, int> pair = std::pair<int, int>(x/biomeGridSize, y/biomeGridSize);

	pair.first = x < 0 ? pair.first-1 : pair.first;
	pair.second = y < 0 ? pair.second-1 : pair.second;

	if ( biomeGrid[pair] )
		return biomeGrid[pair];
	else {
		int cX = pair.first * biomeGridSize + biomeGridSize/2 + (rand()%positionVariance) - positionVariance/2;
		int cY = pair.second * biomeGridSize + biomeGridSize/2 + (rand()%positionVariance) - positionVariance/2;
		int rad = rand()%biomeRadiusVariance + minBiomeRadius;
		int rndType = rand()%3;
		
		biomeGrid[pair] = createBiome((Biome::BiomeType)(rndType), cX, cY, rad);
		return biomeGrid[pair];
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
		case Biome::ROCK: return rockMesh;
		case Biome::DIRT: return dirtMesh;
		case Biome::TORCH: return torchMesh;
		case Biome::PLANK: return plankMesh;
		case Biome::CLOUD: return cloudMesh;
	}
	return nullptr;
}

bool BiomeManager::isTreeType(Biome::BiomeType type) {
	return type == Biome::WOOD || type == Biome::LEAF || type == Biome::ICE || type == Biome::CACTUS || type == Biome::CLOUD;
}