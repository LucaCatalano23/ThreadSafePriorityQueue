/*
 * emManagerPort.h
 *
 *  Created on: /18 lugl 2023
 *      Author: Luca Catalano, Luca Eugenio Corrado
 */

#ifndef EMMANAGERPORT_H_
#define EMMANAGERPORT_H_

#define MANAGER_USE_LOCK_MECHANISM 1

#if MANAGER_USE_LOCK_MECHANISM
#include <pthread.h>
#include <semaphore.h>
#include "PresentazioneEsame.h"
#define emManagerPort_EnterCritical(ptrSem)			    ( !sem_wait(ptrSem) )
#define emManagerPort_ExitCritical(ptrSem)				( !sem_post(ptrSem) )
typedef sem_t* CB_sem_t;
#else
#define emManagerPort_EnterCritical(ptrSem)		    	( 1 )
#define emManagerPort_ExitCritical(ptrSem)				(void)ptrSem
typedef void * CB_sem_t;
#endif /* USE_LOCK_MECHANISM */

#include "ElemType.h"
#include "emQueue.h"

#define emManagerPort_ElemCpy(ptrSrc, ptrDest, size) 				    elemCopy(ptrSrc, ptrDest)
#define emManagerPort_Swap(src, dest)                                   elemSwap(src, dest)

#define emManagerPort_StructGetHead(ptrStruct)							emCircularGetHead(ptrStruct)
#define emManagerPort_StructGetTail(ptrStruct)							emCircularGetTail(ptrStruct)

#endif /* EMMANAGERPORT_H_ */