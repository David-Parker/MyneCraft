#include "Perlin.h"

/* PsuedoCode taken from https://en.wikipedia.org/wiki/Perlin_noise */

int p[256] = { 151, 160, 137, 91, 90, 15,
131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180 };

int grad3[12][3] = { { 1, 1, 0 }, { -1, 1, 0 }, { 1, -1, 0 }, { -1, -1, 0 },
{ 1, 0, 1 }, { -1, 0, 1 }, { 1, 0, -1 }, { -1, 0, -1 },
{ 0, 1, 1 }, { 0, -1, 1 }, { 0, 1, -1 }, { 0, -1, -1 } };

inline float dot(int g[], float x, float y) {
	return g[0] * x + g[1] * y;
}

Perlin::Perlin(int xMx, int yMx, int rndMx, float dnsty) : xMax(xMx), yMax(yMx), randomMax(rndMx), density(dnsty), size(xMx*yMx) {
	// What's going on here
	size = xMax * yMax;
	std::vector<std::vector<std::vector<float>>> tmp(xMax, std::vector<std::vector<float>>(yMax, std::vector<float>(2,0)));
	gradient = tmp;
	density = (density > 0.0f ? density : 0.1f);
	generateGradientTrivial();
}

void Perlin::generateGradientTrivial() {
	static float moTWO = randomMax/2;
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
 
    return value / density;
}

float Perlin::getSimplex(float x, float y) {

	static int perm[512];
	static bool init = false;

	if (!init) {
		for (int i = 0; i < 512; i++) {
			perm[i] = p[i & 255];
		}
		init = true;
	}

	// Noise contributions from the three corners
	float n0, n1, n2;

	// Skew the input space to determine which simplex cell we're in
	const float F2 = 0.5*(std::sqrt(3.0) - 1.0);
	float s = (x + y)*F2;
	int i = std::floor(x + s);
	int j = std::floor(y + s);

	const float G2 = (3.0 - std::sqrt(3.0)) / 6.0;
	float t = (i + j)*G2;
	float X0 = i - t;
	float Y0 = j - t;
	float x0 = x - X0;
	float y0 = y - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle
	// Determine which simplex we are in
	int i1, j1;
	if (x0 > y0) { i1 = 1; j1 = 0; }
	else { i1 = 0; j1 = 1; }

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0 + 2.0 * G2;

	// Work out the hashed gradient indices of the three simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = perm[ii + perm[jj]] % 12;
	int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
	int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;

	float t0 = 0.5 - x0*x0 - y0*y0;
	if (t0<0) n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
	}

	float t1 = 0.5 - x1*x1 - y1*y1;
	if (t1<0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
	}
	float t2 = 0.5 - x2*x2 - y2*y2;
	if (t2<0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
	}
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 70.0 * (n0 + n1 + n2);
}