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

static inline void
fh32_permute(uint32_t v, uint32_t *a, uint32_t *b)
{
	/* reference formula */
	*a = FH_ROTL(*a ^ v, 16) * FH_C1;
	*b = (FH_ROTL(*b, 16) ^ v) * FH_C2;
}

#if FH_READ_UNALIGNED
#define fh_load_u32(v) (*(uint32_t*)(v))
#else
static inline uint32_t
fh_load_u32(const uint8_t *v)
{
	/* note: this function could have other shape to be faster on ARM.
	 * for example, this shape is faster on Core-i7:
	uint32_t x = v[0];
	x += (v[1]<<8) | (v[2]<<16);
	x |= v[3]<<24;
	return x;
	 * but this function has no meaning on Core-i7, so that, i leave base shape.
	 * please, make a pull request if you find faster shape.*/
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
	uint32_t t;
	if (len <= 3) {
		t = fh_load_u24(v, len);
	} else {
		size_t n = len / 4;
		for(; n; n--, v+=4) {
			fh32_permute(fh_load_u32(v), a, b);
		}
		t = fh_load_u32(v-(4-(len&3)));
	}
	(*b) ^= FH_ROTL((uint32_t)len, 16);
	fh32_permute(t, a, b);
}


static inline uint32_t
fh32_finalize(uint32_t a, uint32_t b)
{
	a ^= a >> 17;
	b ^= b >> 16;
	a *= FH_C1;
	b *= FH_C2;
	return a ^ b ^ (a >> 16) ^ (b >> 17);
}

static inline uint32_t
fh32_string_hash(const void* d, size_t len, uint32_t seed)
{
	uint32_t a = seed ^ (2 << 16), b = seed;
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

static inline void
fh64_permute(uint64_t v, uint64_t *a, uint64_t *b)
{
	/* reference formula */
	*a = FH_ROTL(*a ^ v, 32) * FH_BC1;
	*b = (FH_ROTL(*b, 32) ^ v) * FH_BC2;
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
		x |= (uint64_t)(fh_load_u32(v + (len & 3))) << ((len & 3) * 8);
	}
	return x;
}

static inline void
fh64_permute_string(const uint8_t *v, size_t len, uint64_t *a, uint64_t *b)
{
	uint64_t t;
	if (len <= 7) {
		t = fh_load_u56(v, len & 7);
	} else {
		size_t n = len / 8;
		for(; n; n--, v+=8) {
			fh64_permute(fh_load_u64(v), a, b);
		}
		t = fh_load_u64(v - (8-(len&7)));
	}
	(*b) ^= (uint64_t)len << 32;
	fh64_permute(t, a, b);
}

static inline uint64_t
fh64_finalize(uint64_t a, uint64_t b)
{
	a ^= a >> 33;
	b ^= b >> 32;
	a *= FH_BC1;
	b *= FH_BC2;
	return a ^ b ^ (a >> 32) ^ (b >> 33);
}

static inline uint64_t
fh64_string_hash(const void* d, size_t len, uint64_t seed)
{
	uint64_t a = seed ^ ((uint64_t)2 << 32), b = seed;
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
