#pragma once

#include <stdio.h>
#include <stdlib.h>

struct Linked_list_node{
    void* T;
    struct Linked_list_node *next;
};
typedef struct Linked_list_node Linked_list_node;

typedef struct Linked_list{
    Linked_list_node* first_element;
    int element_number;
}Linked_list;


Linked_list* linked_list_new();
void linked_list_add(Linked_list* list, void* T);
void linked_list_add(Linked_list* list, void* T);
Linked_list_node* linked_list_add_aux(Linked_list_node* node, void* T);
void* linked_list_remove(Linked_list* list);
void* linked_list_get_first_element_value(Linked_list* list);
int linked_list_get_element_number(Linked_list* list);
void linked_list_destroyer_aux(Linked_list_node* node, void (*destroyer)());
void linked_list_destroyer(Linked_list* list, void (*destroyer)());

void* linked_list_search_by_value(Linked_list* list, void* T, int (*compar) (const void*, const void*));
void* linked_list_search_by_value(Linked_list* list, void* T, int (*compar) (const void*, const void*));

void printSCL(Linked_list* list, void (*print_T)(void*));
void printSCL_aux(Linked_list_node* node, void (*print_T)(void*));
