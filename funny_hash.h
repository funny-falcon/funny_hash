#ifndef FUNNY_HASH_H
#define FUNNY_HASH_H
/*
 * Copyright (C) 2014 Sokolov Yura aka funny_falcon
 *
 * It is in public domain, free to use and modify by everyone for any usage.
 * I will be glad, if you put copyright notice above in your code or distribution, but you don't ought to.
 * I'm not responsible for any damage cause by this piece of code.
 */

#ifndef FH_READ_UNALIGNED
#define FH_READ_UNALIGNED 1
#endif
#define FH_ROTL(x, n) (((x) << (n)) | ((x) >> (sizeof(x)*8 - (n))))
#ifndef U64_CONSTANT
#define U64_CONSTANT(x) (x##ull)
#endif

typedef struct fh_u64s {
	uint32_t a, b;
} fh_u64_t;
static const uint32_t FH_C1 = 0xb8b34b2d;
static const uint32_t FH_C2 = 0x52c6a5d9;
static inline fh_u64_t
fh32_permute(fh_u64_t h, uint32_t v)
{
	fh_u64_t t;
	t.a = FH_ROTL(h.a ^ v, 16) * FH_C1;
	t.b = (FH_ROTL(h.b, 16) ^ v) * FH_C2;
	return t;
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
		h = fh32_permute(h, *(uint32_t*)v);
#else
		h = fh32_permute(h, fh_load_u32(v, 4));
#endif
	}
	t = (uint32_t)len << 24;
	switch(len & 3) {
	case 3: t |= fh_load_u32(v, 3); break;
	case 2: t |= fh_load_u32(v, 2); break;
	case 1: t |= fh_load_u32(v, 1); break;
	}
	h.a--; /* fight against length extension */
	h = fh32_permute(h, t);
	return h;
}


static uint32_t
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

typedef struct fh_u128s {
	uint64_t a, b;
} fh_u128_t;
static const uint64_t FH_BC1 = U64_CONSTANT(0xacd5ad43274593b9);
static const uint64_t FH_BC2 = U64_CONSTANT(0x6956abd6ed558e3d);
static inline fh_u128_t
fh64_permute(fh_u128_t h, uint64_t v)
{
	fh_u128_t t;
	t.a = FH_ROTL(h.a ^ v, 32) * FH_BC1;
	t.b = (FH_ROTL(h.b, 32) ^ v) * FH_BC2;
	return t;
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
		h = fh64_permute(h, *(uint64_t*)v);
#else
		h = fh64_permute(h, fh_load_u64(v, 8));
#endif
	}
	uint64_t t = (uint64_t)len << 56;
	switch(len & 7) {
	case 7: t |= fh_load_u64(v, 7); break;
	case 6: t |= fh_load_u64(v, 6); break;
	case 5: t |= fh_load_u64(v, 5); break;
	case 4: t |= fh_load_u64(v, 4); break;
	case 3: t |= fh_load_u64(v, 3); break;
	case 2: t |= fh_load_u64(v, 2); break;
	case 1: t |= fh_load_u64(v, 1); break;
	}
	h.a--; /* fight against length extension */
	h = fh64_permute(h, t);
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
