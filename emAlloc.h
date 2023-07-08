/*
 * emAlloc.h
 *
 *  Created on: 18 giu 2023
 *      Author: vitomannone
 */

#ifndef EMALLOC_H_
#define EMALLOC_H_

#include <stddef.h>

#include "emAllocPort.h"

/*
 * User defines for configuration
 */
#define EM_USE_USER_HEAP 0 //set if you want to use your own heap memory buffer

#define EM_TOTAL_HEAP_MEM 1024 //dimension in bytes of the heap memory

typedef size_t Address_t; //for porting to 64bit processor

typedef enum {
	emNotInizialised = -1,
	emMalloc_OK,
	emMalloc_SizeError,
	emMalloc_NoFreeBlockFound,
	emMalloc_NullPointerInput,
	emMalloc_InvalidPointerInput,
	emMalloc_SemError
} emErrorReturn_t;

/*
 * Definition of the malloc function without relying on stdlib.h
 */
void *emMalloc(size_t size);

/*
 * Definition of the free function without relying on stdlib.h
 */
void emFree(void *ptr);

/*
 * Definition of the function to get the errorCode in case of error return value in emMalloc
 */
emErrorReturn_t getEmMallocError(void);


#endif /* EMALLOC_H_ */
