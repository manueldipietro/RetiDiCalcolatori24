#include "scl.h"


Linked_list* linked_list_new(){
    Linked_list* list = malloc(sizeof(Linked_list));
    list->first_element = NULL;
    list->element_number = 0;
    return  list;
}

void linked_list_add(Linked_list* list, void* T){
    list->first_element = linked_list_add_aux(list->first_element, T);
    list->element_number += 1;
    return;
}

Linked_list_node* linked_list_add_aux(Linked_list_node* node, void* T){
    if(node == NULL){
        Linked_list_node* new_node = malloc(sizeof(Linked_list_node));
        new_node->T = T;
        new_node->next = NULL;
        return new_node;
    }
    node->next = linked_list_add_aux(node->next, T);
    return node;
}

void* linked_list_remove(Linked_list* list){
    Linked_list_node* node = list->first_element;
    list->first_element = (list->first_element)->next;
    list->element_number -= 1;
    void* value = node->T;
    free(node);
    return value;
}

void* linked_list_get_first_element_value(Linked_list* list){
    return (list->first_element)->T;
}

int linked_list_get_element_number(Linked_list* list){
    return list->element_number;
}


void linked_list_destroyer_aux(Linked_list_node* node, void (*destroyer)()){
    if(node == NULL)
        return;
    linked_list_destroyer_aux(node->next, destroyer);
    destroyer(node->T);
    free(node);
    return;
}

void linked_list_destroyer(Linked_list* list, void (*destroyer)()){
    linked_list_destroyer_aux(list->first_element, destroyer);
    free(list);
    return;
}

void* linked_list_search_by_value_aux(Linked_list_node* node, void* T, int (*compar) (const void*, const void*)){
    if(node == NULL)
        return NULL;
    if( compar(node->T, T) == 0)
        return node->T;
    return linked_list_search_by_value_aux(node->next, T, compar);
}

void* linked_list_search_by_value(Linked_list* list, void* T, int (*compar) (const void*, const void*)){
    return linked_list_search_by_value_aux(list->first_element, T, compar);
}

void printSCL(Linked_list* list, void (*print_T)(void*)){
    printf("Indirizzo lista: %p, Indirizzo primo elemento: %p, Numero di elementi: %d\n", list, list->first_element, list->element_number);
    printSCL_aux(list->first_element, print_T);
}

void printSCL_aux(Linked_list_node* node, void (*print_T)(void*)){
    if(node == NULL)
        return;
    printf("Indirizzo nodo: %p, next: %p, T: ", node, node->next);
    print_T(node->T);
    printf("\n");
    printSCL_aux(node->next, print_T);
    return;
}