/*
 * ElemType.c
 *
 *  Created on: 18 giu 2023
 *      Author: vitomannone
 */
#include "ElemType.h"

//return a value > 0 if el1 age is greater than el2 age, value = 0 if equal, value < 0 otherwise
int elemCompare(const ElemType_t *el1, const ElemType_t *el2) {
	return (el1->num > el2->num) - (el1->num < el2->num);
}

//returns a copy of the input elements
void elemCopy(const ElemType_t *src, ElemType_t *dest) {
	if(src == NULL || dest == NULL) return;
	dest->num = src->num;
	dest->timer = src->timer;
	clock_t timeIn = clock() ;
	dest->time_in = timeIn ;
}

//swaps the elements content
void elemSwap(ElemType_t *el1, ElemType_t *el2) {
	ElemType_t tmp = *el1;
	*el1 = *el2;
	*el2 = tmp;
}

//free the memory allocated for the element
void elemDel(ElemType_t *el) {
	(void)el;
}

//print element on file
void elemWrite(const ElemType_t *el, FILE *f) {
	fprintf(f, "The num %d has been inserted for %f", el->num, el->timer);
}
