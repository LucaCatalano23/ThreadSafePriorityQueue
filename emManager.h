/*
 * emManager.h
 *
 *  Created on: 18 lug 2023
 *      Authors: Luca Catalano, Luca Eugenio Corrado
 */

#ifndef EMMANAGER_H_
#define EMMANAGER_H_

#include <stdio.h>
#include <time.h>

#include "emManagerPort.h"

typedef struct {
	void **dataStruct;
	Queue_sem_t semHandle;
	size_t elemSize;
} handler_t ;

struct Struttura {
	int num;
	double lifespan;
	clock_t time_in;
};

typedef struct Struttura value ;

typedef handler_t * QueueHandle_t;

/*PUBLIC FUNCTIONS*/
 
//function that manage the lifespan policy
void manage(QueueHandle_t queues) ;

/*PRIVATE FUNCTIONS*/

//increase the priority when decrease the lifetime
void aging(QueueHandle_t queues, CBuffer_t *queue, int currentPriority, double slack, int elemInd, uint8_t *source);

//delete the element whent when the lifetime is 0
void deleteElement(QueueHandle_t queues, CBuffer_t *queue, int currentPriority, int elemInd, uint8_t *source);

#endif /* MANAGER_H_ */