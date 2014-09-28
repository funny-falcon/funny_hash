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

Some benchmark results on Core-i3 2330M 2000MHz 300M random file:

x86_64
By 1-20byte substrings twice
function  | gcc -O2  | clang -O2
-------------------------------
funny32   | 1.729613 | 1.602314
murmur32  | 1.982314 | 1.674969
funny64   | 2.046157 | 1.920657
murmur128 | 2.523948 | 2.362249
lookup3   | 2.355228 | 2.002795
sip13     | 3.176685 | 2.651061
sip24     | 3.786810 | 3.181494

10 times 300M
function  | gcc -O2  | clang -O2
--------------------------------
funny32   | 2.537391 | 2.151737
murmur32  | 2.150074 | 2.189807
funny64   | 1.284647 | 1.138336
murmur128 | 1.103038 | 1.301775
lookup3   | 2.917266 | 2.909621
sip13     | 1.871403 | 1.811810
sip24     | 3.432657 | 3.354086

x86 (by -m32)
By 1-20byte substrings twice
function  | gcc -O2  | clang -O2
--------------------------------
funny32   | 1.907123 | 1.796084
murmur32  | 2.200570 | 1.876548
funny64   | 3.361184 | 2.679012
murmur128 | 4.865942 | 4.223887
lookup3   | 2.443087 | 2.262780
sip13     | 6.422192 | 4.836447
sip24     | 8.775827 | 6.917179


10 times 300M
function  | gcc -O2   | clang -O2
---------------------------------
funny32   | 2.393317  | 2.153600
murmur32  | 2.180202  | 2.191719
funny64   | 3.839035  | 2.516634
murmur128 | 4.354525  | 4.351691
lookup3   | 2.875730  | 2.727401
sip13     | 6.741251  | 7.384649
sip24     | 12.455516 | 15.148697
