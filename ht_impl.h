/* ht_impl.h
 *
 * For any "private"  declarations that should not be visible to the public
 * users of the hash table, but might want to be shared across various source
 * files in the hash table implementation.
 * 
 * It is a common convention to append "_impl" to a private implementation of
 * some public interface, so we do so here.
 *
 * Author: Gurshmeer Singh
 */

#ifndef _HT_IMPL_H_
#define _HT_IMPL_H_

#include <stdlib.h>

/* state shows whether a slot in the table is empty, in use, or deleted */
typedef enum {
   EMPTY,
   OCCUPIED,
   DELETED
} state;

/* one entry in the hash table: stores a key, value, and slot status */

typedef struct{
   char *key;
   void *value;
   state status;
} entry;

/* main hash table structure: keeps track of capacity, size, and the table array */

struct ht {
   int capacity;
   int size;
   int used; /* count of OCCUPIED + DELETED slots; drives resizing so that
              * tombstones from ht_remove can't fill the table and leave
              * ht_insert with no EMPTY slot to terminate its probe on */
   entry *table;
};

#endif
