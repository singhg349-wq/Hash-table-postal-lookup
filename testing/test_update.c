#include "../ht.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
    hashtable ht;
    char *v1;
    char *v2;
    char *found;

    ht = ht_create();
    if (ht == NULL){
        printf("create failed\n");
        return 1;
    }

    v1 = malloc(20);
    v2 = malloc(20);

    if (v1 == NULL || v2 == NULL){
        printf("allocation failed\n");
        ht_free(ht);
        return 1;
    }

    strcpy(v1, "oldvalue");
    strcpy(v2, "newvalue");

    ht_insert(ht, "Calgary", v1);
    ht_insert(ht, "Calgary", v2);

    found = ht_lookup(ht, "Calgary");

    if (found != NULL){
        printf("%s\n", found);
    }
    else{
        printf("update failed\n");
    }

    ht_free(ht);
    return 0;
}