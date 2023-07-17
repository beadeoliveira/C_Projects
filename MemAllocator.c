// A segregated storage memory allocator.
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "alloc.h"

int max_heap_size = 1024;  // default is a 1K heap (pretty small)

void *heap[4] = {NULL,NULL,NULL,NULL}; // the heap bins for each segregated size 8,16,32,64

unsigned int heap_size = 0; // A variable that keeps track of the how much memory we allocated

// allocate_slab uses the default malloc library to allocate memory
// input: size in bytes of slab to allocate
// return: pointer to slab allocated, NULL if allocation would exceed max_heap_size
void *allocate_slab(unsigned int size) {
    
    if(size + heap_size > max_heap_size){
        return NULL;
    }
    else{
        heap_size += size;
        void* ret = malloc(size);
        return ret;
    }

}

// Returns a pointer to a heap block of the appropriate
// size.
// If no such block exists in the free list, this function calls
// allocate_slab to get space NUM_HEAP_BLOCKS blocks of the
// appropriate size and adds them to the free list, then returns
// one of them.
void *my_alloc(unsigned int size) {

    int idx;

    if(size <= 8){
        idx = 0;
    }
    else if(size > 8 && size <= 16){
        idx = 1;
    }
    else if(size > 16 && size <= 32){
        idx = 2;
    }
    else if(size > 32 && size <= 64){
        idx = 3;
    }
    else{
        return NULL;
    }

    if(heap[idx] == NULL){
        unsigned int sze = (size + sizeof(blk_header_t)) * NUM_HEAP_BLOCKS;
        void* mem = allocate_slab(sze);
        if(mem == NULL){
            printf("Malloc failed, out of memory!\n");
            exit(0);
        }
        blk_header_t* blk = (blk_header_t*)mem;
        blk_header_t* first = blk;
        for(int i = 0; i < NUM_HEAP_BLOCKS - 1; i++){
            blk -> size = size;
            blk -> next = (void*)blk + sizeof(blk_header_t) + size;
            blk = blk -> next;
        }
        blk -> size = size;
        blk -> next = NULL;
        heap[idx] = (void*)first;
        blk_header_t* ret = (blk_header_t*)heap[idx];
        heap[idx] = (void*)(ret -> next);
        ret -> next = NULL;
        return (void*)ret + sizeof(blk_header_t);
    }
    else{
        blk_header_t* ret = (blk_header_t*)heap[idx];
        heap[idx] = (void*)(ret -> next);
        ret -> next = NULL;
        return (void*)ret + sizeof(blk_header_t);
    }

}

// Given a pointer to an allocated block, frees the block
// by placing it at the head of the appropriate free list.
// Does nothing if given a NULL pointer.
void my_free(void *ptr) {
    if(ptr != NULL){
        blk_header_t* hdr = (blk_header_t*)(ptr - sizeof(blk_header_t));
        int size = hdr -> size;

        int idx;
        if(size <= 8){
        idx = 0;
        }
        else if(size > 8 && size <= 16){
            idx = 1;
        }
        else if(size > 16 && size <= 32){
            idx = 2;
        }
        else{
            idx = 3;
        }

        hdr -> next = heap[idx];
        heap[idx] = (void*)hdr;
    }

    return;
}
