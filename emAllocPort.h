/*
 * emAllocPort.h
 *
 *  Created on: 26 giu 2023
 *      Author: vitomannone
 */

#ifndef EMALLOCPORT_H_
#define EMALLOCPORT_H_

#define ALLOC_USE_LOCK_MECHANISM 1

//check if the compiler is GCC then find the byte alignment
//if the compiler is not GCC the user must defines the Byte Alignment
#if defined(__GNUC__)
#define BYTE_ALIGNMENT __BIGGEST_ALIGNMENT__
#else
//#define BYTE_ALIGNMENT
#ifndef BYTE_ALIGNMENT
#error "You have to define the number of bytes necessary for the alignment if the compiler is not GCC"
#endif /* BYTE_ALIGNMENT */
#endif /* defined(__GNUC__) */

/*
 * Necessary definition for lock/unlock functions
 */
#if ALLOC_USE_LOCK_MECHANISM
#include <pthread.h>
#include <semaphore.h>
#include "PresentazioneEsame.h"
#define emAllocPort_EnterCritical(ptrSem)			( !sem_wait(ptrSem) )
#define emAllocPort_ExitCritical(ptrSem)			( !sem_post(ptrSem) )
#define emAllocPort_InitBynSem(strName)				( my_sem_init(1, strName) )
#define emAllocPort_BynSemDelete(ptrSem)			( sem_close(ptrSem) )
typedef sem_t* emAllocSem_t;
#else
#define emAllocPort_EnterCritical(ptrSem)			( 1 )
#define emAllocPort_ExitCritical(ptrSem)			(void)ptrSem
#define emAllocPort_InitBynSem(strName)				( NULL ); (void)strName
#define emAllocPort_BynSemDelete(ptrSem)			(void)ptrSem
typedef void * emAllocSem_t;
#endif /* USE_LOCK_MECHANISM */


#endif /* EMALLOCPORT_H_ */
