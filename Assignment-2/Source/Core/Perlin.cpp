#include "Perlin.h"

/* PsuedoCode taken from https://en.wikipedia.org/wiki/Perlin_noise */

Perlin::Perlin(int xMx, int yMx, int rndMx, int crct) : xMax(xMx), yMax(yMx), randomMax(rndMx), correction(crct), size(xMx*yMx) {
	size = xMax * yMax;
	std::vector<std::vector<std::vector<float>>> tmp(xMax, std::vector<std::vector<float>>(yMax, std::vector<float>(2,0)));
	gradient = tmp;
	generateGradientTrivial();
}
void Perlin::generateGradientTrivial() {
	static int moTWO = randomMax/2;
	for ( int i = 0 ; i < xMax ; i++ ) {
		for ( int j = 0 ; j < yMax ; j++ ) {
			gradient[i][j][0] = float(rand()%randomMax)/moTWO; 
			gradient[i][j][1] = float(rand()%randomMax)/moTWO;
		}
	}
	
}
// Function to linearly interpolate between a0 and a1
// Weight w should be in the range [0.0, 1.0]
float Perlin::lerp(float a0, float a1, float w) {
	return (1.0 - w)*a0 + w*a1;
}

 // Computes the dot product of the distance and gradient vectors.
float Perlin::dotGridGradient(int ix, int iy, float x, float y) {
	// Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;

	// Compute the dot-product
	return (dx*gradient[iy][ix][0] + dy*gradient[iy][ix][1]);
}

//float getAvg() { return avg; }

// Compute Perlin noise at coordinates x, y
float Perlin::getPerlin(float x, float y) {
		
	// Determine grid cell coordinates
    int x0 = (x >= 0.0 ? (int)x : (int)x - 1);
    int x1 = x0 + 1;
    int y0 = (y >= 0.0 ? (int)y : (int)y - 1);
    int y1 = y0 + 1;

	// Determine interpolation weights
    float sx = x - (float)x0;
    float sy = y - (float)y0;

	// Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;
    n0 = dotGridGradient(x0, y0, x, y);
    n1 = dotGridGradient(x1, y0, x, y);
    ix0 = lerp(n0, n1, sx);
    n0 = dotGridGradient(x0, y1, x, y);
    n1 = dotGridGradient(x1, y1, x, y);
    ix1 = lerp(n0, n1, sx);
    value = lerp(ix0, ix1, sy);
 
     return value;
}