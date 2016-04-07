#include "Perlin.h"

Perlin::Perlin() {
	repeat = -1;
	std::vector<int> tmp(std::vector<int>(512));
	p = tmp;
	for(int x=0;x<512;x++) {
		p[x] = permutation[x%256];
	}
}

float Perlin::OctavePerlin(float x, float y, float z, int octaves, float persistence) {
	float total = 0;
	float frequency = 1;
	float amplitude = 1;
	float maxValue = 0;			// Used for normalizing result to 0.0 - 1.0
	for(int i=0;i<octaves;i++) {
		total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;
		
		maxValue += amplitude;
		
		amplitude *= persistence;
		frequency *= 2;
	}
	
	return total/maxValue;
}


float Perlin::perlin(float x, float y, float z) {
	if(repeat > 0) {									// If we have any repeat on, change the coordinates to their "local" repetitions
		x = (int)x%repeat;
		y = (int)y%repeat;
		z = (int)z%repeat;
	}
	
	int xi = (int)x & 255;								// Calculate the "unit cube" that the point asked will be located in
	int yi = (int)y & 255;								// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
	int zi = (int)z & 255;								// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
	float xf = x-(int)x;								// We also fade the location to smooth the result.
	float yf = y-(int)y;
	float zf = z-(int)z;
	float u = fade(xf);
	float v = fade(yf);
	float w = fade(zf);
														
	int aaa, aba, aab, abb, baa, bba, bab, bbb;
	aaa = p[p[p[    xi ]+    yi ]+    zi ];
	aba = p[p[p[    xi ]+inc(yi)]+    zi ];
	aab = p[p[p[    xi ]+    yi ]+inc(zi)];
	abb = p[p[p[    xi ]+inc(yi)]+inc(zi)];
	baa = p[p[p[inc(xi)]+    yi ]+    zi ];
	bba = p[p[p[inc(xi)]+inc(yi)]+    zi ];
	bab = p[p[p[inc(xi)]+    yi ]+inc(zi)];
	bbb = p[p[p[inc(xi)]+inc(yi)]+inc(zi)];

	float x1, x2, y1, y2;
	x1 = lerp(	grad (aaa, xf  , yf  , zf),				// The gradient function calculates the dot product between a pseudorandom
				grad (baa, xf-1, yf  , zf),				// gradient vector and the vector from the input coordinate to the 8
				u);										// surrounding points in its unit cube.
	x2 = lerp(	grad (aba, xf  , yf-1, zf),				// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
				grad (bba, xf-1, yf-1, zf),				// values we made earlier.
		          u);
	y1 = lerp(x1, x2, v);

	x1 = lerp(	grad (aab, xf  , yf  , zf-1),
				grad (bab, xf-1, yf  , zf-1),
				u);
	x2 = lerp(	grad (abb, xf  , yf-1, zf-1),
	          	grad (bbb, xf-1, yf-1, zf-1),
	          	u);
	y2 = lerp (x1, x2, v);
	
	return lerp (y1, y2, w);
}

int Perlin::inc(int num) {
	num++;
	if (repeat > 0) num %= repeat;
	
	return num;
}

float Perlin::grad(int hash, float x, float y, float z) {
	int h = hash & 15;									// Take the hashed value and take the first 4 bits of it (15 == 0b1111)
	float u = h < 8 /* 0b1000 */ ? x : y;				// If the most significant bit (MSB) of the hash is 0 then set u = x.  Otherwise y.
	
	float v;											// In Ken Perlin's original implementation this was another conditional operator (?:).  I
														// expanded it for readability.
	
	if(h < 4 /* 0b0100 */)								// If the first and second significant bits are 0 set v = y
		v = y;
	else if(h == 12 /* 0b1100 */ || h == 14 /* 0b1110*/)// If the first and second significant bits are 1 set v = x
		v = x;
	else 												// If the first and second significant bits are not equal (0/1, 1/0) set v = z
		v = z;
	
	return ((h&1) == 0 ? u : -u)+((h&2) == 0 ? v : -v); // Use the last 2 bits to decide if u and v are positive or negative.  Then return their addition.
}

float Perlin::fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float Perlin::lerp(float a, float b, float x) {
	return a + x * (b - a);
}