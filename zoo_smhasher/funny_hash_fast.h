#ifndef FUNNY_HASH_H
#define FUNNY_HASH_H
/*
 * Copyright (C) 2014 Sokolov Yura aka funny_falcon
 *
 * It is in public domain, free to use and modify by everyone for any usage.
 * I will be glad, if you put copyright notice above in your code or distribution, but you don't ought to.
 * I'm not responsible for any damage cause by this piece of code.
 */

/* API */

/* mix 32bit integer into 64bit state */
static inline void fh32_permute(uint32_t v, uint32_t *a, uint32_t *b);
/* hash string, seed with and produce 64bit state */
static inline void fh32_permute_string(const uint8_t *v, size_t len, uint32_t *a, uint32_t *b);
/* state finalization to 32bit value */
static inline uint32_t fh32_finalize(uint32_t a, uint32_t b);
/* convenient function to hash buffer with 32bit seed */
static inline uint32_t fh32_string_hash(const void *buf, size_t len, uint32_t seed);
/* convenient function to hash buffer with 64bit seed */
static inline uint32_t fh32_string_hash2(const void *buf, size_t len, uint32_t seed1, uint32_t seed2);

/* mix 64bit integer into 128bit state */
static inline void fh64_permute(uint64_t v, uint64_t *a, uint64_t *b);
/* hash string, seed with and produce 128bit state */
static inline void fh64_permute_string(const uint8_t *v, size_t len, uint64_t *a, uint64_t *b);
/* state finalization to 64bit value */
static inline uint64_t  fh64_finalize(uint64_t a, uint64_t b);
/* convenient function to hash buffer with 64bit seed */
static inline uint64_t  fh64_string_hash(const void *buf, size_t len, uint64_t seed);
/* convenient function to hash buffer with 128bit seed */
static inline uint64_t  fh64_string_hash2(const void *buf, size_t len, uint64_t seed1, uint64_t seed2);

/* IMPLEMENTATION */

#ifndef FH_READ_UNALIGNED
#define FH_READ_UNALIGNED 1
#endif
#define FH_ROTL(x, n) (((x) << (n)) | ((x) >> (sizeof(x)*8 - (n))))
#ifndef U64_CONSTANT
#define U64_CONSTANT(x) (x##ull)
#endif

static const uint32_t FH_C1 = 0xb8b34b2d;
static const uint32_t FH_C2 = 0x52c6a2d9;
static const uint32_t FH_C3 = 0xcc9e2d51;
static const uint32_t FH_C4 = 0xc6a4a793;
#define C1 9
#define C2 7
#define C3 9
/* this function is cause of clang:
 *     on x86_64 it tries to pack fh_u64_t in one 64bit register and work on it.
 * gcc does no such mistake */
static inline void
fh32_permute(uint32_t v, uint32_t *a, uint32_t *b)
{
	*a = FH_ROTL(*a+v, 24) + *b;
	*b = FH_ROTL(*b+v, 7) ^ (*a*9);
}
static inline void
fh32_permute2(uint32_t v1, uint32_t v2, uint32_t *a, uint32_t *b)
{
	*a += v2;
	*a = FH_ROTL(*a+v1, 24) + *b;
	*b = FH_ROTL(*b+v1, 7) ^ (*a*9);
	*b -= FH_ROTL(v2, 11);
}
static inline void
fh32_permute3(uint32_t v1, uint32_t v2, uint32_t *a, uint32_t *b, uint32_t *c)
{
	*a += v2;
	*a = FH_ROTL(*a+v1, 24) + *b;
	*b = FH_ROTL(*b+v1, 7) ^ *a;
	*c = FH_ROTL(*c + *b, 3) - *a;
	*b -= FH_ROTL(v2, 11);
}
static inline void
fh32_permute_final(uint32_t *a, uint32_t *b)
{
	*a = (FH_ROTL(*a, 24) + *b) * 9;
	*b = FH_ROTL(*b*5, 5) ^ *a;
}

#if FH_READ_UNALIGNED
#define fh_load_u32(v) (*(uint32_t*)(v))
#else
static inline uint32_t
fh_load_u32(const uint8_t *v)
{
	return v[0]|(v[1]<<8)|(v[2]<<16)|(v[3]<<24);
}
#endif

static inline uint32_t
fh_load_u24(const uint8_t *v, unsigned len)
{
	uint32_t x = 0;
	switch(len) {
	case 3: x = v[2] << 16;
	case 2: x |= v[1] << 8;
	case 1: x |= v[0];
	}
	return x;
}


static inline void
fh32_permute_string(const uint8_t *v, size_t len, uint32_t *a, uint32_t *b)
{
	uint32_t t, t1;
	if (len < 8) {
		t = fh_load_u24(v+(len&4), len&3);
		*b ^= t;
	} else if (len < 16) {
		t = fh_load_u32(v);
		t1 = fh_load_u32(v+4);
		fh32_permute2(t, t1, a, b);
		t = fh_load_u32(v+len-4);
		v += 8;
	} else {
		size_t n = len / 8;
		uint32_t c = 0;
		for(; n; n--, v+=8) {
			t = fh_load_u32(v);
			t1 = fh_load_u32(v+4);
			fh32_permute3(t, t1, a, b, &c);
		}
		*b ^= FH_ROTL(c, 3);
		t = fh_load_u32(v+(len&7)-4);
	}
	if (len & 4) {
		fh32_permute(fh_load_u32(v), a, b);
	}
	(*b) += len;
	fh32_permute(t, a, b);
}


static inline uint32_t
fh32_finalize(uint32_t a, uint32_t b)
{
	fh32_permute_final(&a, &b);
	fh32_permute_final(&a, &b);
	//return (FH_ROTL(a*5, 13) + b);
	return (FH_ROTL(a*5 + (a<<12), 13) + b);
}

static inline uint32_t
fh32_string_hash(const void* d, size_t len, uint32_t seed)
{
	//uint32_t a = seed ^ (2 << 16), b = seed;
	//uint32_t a = seed, b = 1;
	//uint32_t a = 0xdeadbeef, b = seed;
	uint32_t a = 0, b = seed;
	fh32_permute_string((const uint8_t*)d, len, &a, &b);
	return fh32_finalize(a, b);
}

static inline uint32_t
fh32_string_hash2(const void* d, size_t len, uint32_t seed1, uint32_t seed2)
{
	uint32_t a = seed1, b = seed2;
	fh32_permute_string((const uint8_t*)d, len, &a, &b);
	return fh32_finalize(a, b);
}

static const uint64_t FH_BC1 = U64_CONSTANT(0xacd5ad43274593b9);
static const uint64_t FH_BC2 = U64_CONSTANT(0x6956abd6ed268a3d);
/* it looks like clang is not as good at instruction reordering as gcc */
static inline void
fh64_permute(uint64_t v, uint64_t *a, uint64_t *b)
{
	*a = FH_ROTL(*a + v, 56) + *b;
	*b = FH_ROTL(*b + v, 15) ^ (*a * 9);
}

static inline void
fh64_permute2(uint64_t v1, uint64_t v2, uint64_t *a, uint64_t *b)
{
	*a += v2;
	*a = FH_ROTL(*a + v1, 56) + *b;
	*b = FH_ROTL(*b + v1, 15) ^ (*a * 9);
	*b -= FH_ROTL(v2, 5);
}

static inline void
fh64_permute3(uint64_t v1, uint64_t v2, uint64_t *a, uint64_t *b, uint64_t *c)
{
	*a += v2;
	*a = FH_ROTL(*a + v1, 56) + *b;
	*b = FH_ROTL(*b + v1, 15) ^ *a;
	*c = FH_ROTL(*c + *b, 7) - *a;
	*b -= FH_ROTL(v2, 5);
}

static inline void
fh64_permute_final(uint64_t *a, uint64_t *b)
{
	*a = (FH_ROTL(*a, 48) + *b) * 9;
	*b = FH_ROTL(*b*5, 17) ^ *a;
}

#if FH_READ_UNALIGNED
#define fh_load_u64(v) (*(uint64_t*)(v))
#else
static inline uint64_t
fh_load_u64(const uint8_t *v)
{
	return (uint64_t)fh_load_u32(v) | ((uint64_t)fh_load_u32(v+4) << 32);
}
#endif

static inline uint64_t
fh_load_u56(const uint8_t *v, unsigned len)
{
	uint64_t x;
	if (len <= 3) {
		x = fh_load_u24(v, len & 3);
	} else {
		x = fh_load_u32(v);
		x |= (uint64_t)fh_load_u24(v+4, len&3) << 32;
	}
	return x;
}

static inline void
fh64_permute_string(const uint8_t *v, size_t len, uint64_t *a, uint64_t *b)
{
	uint64_t t, t1;
	if (len < 8) {
		t = fh_load_u56(v+(len&8), len&7);
	} else if (len < 16) {
		t = fh_load_u64(v+(len&7));
	} else if (len < 32) {
		t = fh_load_u64(v);
		t1 = fh_load_u64(v+8);
		fh64_permute2(t, t1, a, b);
		t = fh_load_u64(v + len-8);
		v += 16;
	} else {
		size_t n = len / 16;
		uint64_t c = 0;
		for(; n; n--, v+=16) {
			t = fh_load_u64(v);
			t1 = fh_load_u64(v+8);
			fh64_permute3(t, t1, a, b, &c);
		}
		*b ^= FH_ROTL(c, 3);
		t = fh_load_u64(v + (len&15) - 8);
	}
	*b += FH_ROTL(t, 37);
	if (len & 8) {
		fh64_permute(fh_load_u64(v), a, b);
	}
	(*b) += (uint64_t)len;
	fh64_permute(t, a, b);
}

static inline uint64_t
fh64_finalize(uint64_t a, uint64_t b)
{
	fh64_permute_final(&a, &b);
	fh64_permute_final(&a, &b);
	fh64_permute_final(&a, &b);
	return b;
	//return FH_ROTL(a*5+(a<<17), 29) + b;
}

static inline uint64_t
fh64_string_hash(const void* d, size_t len, uint64_t seed)
{
	uint64_t a = 0, b = seed;
	fh64_permute_string((const uint8_t*)d, len, &a, &b);
	return fh64_finalize(a, b);
}

static inline uint64_t
fh64_string_hash2(const void* d, size_t len, uint64_t seed1, uint64_t seed2)
{
	uint64_t a = seed1, b = seed2;
	fh64_permute_string((const uint8_t*)d, len, &a, &b);
	return fh64_finalize(a, b);
}

#undef FH_ROTL
#undef FH_READ_UNALIGNED
#endif
