#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

int main(int argc, char** argv) {
	static const uint64_t C = ((uint64_t)0xacd5ad43 << 32) | 0x274593b9;
	uint64_t var = 1, v;
	size_t size = atoi(argv[1]);

	while (size >= 8) {
		var = var * C + 1;
		v = var ^ (var >> 32);
		fwrite(&v, 8, 1, stdout);
		size -= 8;
	}
}
