#pragma once
#include <random>
#include <vector>
#include <math.h>
#include <utility>
#include <unordered_map>

#include <Ogre.h>

#include "Biome.h"
#include "CubeManager.h"
#include "StaticObject.h"
#include "Rand.h"

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

static int biomeGridSize = 1500;
static int positionVariance = 100;
static int minBiomeRadius = 600;
static int biomeRadiusVariance = 100;

class BiomeManager {
public:
	BiomeManager(Ogre::SceneManager*);
	~BiomeManager(void);

	Biome* inBiome(int, int);
	Ogre::Entity* getTerrain(CubeManager::CubeType);

	bool isTreeType(CubeManager::CubeType);

protected:
	Biome* createBiome(CubeManager::CubeType, int, int, int);
	std::string getBiomeName(int, int);
	std::unordered_map<std::pair<int, int>, Biome*> biomeGrid;
	std::vector<Biome*> worldBiomes;
	Ogre::SceneManager* mSceneManager;
};