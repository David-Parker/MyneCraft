#include "Biome.h"

Biome::Biome(Ogre::SceneManager* smgr, int x, int y, int r) : centerX(x), centerY(y), radius(r), mSceneManager(smgr) {

}


bool Biome::inBiome(int x, int y) {
	return (abs(x - centerX) < radius || abs(y - centerY) < radius);
}

Ogre::Entity* Biome::getCubeEntity(int x, int y) {
	int dx = centerX - abs(x);
	int dy = centerY - abs(y);
	int dSq = (dx * dx) + (dy * dy);
	float rSQoN = radius * radius / 9;
	int rnd = rand()%20;
	if ( dSq < 2 * rSQoN ) {
		return mSceneManager->createEntity("Cube-Snow.mesh");	
	}
	else if ( dSq < 4 * rSQoN && rnd < 17 ) {
		return mSceneManager->createEntity("Cube-Snow.mesh");	
	}
	else if ( dSq < 6 * rSQoN && rnd < 14 ) {
		return mSceneManager->createEntity("Cube-Snow.mesh");	
	}
	else if ( dSq < 7 * rSQoN && rnd < 9 ) {
		return mSceneManager->createEntity("Cube-Snow.mesh");	
	}
	else if ( dSq < 8 * rSQoN && rnd < 4 ) {
		return mSceneManager->createEntity("Cube-Snow.mesh");	
	}
	else if ( rnd < 1 ) {
		return mSceneManager->createEntity("Cube-Snow.mesh");	
	}
	else {
		return mSceneManager->createEntity("Cube-Grass.mesh");	
	}	
}