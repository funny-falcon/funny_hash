#ifndef LUAJIT174_H
#define LUAJIT174_H
#include <stdlib.h>

#define lj_getu32(str) (*(uint32_t*)(str))
#define lj_rol(v,r) (((v)<<(r))|((v)>>(32-(r))))


static inline uint32_t
lj32_string_hash(const void* d, size_t len, uint32_t seed)
{
	const char *str = (const char*)d;
	uint32_t h = len ^ seed;
	uint32_t a = 0,b = 0;
	if (len <= 12) {
	if (len >= 4) {  /* Caveat: unaligned access! */
		a = lj_getu32(str);
		h ^= lj_getu32(str+len-4);
		b = lj_getu32(str+(len>>1)-2);
		h ^= b; h -= lj_rol(b, 14);
		b += lj_getu32(str+(len>>2)-1);
	} else if (len > 0) {
		a = *(const uint8_t *)str;
		h ^= *(const uint8_t *)(str+len-1);
		b = *(const uint8_t *)(str+(len>>1));
		h ^= b; h -= lj_rol(b, 14);
	}
	a ^= h; a -= lj_rol(h, 11);
	b ^= a; b -= lj_rol(a, 25);
	h ^= b; h -= lj_rol(b, 16);
	//a ^= h; a -= lj_rol(h, 4);
	//b ^= a; b -= lj_rol(a, 14);
	//h ^= b; h -= lj_rol(b, 24);
	} else {
		uint32_t i = (len-1)/8;
		uint32_t h2 = seed ^ len;
		a = lj_getu32(str + len - 4);
		b = lj_getu32(str + len - 8);
		for (; i; i--, str+=8) {
			h = lj_rol(h ^ a, 20) + (b ^ 0xdeadbeef);
			h2 = lj_rol(h2 ^ b, 23) + a;
			a = lj_rol(a, 13); a -= lj_getu32(str);
			b = lj_rol(a, 11); b -= lj_getu32(str+4);
		}
		h = lj_rol(h ^ a, 20) + (b ^ 0xdeadbeef);
		h2 = lj_rol(h2 ^ b, 23) + a;
		a ^= h; a -= lj_rol(h2, 11);
		b ^= a;  b -= lj_rol(a, 25);
		h2 -= lj_rol(a, 7);
		h ^= b; h -= lj_rol(b ^ h2, 16);
		a ^= h; a -= lj_rol(h, 4);
		b ^= a;  b -= lj_rol(a, 14);
		h ^= b; h -= lj_rol(b ^ h2, 24);
	}
	return h;
}
#endif
