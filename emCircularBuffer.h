/*
 * emCircularBuffer.h
 *
 *  Created on: 21 giu 2023
 *      Author: vitomannone, Luca Catalano, Luca Eugenio Catalano
 */

#ifndef EMCIRCULARBUFFER_H_
#define EMCIRCULARBUFFER_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "emCircularPort.h"

#define CB_DEBUG 0

typedef enum {
	CB_error = -1,
	CB_false,
	CB_true
} CBStatus_t;

typedef struct CBuffer_t {
	uint32_t tailInd;
	uint32_t headInd;
	uint8_t *startBuffer;
	size_t elemSize;
	size_t maxElems;
	size_t NbElems;
	CB_sem_t sem;
} CBuffer_t ;

CBuffer_t *emCircularInit(const size_t maxElems, const size_t elemSize, const char *name);

CBStatus_t emCircularDelete(CBuffer_t *buffer);

CBStatus_t emCircularIsEmpty(const CBuffer_t *buffer);

CBStatus_t emCircularIsFull(const CBuffer_t *buffer);

size_t emCircularGetRemainingSpace(const CBuffer_t *buffer);

void *emCircularGetHead(CBuffer_t *buffer);

void *emCircularGetTail(CBuffer_t *buffer);

void stampaCoda(CBuffer_t *buffer);

#endif /* EMCIRCULARBUFFER_H_ */
