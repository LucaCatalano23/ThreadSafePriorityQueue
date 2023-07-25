/*
 *  emManager.c
 *
 *  Created on: 18 lug 2023
 *      Authors: Luca Catalano, Luca Eugenio Catalano
 */

#include "emManager.h"

double Step = (double) MAX_LIFESPAN/N_QUEUE_PRIORITY ;

void manage(QueueHandle_t queues) {
    //check each multiple queue
    for(int i = 0; i < N_QUEUE_PRIORITY; ++i) {       
        CBuffer_t *queue = queues->dataStruct[i]; 
        int sem_retval = emManagerPort_EnterCritical(queue->sem);
        if(sem_retval != 1) {
		    return NULL;
	    }
        //check each element of the multiple queue
	    for(int y = 0; y < queue->maxElems; ++y) {    
            if((y < queue->headInd) && (y >= queue->tailInd && (queue->headInd != queue->tailInd))) {
                //check the remaining time of the element
                uint8_t *source = (uint8_t*)queue->startBuffer + (y * queue->elemSize);
                value* elem = (value *) source ;
                clock_t now = clock();
                double milliseconds = (double) (((now - elem->time_in) * 1000.0) / CLOCKS_PER_SEC) ;
                double slack = elem->lifespan - milliseconds ;
                /*DA USARE PER DEBUG*/
                /*
                  printf("Num:%d  Slack:%f Coda:%d Indice:%d\n",elem->num, slack, i, y) ;
                */
                elem->time_in = now ;
                elem->lifespan = slack ;
                if(slack > 0) {
                    //increase priority
                    aging(queues, queue, i, slack, y, source) ;
                }
                else {
                    printf("%d ELIMINATO\n", elem->num) ;
                    //delete element
                    deleteElement(queues, queue, i, y, source) ;
                }
            }
        }
        emManagerPort_ExitCritical(queue->sem);
	}
}
void aging(QueueHandle_t queues, CBuffer_t *queue, int currentPriority, double slack, int elemInd, uint8_t *source) {
    size_t priority = (size_t) (slack / Step) ;
    //if the priority is change
    if (priority < currentPriority) {
        value* elem = (value *) source ;
        printf("%d DALLA CODA A PRIORITA' %d ALLA CODA A PRIORITA' %ld\n", elem->num, currentPriority, priority) ;
        /*
          Effettuo uno shift a destra dell'elemento un numero pari di volte 
          tale da fare raggiungere l'elemento in coda all'array circolare
        */
        for(int z = 1; z <= (elemInd-queue->tailInd)%queue->maxElems; z++) {
            uint8_t* destination = (uint8_t*)queue->startBuffer + (((elemInd-z)%queue->maxElems) * queue->elemSize);
            emManagerPort_Swap(source, destination) ;
        }
        emManagerPort_ExitCritical(queue->sem);
        //Estraggo il valore dalla coda per metterlo in testa all'array con priorità corretta
        void * src = emManagerPort_StructGetTail(queues->dataStruct[currentPriority]);
        void * dest = emManagerPort_StructGetHead(queues->dataStruct[priority]) ;
        emManagerPort_ElemCpy(src, dest, queues->elemSize);  
        int sem_retval = emManagerPort_EnterCritical(queue->sem);
        if(sem_retval != 1) {
		    return NULL;
	    } 
    }           
}

void deleteElement(QueueHandle_t queues, CBuffer_t *queue, int currentPriority, int elemInd, uint8_t *source) {
    /*
      Effettuo uno shift a destra dell'elemento un numero pari di volte 
      tale da fare raggiungere l'elemento in coda all'array circolare
    */
    for(int z = 1; z <= (elemInd-queue->tailInd)%queue->maxElems; z++) {             
        uint8_t* destination = (uint8_t*)queue->startBuffer + (((elemInd-z)%queue->maxElems) * queue->elemSize);
        emManagerPort_Swap(source, destination) ;
    }
    emManagerPort_ExitCritical(queue->sem);
    //Estraggo l'elemento dalla coda dell'array
    emManagerPort_StructGetTail(queues->dataStruct[currentPriority]);  
    int sem_retval = emManagerPort_EnterCritical(queue->sem);
    if(sem_retval != 1) {
	    return NULL;
	}            
}