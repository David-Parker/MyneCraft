#pragma once
#include <Ogre.h>

#include <vector>

#include "StaticObject.h"
#include "Biome.h"
#include "Perlin.h"

#define CHUNK_SIZE 16
#define CHUNK_SCALE 50
#define CHUNK_SCALE_FULL 100
class Chunk {
private:
	Ogre::StaticGeometry* _sg;
	std::vector<StaticObject*> _staticObjects;
	std::string _name;
	Simulator* _simulator;
	Ogre::SceneManager* _mSceneManager;

public:
	Chunk(const std::string&, int, int, Ogre::SceneManager*, Biome*, Perlin*, Simulator*);
	~Chunk();
	bool pointInChunk(float x, float y);
	void addChunksToSimulator();
	std::string getName() { return _name; }
	Ogre::Vector3 _scale;
	int _xStart = 0;
	int _xEnd = 0;
	int _yStart = 0;
	int _yEnd = 0;
};