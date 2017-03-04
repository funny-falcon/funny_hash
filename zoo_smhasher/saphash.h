#ifndef SIPHASH_H
#define SIPHASH_H

#if defined(_MSC_VER)
	typedef unsigned __int64 uint64_t;
#else
	#include <stdint.h>
	#include <stdlib.h>
#endif

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
 
uint32_t saphash(const unsigned char key[8], const unsigned char *m, size_t len);
 
#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif


#endif // SIPHASH_H
