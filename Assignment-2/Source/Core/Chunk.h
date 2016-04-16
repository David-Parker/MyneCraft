#pragma once
#include <Ogre.h>

#include <vector>
#include <utility>
#include <unordered_map>

#include "StaticObject.h"
#include "BiomeManager.h"
#include "Perlin.h"

#define CHUNK_SIZE 16
#define CHUNK_SCALE 50
#define CHUNK_SCALE_FULL 100
class Chunk {
private:
	typedef std::pair<int, int> key;

	Ogre::StaticGeometry* _sg;
	std::unordered_map<key, StaticObject*> _staticObjects;
	std::string _name;
	Simulator* _simulator;
	Ogre::SceneManager* _mSceneManager;
	Biome* _biome;
	BiomeManager* _biomeMgr;

	void createTree(const Ogre::Vector3&, Biome::BiomeType);
	key getKey(int x, int y, int z);
	key getKey(Ogre::Vector3& pos);

public:
	Chunk(int, int, Ogre::SceneManager*, BiomeManager*, Perlin*, Simulator*);
	~Chunk();
	bool pointInChunk(float x, float y);
	void addChunksToSimulator();
	StaticObject* getBlock(float x, float z);
	void removeBlock(StaticObject* obj);
	std::string getName() { return _name; }
	void generateNeighborPointers();
	Ogre::Vector3 _scale;
	int _xStart = 0;
	int _xEnd = 0;
	int _yStart = 0;
	int _yEnd = 0;
	bool modified = false;
};

inline static std::string getChunkName(int x, int z) {
	char buf[64];
	sprintf(buf, "Chunk_%d_%d", x / CHUNK_SIZE, z / CHUNK_SIZE);
	return std::string(buf);
}