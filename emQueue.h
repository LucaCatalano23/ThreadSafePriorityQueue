/*
 * emQueue.h
 *
 *  Created on: 17 giu 2023
 *      Author: vitomannone
 */

#ifndef EMQUEUE_H_
#define EMQUEUE_H_

#include <stdint.h>
#include <stddef.h>

/*
 * Include porting header file
 */
#include "emQueuePort.h"
/*
 * API Functions
 */

typedef enum {
	emError = -1,
	em_False,
	em_True,
	em_SemError,
	em_QueueFull,
	em_QueueEmpty,
} emQueueReturn_t;

typedef struct {
	void **dataStruct;
	Queue_sem_t semHandle;
	size_t elemSize;
} Handler_t ;

typedef Handler_t * emQueueHandle_t;

emQueueHandle_t emQueue_New(const size_t queueSize, const size_t elemSize, const char *name, const size_t n_priority);

emQueueReturn_t emQueue_IsFull(emQueueHandle_t queue, size_t priority);

emQueueReturn_t emQueue_IsEmpty(emQueueHandle_t queue, size_t priority);

emQueueReturn_t emQueue_Put(emQueueHandle_t queue, const void *ptrElem, size_t priority);

emQueueReturn_t emQueue_Get(emQueueHandle_t queue, void *ptrDest, size_t n_priority);

emQueueReturn_t emQueue_Delete(emQueueHandle_t queue);

#endif /* EMQUEUE_H_ */
