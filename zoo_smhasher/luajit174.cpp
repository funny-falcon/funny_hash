#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include "luajit174.h"

void luajit174_32 ( const void * key, int len, uint32_t seed, void * out )
{
	*(uint32_t*)out = lj32_string_hash(key, len, seed);
}
