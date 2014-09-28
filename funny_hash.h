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

/* 64bit state for 32bit result */
typedef struct fh_u64_s {
	uint32_t a, b;
} fh_u64_t;
/* mix 32bit integer into 64bit state */
static inline fh_u64_t fh32_permute(fh_u64_t h, uint32_t v);
/* hash string, seed with and produce 64bit state */
static inline fh_u64_t fh32_permute_string(fh_u64_t h, const uint8_t *v, size_t len);
/* state finalization to 32bit value */
static inline uint32_t fh32_finalize(fh_u64_t h);
/* convinient function to hash buffer with 32bit seed */
static inline uint32_t fh32_string_hash(const void *buf, size_t len, uint32_t seed);
/* convinient function to hash buffer with 64bit seed */
static inline uint32_t fh32_string_hash2(const void *buf, size_t len, uint32_t seed1, uint32_t seed2);

/* 128bit state for 64bit result */
typedef struct fh_u128_s {
	uint64_t a, b;
} fh_u128_t;
/* mix 64bit integer into 128bit state */
static inline fh_u128_t fh64_permute(fh_u128_t h, uint64_t v);
/* hash string, seed with and produce 128bit state */
static inline fh_u128_t fh64_permute_string(fh_u128_t h, const uint8_t *v, size_t len);
/* state finalization to 64bit value */
static inline uint64_t  fh64_finalize(fh_u128_t h);
/* convinient function to hash buffer with 64bit seed */
static inline uint64_t  fh64_string_hash(const void *buf, size_t len, uint64_t seed);
/* convinient function to hash buffer with 128bit seed */
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
static const uint32_t FH_C2 = 0x52c6a5d9;
/* this function is cause of clang:
 *     on x86_64 it tries to pack fh_u64_t in one 64bit register and work on it.
 * gcc does no such mistake */
static inline void
fh32_permute_imp(uint32_t *a, uint32_t *b, uint32_t v)
{
	*a ^= v;
	*b = FH_ROTL(*b, 16);
	*a = FH_ROTL(*a, 16) * FH_C1;
	*b = (*b ^ v) * FH_C2;
}

static inline fh_u64_t
fh32_permute(fh_u64_t h, uint32_t v)
{
	h.a = FH_ROTL(h.a ^ v, 16) * FH_C1;
	h.b = (FH_ROTL(h.b, 16) ^ v) * FH_C2;
	return h;
}

static inline uint32_t
fh_load_u32(const uint8_t *v, unsigned len)
{
	uint32_t x = 0;
	switch(len) {
#if !FH_READ_UNALIGNED
	case 4: x |= v[3] << 24;
#endif
	case 3: x |= v[2] << 16;
	case 2: x |= v[1] << 8;
	case 1: x |= v[0];
	case 0:
		return x;
	}
	return 0;
}

static inline fh_u64_t
fh32_permute_string(fh_u64_t h, const uint8_t *v, size_t len)
{
	uint32_t t;
	size_t n = len / 4;
	for(; n; n--, v+=4) {
#if FH_READ_UNALIGNED
		/* clang did something really strange on x86_64 if it were written as
		h = fh32_permute(h, *(uint32_t*)v);
		*/
		fh32_permute_imp(&h.a, &h.b, *(uint32_t*)v);
#else
		fh32_permute_imp(&h.a, &h.b, fh_load_u32(v, 4));
#endif
	}
	t = (uint32_t)len << 24;
	t |= fh_load_u32(v, len & 3);
	h.a--;
	fh32_permute_imp(&h.a, &h.b, t);
	return h;
}


static inline uint32_t
fh32_finalize(fh_u64_t h)
{
	fh_u64_t t;
	t.a = h.a ^ ((h.a >> 16) + (h.b >> 19));
	t.b = h.b + ((h.a >> 19) ^ (h.b >> 16));
	t.a *= FH_C1;
	t.b *= FH_C2;
	return t.a ^ t.b ^ (t.a >> 16) ^ (t.b >> 17);
}

static inline uint32_t
fh32_string_hash(const void* d, size_t len, uint32_t seed)
{
	fh_u64_t v = {seed, seed ^ len};
	v = fh32_permute_string(v, (const uint8_t*)d, len);
	return fh32_finalize(v);
}

static inline uint32_t
fh32_string_hash2(const void* d, size_t len, uint32_t seed1, uint32_t seed2)
{
	fh_u64_t v = {seed1, seed2 ^ len};
	v = fh32_permute_string(v, (const uint8_t*)d, len);
	return fh32_finalize(v);
}

static const uint64_t FH_BC1 = U64_CONSTANT(0xacd5ad43274593b9);
static const uint64_t FH_BC2 = U64_CONSTANT(0x6956abd6ed558e3d);
/* it looks like clang is not as good at instruction reordering as gcc */
static inline void
fh64_permute_imp(uint64_t *a, uint64_t *b, uint64_t v)
{
	*a ^= v;
	*b = FH_ROTL(*b, 32);
	*a = FH_ROTL(*a, 32) * FH_BC1;
	*b = (*b ^ v) * FH_BC2;
}

static inline fh_u128_t
fh64_permute(fh_u128_t h, uint64_t v)
{
	h.a = FH_ROTL(h.a ^ v, 32) * FH_BC1;
	h.b = (FH_ROTL(h.b, 32) ^ v) * FH_BC2;
	return h;
}

static inline uint64_t
fh_load_u64(const uint8_t *v, unsigned len)
{
	uint64_t x = 0;
	switch(len) {
#if !FH_READ_UNALIGNED
	case 8: x |= (uint64_t)v[7] << 56;
#endif
	case 7: x |= (uint64_t)v[6] << 48;
	case 6: x |= (uint64_t)v[5] << 40;
	case 5: x |= (uint64_t)v[4] << 32;
	case 4:
#if FH_READ_UNALIGNED
		return x | *(uint32_t*)v;
#else
		x |= v[3] << 24;
#endif
	case 3: x |= v[2] << 16;
	case 2: x |= v[1] << 8;
	case 1: x |= v[0];
	case 0:
		return x;
	}
	return 0;
}

static inline fh_u128_t
fh64_permute_string(fh_u128_t h, const uint8_t *v, size_t len)
{
	size_t n = len / 8;
	for(; n; n--, v+=8) {
#if FH_READ_UNALIGNED
		/* it should be written like this, but clang faster with explicit ordering
		h = fh64_permute(h, *(uint64_t*)v);
		*/
		fh64_permute_imp(&h.a, &h.b, *(uint64_t*)v);
#else
		fh64_permute_imp(&h.a, &h.b, fh_load_u64(v, 8));
#endif
	}
	uint64_t t = (uint64_t)len << 56;
	t |= fh_load_u64(v, len & 7);
	h.a--;
	fh64_permute_imp(&h.a, &h.b, t);
	return h;
}

static inline uint64_t
fh64_finalize(fh_u128_t h)
{
	fh_u128_t t;
	t.a = h.a ^ ((h.a >> 32) + (h.b >> 35));
	t.b = h.b + ((h.a >> 35) ^ (h.b >> 32));
	t.a *= FH_BC1;
	t.b *= FH_BC2;
	return t.a ^ t.b ^ (t.a >> 32) ^ (t.b >> 33);
}

static inline uint64_t
fh64_string_hash(const void* d, size_t len, uint64_t seed)
{
	fh_u128_t v = {seed, seed ^ len};
	v = fh64_permute_string(v, (const uint8_t*)d, len);
	return fh64_finalize(v);
}

static inline uint64_t
fh64_string_hash2(const void* d, size_t len, uint64_t seed1, uint64_t seed2)
{
	fh_u128_t v = {seed1, seed2 ^ len};
	v = fh64_permute_string(v, (const uint8_t*)d, len);
	return fh64_finalize(v);
}

#undef FH_ROTL
#undef FH_READ_UNALIGNED
#endif
