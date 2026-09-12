#include "../ht.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
    hashtable ht;
    char key[20];
    char *value;
    int i;

    ht = ht_create();
    if (ht == NULL){
        printf("create failed\n");
        return 1;
    }

    for (i = 0; i < 20; i++){
        sprintf(key, "city%d", i);

        value = malloc(20);
        if (value == NULL){
            printf("value allocation failed\n");
            ht_free(ht);
            return 1;
        }

        sprintf(value, "value%d", i);
        ht_insert(ht, key, value);
    }

    for (i = 0; i < 20; i++){
        sprintf(key, "city%d", i);

        if (ht_lookup(ht, key) == NULL){
            printf("resize test failed at %s\n", key);
            ht_free(ht);
            return 1;
        }
    }

    printf("resize test passed\n");

    ht_free(ht);
    return 0;
}