#include "../ht.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
    hashtable ht;
    char *v1;
    char *v2;
    char *v3;
    char *found;

    ht = ht_create();
    if (ht == NULL){
        printf("create failed\n");
        return 1;
    }

    v1 = malloc(20);
    v2 = malloc(20);
    v3 = malloc(20);

    strcpy(v1, "Alberta");
    strcpy(v2, "Ontario");
    strcpy(v3, "Quebec");

    ht_insert(ht, "Calgary", v1);
    ht_insert(ht, "Toronto", v2);
    ht_insert(ht, "Montreal", v3);

    found = ht_lookup(ht, "Toronto");
    if (found != NULL){
        printf("lookup: %s\n", found);
    }
    else{
        printf("lookup failed\n");
    }

    ht_remove(ht, "Toronto");

    found = ht_lookup(ht, "Toronto");
    if (found == NULL){
        printf("remove works\n");
    }
    else{
        printf("remove failed\n");
    }

    ht_free(ht);
    return 0;
}