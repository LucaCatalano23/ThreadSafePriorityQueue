/*
 * emCircularBuffer.c
 *
 *  Created on: 21 giu 2023
 *      Author: vitomannone
 */
#include "emCircularBuffer.h"
#include "emCircularPort.h"

/*
 * PRIVATE FUNCTIONS
 */
#if CB_DEBUG
#include <stdio.h>
#define CB_DEBUG_Print(...) printf(__VA_ARGS__)
#else
#define CB_DEBUG_Print(...)
#endif

/*
 * PUBLIC FUNCTIONS
 */

CBuffer_t *emCircularInit(const size_t maxElems, const size_t elemSize, const char *name) {
	if(maxElems < 2) return NULL;
	if(elemSize < 1) return NULL;
	CBuffer_t *retval = (CBuffer_t*)emCircularPortMalloc(sizeof(CBuffer_t));
	uint8_t *buffer = (uint8_t*)emCircularPortMalloc(maxElems * elemSize);
	retval->headInd = 0;
	retval->tailInd = 0;
	retval->startBuffer = buffer;
	retval->elemSize = elemSize;
	retval->maxElems = maxElems;
	retval->NbElems = 0;
	retval->sem = emCircularPort_InitBynSem(name);
#if CIRCULAR_USE_LOCK_MECHANISM
	if(retval->sem == NULL) {
		retval = NULL;
		CB_DEBUG_Print("CB Error:\tCannot inizialize buffer semaphore!\r\n");
	}
#endif
#if CB_DEBUG
	if(retval != NULL) {
		CB_DEBUG_Print("CB:\tCircularBuffer initialised. CB pointer is %p.\r\n", (void*)retval);
	}
#endif
	return retval;
}

CBStatus_t emCircularDelete(CBuffer_t *buffer) {
	CBStatus_t retval = CB_false;
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	CB_sem_t temp_sem = buffer->sem;
	if(sem_retval != 1) {
		return CB_error;
	}
	if(buffer->startBuffer != NULL) {
		emCircularPortFree(buffer->startBuffer);
	}
	if(buffer == NULL) {
		retval = CB_true;
	} else {
		emCircularPortFree(buffer);
	}
	emCircularPort_ExitCritical(temp_sem);
	emCircularPort_BynSemDelete(temp_sem);
	return retval;
}

CBStatus_t emCircularIsEmpty(const CBuffer_t *buffer) {
	CBStatus_t retval = CB_false;
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	if(sem_retval != 1) {
		return CB_error;
	}
	if(buffer == NULL) {
		retval = CB_error;
	} else {
		if ( buffer->headInd == buffer->tailInd ) {
			retval = CB_true;
		}
	}
	emCircularPort_ExitCritical(buffer->sem);
	return retval;
}

CBStatus_t emCircularIsFull(const CBuffer_t *buffer) {
	CBStatus_t retval = CB_false;
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	if(sem_retval != 1) {
		return CB_error;
	}
	if(buffer == NULL) {
		retval = CB_error;
	} else {
		if ( ( (buffer->headInd + 1) % buffer->maxElems ) == buffer->tailInd ) {
			retval = CB_true;
		}
	}
	emCircularPort_ExitCritical(buffer->sem);
	return retval;
}

size_t emCircularGetRemainingSpace(const CBuffer_t *buffer) {
	if(buffer == NULL) return 0;
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	if(sem_retval != 1) {
		return 0;
	}
	size_t retval = 0;
	if(buffer->maxElems >= buffer->NbElems) {
		retval = buffer->maxElems - buffer->NbElems;
	}
	emCircularPort_ExitCritical(buffer->sem);
	return retval;
}

void *emCircularGetHead(CBuffer_t *buffer) {
	if(buffer == NULL) return NULL;
	switch(emCircularIsFull(buffer)) {
	case CB_true:
		CB_DEBUG_Print("CB:\tBuffer is full.\r\n");
		return NULL;
	case CB_error:
		CB_DEBUG_Print("CB:\tError checking whether CB is full.\r\n");
		return NULL;
	case CB_false:
		break;
	default:
		CB_DEBUG_Print("CB:\tError emCircularIsFull returned a a strange value.\r\n");
		return NULL;
	}
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	if(sem_retval != 1) {
		return NULL;
	}
	void *retval = (uint8_t*)buffer->startBuffer + (buffer->headInd * buffer->elemSize);

	CB_DEBUG_Print("CB:\tBuffer Head pointer is %p.\r\n", retval);

	buffer->headInd = (buffer->headInd + 1) % buffer->maxElems;
	buffer->NbElems++;

	if(buffer->NbElems > buffer->maxElems) {
		CB_DEBUG_Print("Error!! Number of elements greater than max number of elements!\r\n");
		retval = NULL;
	}
	emCircularPort_ExitCritical(buffer->sem);
	return retval;
}

void *emCircularGetTail(CBuffer_t *buffer) {
	if(buffer == NULL) return NULL;
	switch(emCircularIsEmpty(buffer)) {
	case CB_true:
		CB_DEBUG_Print("CB:\tBuffer is empty.\r\n");
		return NULL;
	case CB_error:
		CB_DEBUG_Print("CB:\tError checking whether CB is empty.\r\n");
		return NULL;
	case CB_false:
		break;
	default:
		CB_DEBUG_Print("CB:\tError emCircularIsFull returned a a strange value.\r\n");
		return NULL;
	}
	int sem_retval = emCircularPort_EnterCritical(buffer->sem);
	if(sem_retval != 1) {
		return NULL;
	}
	void *retval = (uint8_t*)buffer->startBuffer + (buffer->tailInd * buffer->elemSize);

	CB_DEBUG_Print("CB:\tBuffer Tail pointer is %p.\r\n", retval);

	buffer->tailInd = (buffer->tailInd + 1) % buffer->maxElems;
	buffer->NbElems--;

	if(buffer->NbElems < 0) {
		CB_DEBUG_Print("Error!! No elements in the buffer!\r\n");
		retval = NULL;
	}
	emCircularPort_ExitCritical(buffer->sem);
	return retval;
}

void stampaCoda(CBuffer_t *buffer) {

	printf("elementi %ld, indice coda %d, indice testa %d\n", buffer->NbElems, buffer->tailInd, buffer->headInd) ;
	for(int i = 0; i < buffer->maxElems; ++i) {
		uint8_t *dato = (uint8_t*)buffer->startBuffer + (i * buffer->elemSize);
		ElemType_t *value = (ElemType_t*) dato ;
		printf("%d/%f || ", value->num, (double)value->timer);
	}
	printf("\n");
}
