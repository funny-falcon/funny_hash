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
funny32   |   0.90  |   0.87
funny64   |   0.77  |   0.75
fnv1a     |   0.82  |   0.81
murmur32  |   1.00  |   1.00
murmur128 |   1.35  |   1.24
sip24     |   1.96  |   1.62
sip13     |   1.67  |   1.37
sap24     |   1.96  |   1.54
sap13     |   1.60  |   1.25
lookup3   |   0.95  |   0.85
spooky    |   1.03  |   0.88

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   0.96  |   1.08
funny64   |   0.49  |   0.54
fnv1a     |   3.07  |   3.12
murmur32  |   1.00  |   1.00
murmur128 |   0.42  |   0.46
sip24     |   1.15  |   1.34
sip13     |   0.66  |   0.72
sap24     |   2.34  |   2.66
sap13     |   1.30  |   1.44
lookup3   |   1.05  |   0.99
spooky    |   0.65  |   0.64

x86 (by -m32)
-------------

By 1-14byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   0.99  |   0.90
funny64   |   1.40  |   1.26
fnv1a     |   0.80  |   0.89
murmur32  |   1.00  |   1.00
murmur128 |   1.98  |   2.31
sip24     |   3.87  |   3.24
sip13     |   2.88  |   2.29
sap24     |   1.86  |   1.79
sap13     |   1.61  |   1.46
lookup3   |   1.05  |   0.97
spooky    |   2.26  |   1.59

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.02  |   0.99
funny64   |   1.80  |   1.11
fnv1a     |   3.19  |   3.17
murmur32  |   1.00  |   1.00
murmur128 |   1.80  |   1.75
sip24     |   4.99  |   4.58
sip13     |   2.67  |   1.93
sap24     |   2.37  |   2.73
sap13     |   1.39  |   1.40
lookup3   |   1.09  |   1.07
spooky    |   2.40  |   1.87
