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

static StaticObject* air = nullptr;

class Chunk {
private:
	typedef std::string key;

	Ogre::StaticGeometry* _sg;
	std::string _name;
	Simulator* _simulator;
	Ogre::SceneManager* _mSceneManager;
	Biome* _biome;
	BiomeManager* _biomeMgr;

	std::unordered_map<key, Ogre::Light*> lights;

	bool createTree(const Ogre::Vector3&, Biome::BiomeType);
	key getKey(int x, int y, int z);
	key getKey(const Ogre::Vector3& pos);

	StaticObject* getObjFromChunks(const std::vector<Chunk*>& chunks, key index);
	void createAirNeighbors(const std::vector<Chunk*>& chunks, StaticObject* obj);

public:
	Chunk(int, int, Ogre::SceneManager*, BiomeManager*, Perlin*, Simulator*);
	~Chunk();
	bool pointInChunk(float x, float y);
	void addChunksToSimulator();
	StaticObject* getBlock(int x, int y, int z);
	Biome::BiomeType getGeneratedType(Biome::BiomeType objType, int height);
	void removeBlock(const std::vector<Chunk*>& chunks, StaticObject* obj);
	void addBlock(const std::vector<Chunk*>& chunks, StaticObject* obj, const btVector3& hitnormal, Biome::BiomeType newType);
	std::string getName() { return _name; }

	std::unordered_map<key, StaticObject*> _staticObjects;

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