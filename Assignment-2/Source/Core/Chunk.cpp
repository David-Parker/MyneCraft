#include "Chunk.h"

Chunk::Chunk(int xStart, int xEnd, int yStart, int yEnd, Ogre::SceneManager* mSceneManager, Perlin* perlin, Simulator* sim) : _xStart(xStart), _xEnd(xEnd), _yStart(yStart), _yEnd(yEnd) {
	// create unique name
	char buf[32];
	sprintf(buf, "Chunk_%d_%d", xStart, yStart);
	std::string name(buf);
	
	sg = mSceneManager->createStaticGeometry(name);

	if (grassCube == nullptr & snowCube == nullptr) {
		grassCube = mSceneManager->createEntity("Cube-Grass.mesh");
		snowCube = mSceneManager->createEntity("Cube-Snow.mesh");	
	}

	for (int i = xStart; i < xEnd; ++i) {
		for (int j = yStart; j < yEnd; ++j) {
			float fi = (float)i / (float)100.0f;
			float fj = (float)j / (float)100.0f;

			Ogre::Vector3 scale = Ogre::Vector3(50, 50, 50);

			int y = (int)((perlin->getPerlin(fi, fj)) * 100);
			Ogre::Vector3 pos(i*scale.x * 2.01, y*scale.y * 2, j*scale.z * 2);

			StaticObject* so;

			if(y >= 15)
				so = new StaticObject(snowCube, scale, pos, sim);
			else
				so = new StaticObject(grassCube, scale, pos, sim);

			sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}
	}


	sg->build();
}

