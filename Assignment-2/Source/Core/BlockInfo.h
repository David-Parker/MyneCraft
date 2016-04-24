#pragma once
#include "Biome.h"

struct BlockInfo {
	int x;
	int y;
	int z;
	Biome::BiomeType type;
	BlockInfo(int x1, int y1, int z1, Biome::BiomeType tp) : x(x1), y(y1), z(z1), type(tp) {}
};