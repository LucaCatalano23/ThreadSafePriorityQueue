/*
 * ElemType.h
 *
 *  Created on: 18 giu 2023
 *      Author: vitomannone
 */

#ifndef ELEMTYPE_H_
#define ELEMTYPE_H_

#include <stdio.h>
#include <stdint.h>

struct UserDataStruct_t {
	uint8_t age;
	char name[16];
	double height;
	double weight;
};

typedef struct UserDataStruct_t ElemType_t;

//return a value > 0 if el1 is greater than el2, value = 0 if inputs are equal, value < 0 if el1 < el2
int elemCompare(const ElemType_t *el1, const ElemType_t *el2);

//returns a copy of the input elements
void elemCopy(const ElemType_t *src, ElemType_t *dest);

//swaps the elements content
void elemSwap(ElemType_t *el1, ElemType_t *el2);

//free the memory allocated for the element
void elemDel(ElemType_t *el);

//print element on file
void elemWrite(const ElemType_t *el, FILE *f);


#endif /* ELEMTYPE_H_ */
