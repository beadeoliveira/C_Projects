#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "kv_cache.h"


// This project implements a KV Cache.

// Lookup the provided key in the KV cache.
// If the key is not found, fetch it from storage by calling read_key
// and place the KVPAIR in the cache.


// If placing a KVPAIR from memory into the cache causes the number of elements
// in the bin to exceed MAX_BIN_ELEMENTS, remove a KVPAIR using the LRU
// replacement policy and call write_key if the removed KVPAIR has
// been modified and changes need to be written back to memory.


// If the key is found in the cache, update LRU order for the
// key's bin appropriately.
// Input: the KV cache hash table and key to lookup
// Return: pointer to KVPAIR with the desired key

KVPAIR *lookup(KVCACHE *kv_cache, long key, unsigned int bins) {

    long indx = key % bins;
    int found = 0;
    KVPAIR *current = kv_cache[indx].list;

    while(current != NULL){
        if(current -> key == key){
            found = 1;
        }
        current = current -> next;
    }

    if(found == 0){

        KVPAIR* new_key = read_key(key);
        misses += 1;
        if(kv_cache[indx].num_keys == 0){
            kv_cache[indx].list = new_key;
            kv_cache[indx].list -> next = NULL;
            kv_cache[indx].list -> prev = NULL;
            kv_cache[indx].num_keys += 1;
        }
        else if (kv_cache[indx].num_keys < MAX_BIN_ELEMENTS){
            kv_cache[indx].list -> prev = new_key;
            new_key -> prev = NULL;
            new_key -> next = kv_cache[indx].list;
            kv_cache[indx].list = new_key;
            kv_cache[indx].num_keys += 1;
        }
        else{
            if(kv_cache[indx].list -> next -> next -> next -> modified != 0){
                write_key(kv_cache[indx].list -> next -> next -> next);
                 writebacks += 1;
            }
            free(kv_cache[indx].list -> next -> next -> next -> val);
            free(kv_cache[indx].list -> next -> next -> next);
            kv_cache[indx].list -> next -> next -> next = NULL;
            kv_cache[indx].list -> prev = new_key;
            new_key -> prev = NULL;
            new_key -> next = kv_cache[indx].list;
            kv_cache[indx].list = new_key;
        }
    }
    else{

        KVPAIR *curr = kv_cache[indx].list;
        hits += 1;
        if(curr -> key != key){
            KVPAIR *frst = kv_cache[indx].list;
            while(curr != NULL){
                if(curr -> key == key){
                    if(curr -> next != NULL){
                        curr -> prev -> next = curr -> next;
                        curr -> next -> prev = curr -> prev;
                    }
                    else{
                        curr -> prev -> next = NULL;
                    }
                    frst -> prev = curr;
                    curr -> prev = NULL;
                    curr -> next = frst;
                    kv_cache[indx].list = curr;
                    break;
                }
                curr =  curr -> next;
            }
        }
    }
    
    KVPAIR *prt = kv_cache[indx].list;

    return prt;

}
