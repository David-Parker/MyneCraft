#pragma once
#include <iostream>

class Rand {
private:
	Rand();
	static int next;

public:
	static int rand() {
		next = next * 1103515245 + 12345;
	    return((unsigned)(next/65536) % 32768);
	}

	static void srand(unsigned seed) {
		next = seed;
	}
};