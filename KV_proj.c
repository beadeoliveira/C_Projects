#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "kv_store.h"

// PROJECT GOAL: data deserialization of a custom key-value buffer

/**
 * Purpose: deserialize a key-value pair buffer
 * 
 * Inputs:
 * - char* buf: pointer to the key-value pair buffer
 * - size_t buffer_size: total size of the key-value pair buffer (in bytes)
 * 
 * Return:
 * - KVPAIR* start_of_list: pointer to start of linked list of KVPAIRs if successful
 * - (OR)
 * - NULL if an error occurs
 * 
 * Assumptions: buf is not empty or NULL and the buffer_size is the correct size of buf
 */

KVPAIR *deserialize(char *buf, size_t buffer_size) {
    KVPAIR* head = NULL;
    KVPAIR* start_of_list = NULL;
    KVPAIR* previous = NULL;

    while(buffer_size > 0){
        head = (KVPAIR*) malloc(sizeof(KVPAIR));

        memcpy(&(head -> key), buf, sizeof(long));
        buf += 8;
        buffer_size -= 8;

        memcpy(&(head -> size), buf, sizeof(unsigned int));
        buf += 4;
        buffer_size -= 4;

        head -> val = (char*) malloc(head->size);

        memcpy(head-> val, buf, (head -> size));
        buf += head->size;
        buffer_size -= head->size;


        head->next = NULL;
        if(previous == NULL){
            start_of_list = head;
            previous = head;
        }
        else{
            previous -> next = head;
            previous = previous-> next;
        }
    }

    return start_of_list;

}


/**
 * Purpose: Lookup the provided key in a KV list
 * 
 * Inputs: 
 * - KVPAIR* list: pointer to the start of the linked list of KVPAIRs
 * - long key: the key to lookup in the linked list
 * 
 * Return:
 * - KVPAIR* found_entry: pointer to the entry that contains the key
 * - (OR)
 * - NULL if the key is not found
 */

KVPAIR *lookup(KVPAIR *list, long key) {



    KVPAIR* found_entry = NULL;

    while(list != NULL){
        if(list -> key == key){
            found_entry = list;
            return found_entry;
        }
        else{
            list = list -> next;
        }
    }

    

    return NULL;
}


/**
 * Purpose: Delete the KVPAIR with the specified key from the KVPAIR list
 * 
 * Inputs: 
 * KVPAIR** list: pointer to the pointer of first KVPAIR
 * long key: the key to the KVPAIR entry that should be deleted
 * 
 * Return:
 * 1: if key found in list and KVPAIR entry successfully deleted
 * 0: if key not found in list and/or some error occurs in deletion
 * 
 * Hints:
 * (hint: You will need to change KVPAIR's next pointers in the list and free memory)
 * (hint: What needs to be updated if the first entry in the list is deleted? KVPAIR** list)
 */

int delete(KVPAIR **list, long key) {

    KVPAIR* lst = (*list);
    KVPAIR* t = NULL;

    while(lst != NULL){
        if((lst) -> key == key){
            if(t == NULL){
                (*list) = (*list)->next;
                lst -> next = NULL;
                free(lst -> val);
                free(lst);
                return 1;
            }
            else{
                t -> next = (*list) -> next;
                lst-> next = NULL;
                free(lst -> val);
                free(lst);
                return 1;
            }
           
        }

        t = lst;
        lst = lst -> next;
    }

    return 0;

}
