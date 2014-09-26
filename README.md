funny_hash
==========

Simple multiplicative hash function like Murmur3 but a bit safer.

It is safer cause it uses state twice larger than block size.
It is not cryptographic, but I think it is safe for use in a hash tables with secret seed
(so that no seed nor result hash is known to attacker - Murmur3 were collision unsafe even in this case).

There is two boxes of building blocks: 32bit hash for 32bit cpu and 64bit hash for 64bit cpu.

````C
/* 64bit state for 32bit result */
typedef ... fh_u64_t;
/* mix 32bit integer into 64bit state */
fh_u64_t fh32_permute(fh_u64_t h, uint32_t v);
/* hash string, seed with and produce 64bit state */
fh_u64_t fh32_permute_string(fh_u64_t h, const uint8_t *v, size_t len);
/* state finalization to 32bit value */
uint32_t fh32_finalize(fh_u64_t h);
/* convinient function to hash buffer with 32bit seed */
uint32_t fh32_string_hash(const void *buf, size_t len, uint32_t seed);
/* convinient function to hash buffer with 64bit seed */
uint32_t fh32_string_hash(const void *buf, size_t len, uint32_t seed1, uint32_t seed2);

/* 128bit state for 64bit result */
typedef ... fh_u128_t;
/* mix 64bit integer into 128bit state */
fh_u128_t fh64_permute(fh_u128_t h, uint64_t v);
/* hash string, seed with and produce 128bit state */
fh_u128_t fh64_permute_string(fh_u128_t h, const uint8_t *v, size_t len);
/* state finalization to 64bit value */
uint64_t fh64_finalize(fh_u128_t h);
/* convinient function to hash buffer with 64bit seed */
uint64_t fh64_string_hash(const void *buf, size_t len, uint64_t seed);
/* convinient function to hash buffer with 128bit seed */
uint64_t fh64_string_hash(const void *buf, size_t len, uint64_t seed1, uint64_t seed2);
````
