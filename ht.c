/* ht.c
 *
 * Implementation of the hash table using linear probing.
 * Supports insert, lookup, remove, and resizing.
 *
 * Author: Gurshmeer Singh
 */

#include "ht.h"      /* Import the public hashtable header. */
#include "ht_impl.h" /* Also import the private header, just for us! */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* computes hash index for a key within table capacity */

static int hash_index(const char *s, int capacity){
    int hash = 0;

    while (*s != '\0'){
        hash = hash * 31 + *s;
        s++;
    }
    if (hash < 0){
        hash = -hash;
    }
    return hash % capacity;
}

static char *copy_key(const char *key){
    char *new_key;

    new_key = malloc(strlen(key) + 1);
    if (new_key == NULL){
        return NULL;
    }

    strcpy(new_key, key);
    return new_key;
}

/* inserts key-value pair into table without copying (used during resizing) */

static void place_entry(hashtable ht, char *key, void *value){
    int index;

    index = hash_index(key, ht->capacity);

    while (ht->table[index].status == OCCUPIED){
        index = (index + 1) % ht->capacity;
    }

    ht->table[index].key = key;
    ht->table[index].value = value;
    ht->table[index].status = OCCUPIED;
    ht->size++;
}

/* resizes the table to a larger size and rehashes all existing entries 
* resize table if it becomes more than about 2/3 full
*/

static void resize_table(hashtable ht){
    entry *old_table;
    int old_capacity;
    int old_size;
    int i;

    old_table = ht->table;
    old_capacity = ht->capacity;
    old_size = ht->size;

    ht->capacity = ht->capacity * 2 + 1;
    ht->size = 0;

    ht->table = malloc(sizeof(entry) * ht->capacity);
    if (ht->table == NULL){
        ht->table = old_table;
        ht->capacity = old_capacity;
        ht->size = old_size;
        return;
    }

    for (i = 0; i < ht->capacity; i++){
        ht->table[i].key = NULL;
        ht->table[i].value = NULL;
        ht->table[i].status = EMPTY;
    }

    for (i = 0; i < old_capacity; i++){
        if (old_table[i].status == OCCUPIED){
            place_entry(ht, old_table[i].key, old_table[i].value);
        }
    }

    /* fresh table has no tombstones yet, so used == size */
    ht->used = ht->size;

    free(old_table);
}

/* creates and initializes a new empty hash table */

hashtable ht_create(void){
    hashtable ht;
    int i;

    ht = malloc(sizeof(struct ht));
    if (ht == NULL){
        return NULL;
    }

    ht->capacity = START_SIZE;
    ht->size = 0;
    ht->used = 0;

    /* inserts a key-value pair into the table, or updates value if key exists */

    ht->table = malloc(sizeof(entry) * ht->capacity);
    if (ht->table == NULL){
        free(ht);
        return NULL;
    }

    for (i = 0; i < ht->capacity; i++){
        ht->table[i].key = NULL;
        ht->table[i].value = NULL;
        ht->table[i].status = EMPTY;
    }

    return ht;
}

/* inserts a key-value pair into the table, or updates value if key exists */
void ht_insert(hashtable ht, char *key, void *value){
    int index;
    int first_deleted;
    char *new_key;

    if ((ht->used + 1) * 3 > ht->capacity * 2){
    resize_table(ht);
    }

    index = hash_index(key, ht->capacity);
    first_deleted = -1;

    while (ht->table[index].status != EMPTY){
        if (ht->table[index].status == DELETED){
            if (first_deleted == -1){
                first_deleted = index;
            }
        }
        else if (strcmp(ht->table[index].key, key) == 0){
            free(ht->table[index].value);
            ht->table[index].value = value;
            return;
        }

        index = (index + 1) % ht->capacity;
    }

    if (first_deleted != -1){
        index = first_deleted;
    }
    else{
        /* claiming a slot that was EMPTY, not a reused tombstone */
        ht->used++;
    }

    new_key = copy_key(key);
    if (new_key == NULL){
        return;
    }

    ht->table[index].key = new_key;
    ht->table[index].value = value;
    ht->table[index].status = OCCUPIED;
    ht->size++;
}

/* searches for a key and returns its value, or NULL if not found */
void *ht_lookup(const hashtable ht, const char *key){
    int index;
    int start;

    index = hash_index(key, ht->capacity);
    start = index;

    while (ht->table[index].status != EMPTY){
        if (ht->table[index].status == OCCUPIED &&
            strcmp(ht->table[index].key, key) == 0){
            return ht->table[index].value;
        }

        index = (index + 1) % ht->capacity;

        if (index == start){
            break;
        }
    }

    return NULL;
}

/* removes a key from the table and marks the slot as deleted */

void ht_remove(hashtable ht, const char *key){
    int index;
    int start;

    index = hash_index(key, ht->capacity);
    start = index;

    while (ht->table[index].status != EMPTY){
        if (ht->table[index].status == OCCUPIED &&
            strcmp(ht->table[index].key, key) == 0){

            free(ht->table[index].key);
            free(ht->table[index].value);

            ht->table[index].key = NULL;
            ht->table[index].value = NULL;
            ht->table[index].status = DELETED;
            ht->size--;

            return;
        }

        index = (index + 1) % ht->capacity;

        if (index == start){
            break;
        }
    }
}

/* frees all memory used by the hash table */
void ht_free(hashtable ht){
    int i;

    if (ht == NULL){
        return;
    }

    for (i = 0; i < ht->capacity; i++){
        if (ht->table[i].status == OCCUPIED){
            free(ht->table[i].key);
            free(ht->table[i].value);
        }
    }

    free(ht->table);
    free(ht);
}


/* prints the contents of the table for debugging */
void ht_print_dist(const hashtable ht){
    int i;

    for (i = 0; i < ht->capacity; i++){
        if (ht->table[i].status == OCCUPIED){
            printf("%s\n", ht->table[i].key);
        }
        else{
            printf("NULL\n");
        }
    }
}

/* applies a function to each key-value pair in the table */
void ht_foreach(const hashtable ht, ht_iter f){
    int i;

    for (i = 0; i < ht->capacity; i++){
        if (ht->table[i].status == OCCUPIED){
            f(ht->table[i].key, ht->table[i].value);
        }
    }
}