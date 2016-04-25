#pragma once
#include <iostream>
#include "Rand.h"
#include <vector>

/* PsedoCode taken from https://en.wikipedia.org/wiki/Perlin_noise */

class Perlin {
private:
	int size;
	int randomMax;
	float density; /* Density domain is (0, 10+). 0 is unpopulated 10 is extremely dense. */
	std::vector<std::vector<std::vector<float>>> gradient;

public:
	Perlin(int, int, int, float);
	void generateGradientTrivial();
	float lerp(float, float, float);
	float dotGridGradient(int, int, float, float);
	float getPerlin(float, float);
	int xMax;
	int yMax;
};