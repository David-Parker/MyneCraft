#include "Chunk.h"


Chunk::Chunk(const std::string& name, int xStart, int yStart, Ogre::SceneManager* mSceneManager, Biome* biome, Perlin* perlin, Simulator* sim) : _name(name), _xStart(xStart), _yStart(yStart), _simulator(sim) {

	Ogre::Vector3 scale(CHUNK_SCALE, CHUNK_SCALE, CHUNK_SCALE);
	_scale = scale;

	_xEnd = xStart + CHUNK_SIZE;
	_yEnd = yStart + CHUNK_SIZE;

	float steepness = 100.0f;

	_sg = mSceneManager->createStaticGeometry(_name);

	if (grassCube == nullptr && snowCube == nullptr) {
		grassCube = mSceneManager->createEntity("Cube-Grass.mesh");
		snowCube = mSceneManager->createEntity("Cube-Snow.mesh");	
	}

	bool inBiome = biome->inBiome(_xStart, _yStart);

	for (int i = xStart; i < _xEnd; ++i) {
		for (int j = yStart; j < _yEnd; ++j) {
			float fi = (float)i / (float)100.0f;
			float fj = (float)j / (float)100.0f;

			int y = (int)((perlin->getPerlin(fi, fj)) * steepness);
			Ogre::Vector3 pos(i*scale.x * 2, y*scale.y * 2, j*scale.z * 2);
			StaticObject* so;

			if ( inBiome )
				so = new StaticObject(biome->getCubeEntity(i, j), scale, pos, sim);
			else if(y >= 15)
				so = new StaticObject(snowCube, scale, pos, sim);
			else
				so = new StaticObject(grassCube, scale, pos, sim);

			_staticObjects.push_back(so);

			_sg->addEntity(so->_geom, so->_pos, so->_orientation, so->_scale);
		}
	}
	_sg->build();
}

Chunk::~Chunk() {
	for(auto& var : _staticObjects) {
		delete var;
	}
	std::cout << "~Chunk()" << std::endl;
	_staticObjects.clear();
	_sg->destroy();
	delete _sg;
}

bool Chunk::pointInChunk(float x, float y) {
	int roundX = (int) x;
	int roundY = (int) y;

	return (roundX >= _xStart*_scale.x*2 && roundX <= _xEnd*_scale.x*2) && (roundY >= _yStart*_scale.y*2 && roundY <= _yEnd*_scale.y*2);
}

void Chunk::addChunksToSimulator() {
	for (auto& var : _staticObjects) {
		var->addToSimulator();
	}
}