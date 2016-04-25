#pragma once

namespace Rand {
	static unsigned long next = 1;

	/* RAND_MAX assumed to be 32767 */
	inline int rand(void) {
	    next = next * 1103515245 + 12345;
	    return((unsigned)(next/65536) % 32768);
	}

	inline void srand(unsigned seed) {
	    next = seed;
	    std::cout << "Seed: " << seed << std::endl;
	}
}