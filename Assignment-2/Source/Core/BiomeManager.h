#pragma once
#include <random>
#include <vector>
#include <math.h>
#include <utility>
#include <unordered_map>

#include <Ogre.h>

#include "Biome.h"
#include "StaticObject.h"

// TODO Put in its own file
template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
  template<typename S, typename T> struct hash<pair<S, T>>
  {
    inline size_t operator()(const pair<S, T> & v) const
    {
      size_t seed = 0;
      ::hash_combine(seed, v.first);
      ::hash_combine(seed, v.second);
      return seed;
    }
  };
}


static Ogre::Entity* grassTree = nullptr;
static Ogre::Entity* snowTree = nullptr;
static Ogre::Entity* sandTree = nullptr;

static int biomeGridSize = 1600;
static int positionVariance = 200;
static int minBiomeRadius = 500;
static int biomeRadiusVariance = 200;

class BiomeManager {
public:
	BiomeManager(Ogre::SceneManager*);
	~BiomeManager(void);

	Biome* inBiome(int, int);
	Ogre::Entity* getTerrain(Biome::BiomeType);
	Ogre::Entity* getTreeEntity(Biome::BiomeType);

protected:
	Biome* createBiome(Biome::BiomeType, int, int, int);
	std::string getBiomeName(int, int);
	std::unordered_map<std::pair<int, int>, Biome*> biomeGrid;
	std::vector<Biome*> worldBiomes;
	Ogre::SceneManager* mSceneManager;
	Ogre::Entity* grassMesh;
	Ogre::Entity* snowMesh;
	Ogre::Entity* sandMesh;
};