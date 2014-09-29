funny_hash
==========

Simple multiplicative hash function like Murmur3 but a bit safer.

It is safer cause it uses state twice larger than block size, feeds block twice at time and every bit affects at least 10/20 bits of a state. So it is impossible to revert change without introducing other change, and I think it is hardly possible to make seed independent collisions (which is the main defect of Murmur2/Murmur3).

So it is safe for use in a internal hash table implementations when nor seed nor hashsum is known to attacker.

(but it is certainly not cryptographic: I think attack with choosen plaintext and known hashsum will succeed
with little effort)

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
uint32_t fh32_string_hash2(const void *buf, size_t len, uint32_t seed1, uint32_t seed2);

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
uint64_t fh64_string_hash2(const void *buf, size_t len, uint64_t seed1, uint64_t seed2);
````

Benchmark
=========

Some benchmark results on Core-i3 2330M 2000MHz 300M random file:

x86_64
------

By 1-20byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.75  |   1.60 
funny64   |   1.86  |   1.69 
murmur32  |   2.03  |   1.68 
murmur128 |   2.48  |   2.37 
sip24     |   3.72  |   3.20 
sip13     |   3.18  |   2.65 
lookup3   |   2.37  |   2.01 

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   2.49  |   2.23 
funny64   |   1.23  |   1.18 
murmur32  |   2.09  |   2.24 
murmur128 |   1.05  |   1.35 
sip24     |   3.40  |   3.32 
sip13     |   1.80  |   1.87 
lookup3   |   2.85  |   2.94 

x86 (by -m32)
-------------

By 1-20byte substrings twice

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   1.89  |   1.78 
funny64   |   3.37  |   2.67 
murmur32  |   2.19  |   1.87 
murmur128 |   4.86  |   4.10 
sip24     |   8.80  |   6.93 
sip13     |   6.41  |   4.86 
lookup3   |   2.54  |   2.21 

10 times 300M at once

function  | gcc -O2 | clang -O2
----------|---------|----------
funny32   |   2.35  |   2.15 
funny64   |   3.77  |   2.67 
murmur32  |   2.12  |   2.18 
murmur128 |   4.30  |   4.37 
sip24     |  12.33  |  15.19 
sip13     |   6.66  |   7.41 
lookup3   |   2.81  |   2.95 
