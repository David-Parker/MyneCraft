#pragma once
#include <Ogre.h>

#include <vector>

#include "StaticObject.h"
#include "Perlin.h"

static Ogre::Entity* grassCube = nullptr;
static Ogre::Entity* snowCube = nullptr;

#define CHUNK_SIZE 64
class Chunk {
private:
	Ogre::StaticGeometry *sg;
	std::vector<StaticObject *> staticObjects;
	int _xStart;
	int _xEnd;
	int _yStart;
	int _yEnd;

public:
	Chunk(int, int, int, int, Ogre::SceneManager*, Perlin*, Simulator*);
};