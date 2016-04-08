#include "Chunk.h"

Chunk::Chunk(int xStart, int yStart, Ogre::SceneManager* mSceneManager, Perlin* perlin, Simulator* sim) : _xStart(xStart), _yStart(yStart) {
	// create unique name
	char buf[32];
	sprintf(buf, "Chunk_%d_%d", xStart, yStart);
	std::string name(buf);
	Ogre::Vector3 scale = Ogre::Vector3(50, 50, 50);
	_scale = scale;

	_xEnd = xStart + CHUNK_SIZE;
	_yEnd = yStart + CHUNK_SIZE;
	
	sg = mSceneManager->createStaticGeometry(name);

	if (grassCube == nullptr & snowCube == nullptr) {
		grassCube = mSceneManager->createEntity("Cube-Grass.mesh");
		snowCube = mSceneManager->createEntity("Cube-Snow.mesh");	
	}

	for (int i = xStart; i < _xEnd; ++i) {
		for (int j = yStart; j < _yEnd; ++j) {
			float fi = (float)i / (float)100.0f;
			float fj = (float)j / (float)100.0f;


			int y = (int)((perlin->getPerlin(fi, fj)) * 100);
			Ogre::Vector3 pos(i*scale.x * 2, y*scale.y * 2, j*scale.z * 2);

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

bool Chunk::pointInChunk(float x, float y) {
	int roundX = (int) x;
	int roundY = (int) y;

	return (roundX >= _xStart*_scale.x*2 && roundX <= _xEnd*_scale.x*2) && (roundY >= _yStart*_scale.y*2 && roundY <= _yEnd*_scale.y*2);
}

