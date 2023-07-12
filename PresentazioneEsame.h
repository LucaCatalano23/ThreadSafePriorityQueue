/** @file PresentazioneEsame.h
 * 
 * @brief A description of the module’s purpose. 
 *
 * 
 */ 

#ifndef PRESENTAZIONE_ESAME_H
#define PRESENTAZIONE_ESAME_H

#include <stddef.h>

void shuffle(void *array[], size_t n);

//questa funzione permette di inizializzare i semafori con una funzione simile
//a sem_init, serve per compatibilità con macOs
void *my_sem_init(const size_t count, const char *name);

#define N_SCRITTORI 7
#define N_LETTORI 3
#define N_SCRITTURE 3
#define L_BUFFER 10
#define MIN_MS_PAUSA 100
#define MAX_MS_PAUSA 500

#define ESAME_SCRITTORE()       enQueue_example_01()
#define ESAME_LETTORE()         deQueue_example_02()

void enQueue_example_01(void);
void deQueue_example_02(void);

#endif /* PRESENTAZIONE_ESAME_H */

/*** end of file ***/
