/* pcode.c
 *
 * Reads a file of city and postal code pairs and stores them using a hash table.
 * Each city maps to a list of postal codes. The program then allows the user
 * to enter city names and prints all postal codes for that city.
 *
 * Author: Gurshmeer Singh
 */


#include "ht.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* node for storing one postal code in a linked list */
typedef struct pnode {
    char *code;
    struct pnode *next;
} pnode;

typedef struct {
    pnode *head;
    pnode *tail;
} code_list;

/* creates a dynamically allocated copy of a string */
char *copy_string(const char *s){
    char *copy;

    copy = malloc(strlen(s) + 1);
    if (copy == NULL){
        return NULL;
    }

    strcpy(copy, s);
    return copy;
}

/* creates an empty list to store postal codes for a city */
code_list *create_code_list(void){
    code_list *list;

    list = malloc(sizeof(code_list));
    if (list == NULL){
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    return list;
}

/* adds a postal code to the end of the linked list (keeps order from file) */
void add_code(code_list *list, const char *code){
    pnode *new_node;

    new_node = malloc(sizeof(pnode));
    if (new_node == NULL){
        return;
    }

    new_node->code = copy_string(code);
    if (new_node->code == NULL){
        free(new_node);
        return;
    }

    new_node->next = NULL;

    if (list->head == NULL){
        list->head = new_node;
        list->tail = new_node;
    }
    else{
        list->tail->next = new_node;
        list->tail = new_node;
    }
}

/* prints all postal codes, with at most 10 codes per line */
void print_codes(code_list *list){
    pnode *cur;
    int count;

    cur = list->head;
    count = 0;

    while (cur != NULL){
        printf("%s", cur->code);
        count++;

        if (cur->next != NULL){
            if (count % 10 == 0){
                printf("\n");
            }
            else{
                printf(",");
            }
        }

        cur = cur->next;
    }

    printf("\n");
}

/* frees all nodes and strings inside a code list (used before ht_free) */
void free_code_list_value(char *key, void *value){
    code_list *list;
    pnode *cur;
    pnode *next;

    (void)key;

    list = (code_list *)value;
    cur = list->head;

    while (cur != NULL){
        next = cur->next;
        free(cur->code);
        free(cur);
        cur = next;
    }
}

int main(int argc, char *argv[]){
    FILE *fp;
    hashtable ht;
    char line[200];
    char query[200];
    char *city;
    char *postal;
    char *newline;
    code_list *list;

    /* check that input file is provided */
    if (argc != 2){
        printf("Usage: %s inputfile\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL){
        printf("Could not open file\n");
        return 1;
    }

    /* create hash table */
    ht = ht_create();
    if (ht == NULL){
        printf("Could not create hash table\n");
        fclose(fp);
        return 1;
    }

    /* read each line from file and store city -> postal code mapping */
    while (fgets(line, sizeof(line), fp) != NULL){
        newline = strchr(line, '\n');
        if (newline != NULL){
            *newline = '\0';
        }

        city = strtok(line, ",");
        postal = strtok(NULL, ",");

        if (city == NULL || postal == NULL){
            continue;
        }

        list = (code_list *)ht_lookup(ht, city);

        /* if city not found, create a new list and insert into table */
        if (list == NULL){
            list = create_code_list();
            if (list == NULL){
                continue;
            }

        /* if city already exists, just add the new postal code */
            add_code(list, postal);
            ht_insert(ht, city, list);
        }
        else{
            add_code(list, postal);
        }
    }

    fclose(fp);

    while (fgets(query, sizeof(query), stdin) != NULL){
        newline = strchr(query, '\n');
        if (newline != NULL){
            *newline = '\0';
        }

        list = (code_list *)ht_lookup(ht, query);

        if (list != NULL){
            print_codes(list);
        }
    }

    /* free all postal code lists stored in the table */
    ht_foreach(ht, free_code_list_value);
    ht_free(ht);

    return 0;
}