/****************** PRIVATE INCLUDES ********************************/

#include "emAlloc.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/****************** END PRIVATE INCLUDES ****************************/

/****************** PRIVATE DEFINES *********************************/

#define BYTE_ALIGNMENT_MASK (Address_t)(BYTE_ALIGNMENT-1)
#define NODE_STRUCTURE_SIZE sizeof(FreeBlockNode_t)
#define MINIMUM_BLOCK_SIZE (NODE_STRUCTURE_SIZE<<1)

/****************** END PRIVATE DEFINES *****************************/

/****************** PRIVATE PROTOTYPES ******************************/

//definition of linked list structure of free data blocks
typedef struct FreeBlockNode_t {
    size_t size;
    struct FreeBlockNode_t *ptrNext;
} FreeBlockNode_t;

/****************** END PRIVATE PROTOTYPES **************************/

/****************** PRIVATE VARIABLES *******************************/

emErrorReturn_t emMallocStatus = emNotInizialised;

#if EM_USE_USER_HEAP
extern uint8_t heapBuf[EM_TOTAL_HEAP_MEM];
#else
static uint8_t heapBuf[EM_TOTAL_HEAP_MEM];
#endif

static FreeBlockNode_t startNode, *endNode = NULL;

static size_t minimumEverSizeAvaiable = 0;
static size_t remainingBytes = 0;
static size_t allocatedFlag = ( (size_t) 1 ) << ( (sizeof(size_t)<<3) - 1 );

static emAllocSem_t heapSem;

/****************** END PRIVATE VARIABLES ***************************/

/****************** PRIVATE FUNCTIONS *******************************/

static inline void *getAlignedAddress(const uint8_t *startAddr) {
    Address_t retval = (Address_t)startAddr;
    if(retval & BYTE_ALIGNMENT_MASK) {
        retval += BYTE_ALIGNMENT_MASK;
        retval &= ~(BYTE_ALIGNMENT_MASK);
    }
    return (void*)retval;
}

static void emHeapInit(void) {

	FreeBlockNode_t *firstNode = NULL;

    void *xAddress = getAlignedAddress(heapBuf); //get an aligned address for the first free block

    startNode.size = 0;
    startNode.ptrNext = (FreeBlockNode_t*)xAddress;

    //allocate the first node
    firstNode = (FreeBlockNode_t*)xAddress;

    //allocate the last node to calculate the size of first node
    xAddress = getAlignedAddress((uint8_t*)xAddress + EM_TOTAL_HEAP_MEM - NODE_STRUCTURE_SIZE);
    endNode = (FreeBlockNode_t*)xAddress;
    endNode->size = 0;
    endNode->ptrNext = NULL;

    //complete to allocate the first node
    firstNode->size = (Address_t)xAddress - (Address_t)firstNode;
    firstNode->ptrNext = endNode;

    minimumEverSizeAvaiable = firstNode->size;
    remainingBytes = firstNode->size;

#if ALLOC_USE_LOCK_MECHANISM
	char sem_name[16];
	snprintf(sem_name, 16, "heapBuf_sem");
    heapSem = emAllocPort_InitBynSem(sem_name);
    if(heapSem == NULL) {
    	emMallocStatus = emMalloc_SemError;
    }
#endif

}

static inline size_t getNbAlignedBytes(const size_t bytes) {
	size_t retval = bytes + NODE_STRUCTURE_SIZE;
	if ( retval & BYTE_ALIGNMENT_MASK ) {
		retval += ( BYTE_ALIGNMENT - (retval & BYTE_ALIGNMENT_MASK) );;
		return retval;
	}

	return retval;
}

static void insertNodeInList(FreeBlockNode_t *node) {
	//iterate through list to find a free block with higher address
	FreeBlockNode_t *temp = &startNode;
	while(temp->ptrNext < node) {
		temp = temp->ptrNext;
	}

	uint8_t *tempAddr = (uint8_t*)temp; //take the address of the free block node
	//handle the situation where the freeNode to insert is contiguous with the previous one
	if((tempAddr + temp->size) == (uint8_t*)node) {
		temp->size += node->size;
		//the node to insert starts and the address of the previous node
		node = temp;
	}

	tempAddr = (uint8_t*)node;
	//handle the situation where the next free block is contiguous with the current node to insert
	if((tempAddr + node->size) == (uint8_t*) temp->ptrNext) {

		if(temp->ptrNext != endNode) {
			node->size += temp->ptrNext->size;
			node->ptrNext = temp->ptrNext->ptrNext;
		} else {
			node->ptrNext = endNode;
		}

	} else {
		node->ptrNext = temp->ptrNext;
	}

	if(temp != node) {
		temp->ptrNext = node;
	}
}

/****************** END PRIVATE FUNCTIONS ***************************/

/****************** PUBLIC FUNCTIONS ********************************/

emErrorReturn_t getEmMallocError(void) {
	emErrorReturn_t retval = emMallocStatus;
	emMallocStatus = emMalloc_OK; //reset status if acknowledged
	return retval;
}

void* emMalloc(size_t wantedSize) {

	void *retPtr = NULL;
	FreeBlockNode_t *prevPtr = NULL, *nextPtr = NULL, *newBlock = NULL;

	if(endNode == NULL) {
		emHeapInit();
	}

	int sem_retval = emAllocPort_EnterCritical(heapSem);
	if(sem_retval != 1) {
		emMallocStatus = emMalloc_SemError;
		return NULL;
	}

	if(wantedSize <= 0) {
		emMallocStatus = emMalloc_SizeError;
		emAllocPort_ExitCritical(heapSem);
		return NULL;
	}

	size_t necessary_size = getNbAlignedBytes(wantedSize);

	if(necessary_size <= remainingBytes) {
		prevPtr = &startNode;
		nextPtr = startNode.ptrNext;

		//search for a free block that is big enough
		while( (nextPtr->size < necessary_size) && (nextPtr->ptrNext != NULL) ) {
			prevPtr = nextPtr;
			nextPtr = nextPtr->ptrNext;
		}

		//work with the block only if the end has not been reached
		if( nextPtr != endNode ) {

			//the return pointer will be the ptrNext with an offset of the memory needed for the node information
			retPtr = (void *) ( ((uint8_t *)prevPtr->ptrNext) + NODE_STRUCTURE_SIZE );

			prevPtr->ptrNext = nextPtr->ptrNext;
			//prevPtr has already been allocated at the last malloc or in the heapInit()
			if( (nextPtr->size - necessary_size) > MINIMUM_BLOCK_SIZE ) {
				newBlock = (FreeBlockNode_t*) (void*)((uint8_t*)nextPtr + necessary_size);
				newBlock->size = nextPtr->size - wantedSize;
				nextPtr->size = necessary_size;
				insertNodeInList(newBlock);
			}

			remainingBytes -= nextPtr->size;
			if(remainingBytes < minimumEverSizeAvaiable) {
				minimumEverSizeAvaiable = remainingBytes;
			}

			nextPtr->size |= allocatedFlag;
			nextPtr->ptrNext = NULL;

		} else {
			emMallocStatus = emMalloc_NoFreeBlockFound;
		}
	}

	emAllocPort_ExitCritical(heapSem);
	return retPtr;
}

void emFree(void *ptr) {
	//check input value
	if(ptr == NULL) {
		emMallocStatus = emMalloc_NullPointerInput;
		return;
	}

	int sem_retval = emAllocPort_EnterCritical(heapSem);
	if(sem_retval != 1) {
		emMallocStatus = emMalloc_SemError;
		return;
	}

	if( ( (uint8_t*)ptr < heapBuf ) || ( (uint8_t*)ptr > (heapBuf + EM_TOTAL_HEAP_MEM) ) ) {
		emMallocStatus = emMalloc_InvalidPointerInput;
		emAllocPort_ExitCritical(heapSem);
		return;
	}

	//actual node starting address
	uint8_t *tempAddr = (uint8_t*)ptr - NODE_STRUCTURE_SIZE;
	FreeBlockNode_t *block = (FreeBlockNode_t*)( (void*)tempAddr );

	if( ((size_t)block & allocatedFlag) != 0) {
		emMallocStatus = emMalloc_InvalidPointerInput;
		emAllocPort_ExitCritical(heapSem);
		return;
	}

	if(block->ptrNext == NULL) {
		remainingBytes += block->size;
		block->size &= ~allocatedFlag;
		memset(ptr, 0, block->size - sizeof(FreeBlockNode_t));
		insertNodeInList(block);
	}
	emAllocPort_ExitCritical(heapSem);
}

/****************** END PUBLIC FUNCTIONS ****************************/
