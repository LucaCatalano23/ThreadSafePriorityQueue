/*
 * emCircularPort.h
 *
 *  Created on: 21 giu 2023
 *      Author: vitomannone
 */

#ifndef EMCIRCULARPORT_H_
#define EMCIRCULARPORT_H_

#define CIRCULAR_USE_LOCK_MECHANISM 1
#define CIRCULAR_USE_EMALLOC 0

#include "ElemType.h"
/*
 * Definition of necessary functions for memory management:
 * User can define its own emQueuePortMalloc() implementation
 */
#if CIRCULAR_USE_EMALLOC
#include "emAlloc.h"
#define emCircularPortMalloc(bytes) 					emMalloc(bytes)
#define emCircularPortFree(pointer) 					emFree(pointer)
#else
#include <stdlib.h>
#define emCircularPortMalloc(bytes) 					malloc(bytes)
#define emCircularPortFree(pointer) 					free(pointer)
#endif

/*
 * Necessary definition for lock/unlock functions
 */
#if CIRCULAR_USE_LOCK_MECHANISM
#include <pthread.h>
#include <semaphore.h>
#include "PresentazioneEsame.h"
#define emCircularPort_EnterCritical(ptrSem)			( !sem_wait(ptrSem) )
#define emCircularPort_ExitCritical(ptrSem)				( !sem_post(ptrSem) )
#define emCircularPort_InitBynSem(strName)				( my_sem_init(1, strName) )
#define emCircularPort_BynSemDelete(ptrSem)				( sem_close(ptrSem) )
typedef sem_t* CB_sem_t;
#else
#define emCircularPort_EnterCritical(ptrSem)			( 1 )
#define emCircularPort_ExitCritical(ptrSem)				(void)ptrSem
#define emCircularPort_InitBynSem(strName)				( NULL ); (void)strName
#define emCircularPort_BynSemDelete(ptrSem)				(void)ptrSem
typedef void * CB_sem_t;
#endif /* USE_LOCK_MECHANISM */



#endif /* EMCIRCULARPORT_H_ */
