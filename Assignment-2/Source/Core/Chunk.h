#pragma once
#include <Ogre.h>

#include <vector>

#include "StaticObject.h"
#include "Perlin.h"

static Ogre::Entity* grassCube = nullptr;
static Ogre::Entity* snowCube = nullptr;

#define CHUNK_SIZE 8
class Chunk {
private:
	Ogre::StaticGeometry* _sg;
	std::vector<StaticObject *> staticObjects;

public:
	Chunk(int, int, Ogre::SceneManager*, Perlin*, Simulator*);
	bool pointInChunk(float x, float y);
	Ogre::Vector3 _scale;
	int _xStart = 0;
	int _xEnd = 0;
	int _yStart = 0;
	int _yEnd = 0;
};