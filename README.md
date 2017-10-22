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

Some benchmark results on Intel CPU 300M random blob
(time relative to murmur3a, less is better):

x86_64
------

By 1-14byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   0.86  |   0.89
funny64   |   0.74  |   0.78
fnv1a     |   0.78  |   0.83
murmur32  |   1.00  |   1.00
murmur128 |   1.24  |   1.26
sip24     |   1.83  |   1.66
sip13     |   1.57  |   1.41
sap24     |   1.81  |   1.58
sap13     |   1.50  |   1.28
lookup3   |   0.90  |   0.87
spooky    |   0.98  |   0.90

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   0.97  |   1.09
funny64   |   0.49  |   0.55
fnv1a     |   3.10  |   3.17
murmur32  |   1.00  |   1.00
murmur128 |   0.42  |   0.47
sip24     |   1.16  |   1.35
sip13     |   0.66  |   0.73
sap24     |   2.31  |   2.71
sap13     |   1.31  |   1.46
lookup3   |   1.06  |   1.00
spooky    |   0.65  |   0.65

x86 (by -m32)
-------------

By 1-14byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   0.96  |   0.90
funny64   |   1.41  |   1.25
fnv1a     |   0.80  |   0.90
murmur32  |   1.00  |   1.00
murmur128 |   1.99  |   2.33
sip24     |   3.88  |   3.28
sip13     |   2.89  |   2.31
sap24     |   1.87  |   1.80
sap13     |   1.62  |   1.47
lookup3   |   1.04  |   1.00
spooky    |   2.27  |   1.55

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.00  |   1.00
funny64   |   1.80  |   1.12
fnv1a     |   3.19  |   3.19
murmur32  |   1.00  |   1.00
murmur128 |   1.80  |   1.77
sip24     |   4.97  |   4.60
sip13     |   2.67  |   1.95
sap24     |   2.37  |   2.74
sap13     |   1.35  |   1.47
lookup3   |   1.09  |   1.05
spooky    |   2.40  |   1.85
