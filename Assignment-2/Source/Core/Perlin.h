#pragma once
#include <iostream>
#include <random>
#include <vector>

/* PsedoCode taken from https://en.wikipedia.org/wiki/Perlin_noise */

class Perlin {
private:
	int size;
	int randomMax;
	int correction;
	std::vector<std::vector<std::vector<float>>> gradient;
	
public:
	Perlin(int, int, int, int);
	void generateGradientTrivial();
	float lerp(float, float, float);
	float dotGridGradient(int, int, float, float);
	float getPerlin(float, float);
	float getSimplex(float, float);
	int xMax;
	int yMax;
};