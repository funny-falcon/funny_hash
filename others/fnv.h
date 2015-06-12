static inline uint32_t fnv1a (const void* d, size_t len, uint32_t seed) {
    const uint8_t * str = d;
    const uint8_t * const end = (const uint8_t*)str + len;
    uint32_t hash = 0x811C9DC5 + seed; /* maybe also get rid of seed */
    while (str < end) {
	hash ^= *str++;
	hash *= 16777619;
    }
    return hash;
}
