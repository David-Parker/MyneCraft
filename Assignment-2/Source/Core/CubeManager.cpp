#include "CubeManager.h"

CubeManager* CubeManager::singleton = nullptr;
Ogre::SceneManager* CubeManager::mSceneManager = nullptr;

CubeManager::CubeManager ( ) {
	assert(mSceneManager != nullptr);
}

void CubeManager::injectSceneManager ( Ogre::SceneManager* scnMgr ) {
	mSceneManager = scnMgr;
	singleton = new CubeManager();

	singleton->cactusCube = mSceneManager->createEntity("Cube-Cactus.mesh");
	singleton->cloudCube = mSceneManager->createEntity("Cube-Cloud.mesh");
	singleton->dirtCube = mSceneManager->createEntity("Cube-Dirt.mesh");
	singleton->iceCube = mSceneManager->createEntity("Cube-Ice.mesh");
	singleton->glassCube = mSceneManager->createEntity("Cube-Glass.mesh");
	singleton->grassCube = mSceneManager->createEntity("Cube-Grass.mesh");
	singleton->leafCube = mSceneManager->createEntity("Cube-Leaves.mesh");
	singleton->plankCube = mSceneManager->createEntity("Cube-Plank.mesh");
	singleton->rockCube = mSceneManager->createEntity("Cube-Rock.mesh");
	singleton->sandCube = mSceneManager->createEntity("Cube-Sand.mesh");
	singleton->snowCube = mSceneManager->createEntity("Cube-Snow.mesh");
	singleton->torchCube = mSceneManager->createEntity("Cube-Torch.mesh");
	singleton->woodCube = mSceneManager->createEntity("Cube-Wood.mesh");
	singleton->stoneCube = mSceneManager->createEntity("Cube-Stone.mesh");
	singleton->waterCube = mSceneManager->createEntity("Cube-Water.mesh");
}

CubeManager* CubeManager::getSingleton ( ) {
	assert(singleton != nullptr);
	return singleton;
}

Ogre::Entity* CubeManager::getEntity(CubeType type) {
	switch ( type ) {
		case CACTUS: return singleton->cactusCube;
		case CLOUD: return singleton->cloudCube;
		case DIRT: return singleton->dirtCube;
		case ICE: return singleton->iceCube;
		case GLASS: return singleton->glassCube;
		case GRASS: return singleton->grassCube;
		case LEAF: return singleton->leafCube;
		case PLANK: return singleton->plankCube;
		case ROCK: return singleton->rockCube;
		case SAND: return singleton->sandCube;
		case SNOW: return singleton->snowCube;
		case TORCH: return singleton->torchCube;
		case WOOD: return singleton->woodCube;
		case STONE: return singleton->stoneCube;
		case WATER: return singleton->waterCube;
		default: return nullptr;
	}
}