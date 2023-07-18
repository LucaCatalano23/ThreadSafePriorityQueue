/*
 * emQueue.c
 *
 *  Created on: 21 giu 2023
 *      Author: vitomannone
 */
#include "emQueue.h"

double step = (double) MAX_LIFESPAN / N_QUEUE_PRIORITY;

emQueueHandle_t emQueue_New(const size_t queueSize, const size_t elemSize, const char *name, const size_t n_priority) {
	if( (queueSize < 2) || (elemSize < 1) ) return NULL;
	emQueueHandle_t retval = emQueuePort_Malloc(sizeof(Handler_t));
	if(retval != NULL) {
		retval->dataStruct = emQueuePort_Malloc(n_priority * sizeof(void*)) ;
		for(size_t i = 0; i < n_priority; i++) {
			retval->dataStruct[i] = emQueuePort_InitStruct(queueSize, elemSize, name);
		}
		retval->semHandle = emQueuePort_InitBynSem(name);
		retval->elemSize = elemSize;
	}
	return retval;
}

emQueueReturn_t emQueue_IsFull(emQueueHandle_t queue, size_t priority) {
	emQueueReturn_t retval = emQueuePort_EnterCritical(queue->semHandle);
	if(retval != 1) {
		return em_SemError;
	} else {
		int temp = emQueuePort_StructIsFull(queue->dataStruct[priority]);
		
		retval = temp ? em_True : em_False;
	}
	emQueuePort_ExitCritical(queue->semHandle);
	return retval;
}

emQueueReturn_t emQueue_IsEmpty(emQueueHandle_t queue, size_t priority) {
	emQueueReturn_t retval = emQueuePort_EnterCritical(queue->semHandle);
	if(retval != 1) {
		return em_SemError;
	} else {
		int temp = emQueuePort_StructIsEmpty(queue->dataStruct[priority]);
		
		retval = temp ? em_True : em_False;
	}
	emQueuePort_ExitCritical(queue->semHandle);
	return retval;
}

emQueueReturn_t emQueue_Put(emQueueHandle_t queue, void *ptr) {
	data* ptrElem = (data*) ptr;
	if(queue == NULL) return emError;
	size_t priority = (size_t) (ptrElem->lifespan / step) ;
	emQueueReturn_t retVal = emQueuePort_EnterCritical(queue->semHandle);
	if(retVal == 1) {
		if(emQueuePort_StructIsFull(queue->dataStruct[priority])) {
			retVal = em_QueueFull;
		} else {
			void * dest = emQueuePort_StructGetHead(queue->dataStruct[priority]) ; 
			emQueuePort_ElemCpy(ptrElem, dest, queue->elemSize) ;
			retVal = em_True;
		}
	} else {
		return em_SemError;
	}
	emQueuePort_ExitCritical(queue->semHandle);
	return (emQueueReturn_t)retVal;
}

emQueueReturn_t emQueue_Get(emQueueHandle_t queue, void *ptrDest, size_t n_priority) {
	if(queue == NULL) return emError;
	emQueueReturn_t retVal = emQueuePort_EnterCritical(queue->semHandle);
	if(retVal == 1) {
		for(size_t i = 0; i < n_priority; i++) {
			if(emQueuePort_StructIsEmpty(queue->dataStruct[i])) {
				retVal = em_QueueEmpty;
			} else {
				void * src = emQueuePort_StructGetTail(queue->dataStruct[i]);
				emQueuePort_ElemCpy(src, ptrDest, queue->elemSize);
				retVal = em_True;
				break;
			}
		}
	} else {
		return em_SemError;
	}
	emQueuePort_ExitCritical(queue->semHandle);
	return (emQueueReturn_t)retVal;
}

emQueueReturn_t emQueue_Delete(emQueueHandle_t queue) {
	if(queue == NULL) return em_True;
	/* Save the reference to the semaphore */
	void *sem = queue->semHandle;
	int retVal = emQueuePort_EnterCritical(sem);

	for(size_t i = 0; i < sizeof(queue->dataStruct)/sizeof(queue->dataStruct[0]); ++i) {
		emQueueport_DeleteStruct(queue->dataStruct + i);
	}

	emQueuePort_Free(queue);

	emQueuePort_ExitCritical(sem);
	emQueuePort_BynSemDelete(sem);
	return (emQueueReturn_t)retVal;
}
