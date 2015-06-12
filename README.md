funny_hash
==========

Simple multiplicative hash function like Murmur3 but a bit safer.

It is safer cause it uses state twice larger than block size, feeds block twice at time and every bit affects at least 10/20 bits of a state. So it is impossible to revert change without introducing other change, and I think it is hardly possible to make seed independent collisions (which is the main defect of Murmur2/Murmur3).

So it is safe for use in a internal hash table implementations when nor seed nor hashsum is known to attacker.

(but it is certainly not cryptographic: I think attack with choosen plaintext and known hashsum will succeed
with little effort)

There is two boxes of building blocks: 32bit hash for 32bit cpu and 64bit hash for 64bit cpu.

````C
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
````

Benchmark
=========

Some benchmark results on Core-i7 2600 2400MHz 300M random blob:

x86_64
------

By 1-14byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.06  |   1.08
funny64   |   1.02  |   1.03
fnv1a     |   0.95  |   0.96
murmur32  |   1.23  |   1.19
murmur128 |   1.62  |   1.83
sip24     |   2.46  |   2.33
sip13     |   2.04  |   1.81
lookup3   |   1.13  |   1.21
spooky    |   1.24  |   1.18

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.22  |   1.11
funny64   |   0.62  |   0.58
fnv1a     |   3.19  |   3.22
murmur32  |   1.10  |   1.12
murmur128 |   0.59  |   0.68
sip24     |   1.76  |   2.28
sip13     |   0.92  |   0.92
lookup3   |   1.47  |   1.51
spooky    |   0.73  |   0.72

x86 (by -m32)
-------------

By 1-14byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.16  |   1.29
funny64   |   2.18  |   1.79
fnv1a     |   0.99  |   1.08
murmur32  |   1.33  |   1.29
murmur128 |   2.83  |   2.80
sip24     |   5.39  |   3.87
sip13     |   3.87  |   3.02
lookup3   |   1.53  |   1.29
spooky    |   2.95  |   2.04

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.21  |   1.10
funny64   |   1.92  |   1.98
fnv1a     |   3.21  |   3.22
murmur32  |   1.09  |   1.12
murmur128 |   2.48  |   2.48
sip24     |   6.12  |   4.55
sip13     |   3.35  |   2.62
lookup3   |   1.48  |   1.44
spooky    |   3.31  |   1.71
