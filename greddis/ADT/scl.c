/**
 * Author: Manuel Di Pietro
 * License: 
 * Copyright:
 * Object:
 * 
 *
*/

#include "scl.h"

Scl* scl_new(){
    Scl* list = (Scl*) malloc(sizeof(Scl));
    list->first_element = NULL;
    list->element_number = 0;
    return  list;
}

Scl_node* scl_append_aux(Scl_node* node, void* T){
    if(node == NULL){
        Scl_node* new_node = (Scl_node*) malloc(sizeof(Scl_node));
        new_node->T = T;
        new_node->next = NULL;
        return new_node;
    }
    node->next = scl_append_aux(node->next, T);
    return node;
}

void scl_append(Scl* list, void* T){
    list->first_element = scl_append_aux(list->first_element, T);
    list->element_number += 1;
    return;
}

int scl_element_number(Scl* list){
    return list->element_number;
}

void* scl_get(Scl* list){
    return (list->first_element)->T;
}

void* scl_remove(Scl* list){
    if(list->first_element == NULL)
        return NULL;
    Scl_node* node = list->first_element;
    list->first_element = (list->first_element)->next;
    list->element_number -= 1;
    void* value = node->T;
    free(node);
    return value;
}

void scl_remove_andfreeinfo(Scl* list, void (*T_destroyer) (void*)){
    if(list->first_element == NULL)
        return;
    Scl_node* node = list->first_element;
    list->first_element = (list->first_element)->next;
    list->element_number -= 1;
    T_destroyer(node->T);
    free(node);
    return;
}

void scl_delete(Scl* list){
    while(list->first_element != NULL)
        scl_remove(list);
    free(list);
    return;
}

void scl_delete_andfreeinfo(Scl* list, void (*T_destroyer) (void*)){
    while(list->first_element != NULL)
        scl_remove_andfreeinfo(list, T_destroyer);
    free(list);
    return;
}

void* scl_q_aux(Scl_node* node, void* T, int (*compar) (const void*, const void*)){
    if(node == NULL)
        return NULL;
    if( compar(node->T, T) == 0)
        return node->T;
    return scl_q_aux(node->next, T, compar);
}

void* scl_q(Scl* list, void* T, int (*compar) (const void*, const void*)){
    return scl_q_aux(list->first_element, T, compar);
}

Scl_node* scl_qr_aux(Scl_node* node, void** findend, void* T, int (*compar) (const void*, const void*)){
    if(node == NULL)
        return NULL;
    if(compar(node->T, T) == 0){
        (*findend) = node->T;
        Scl_node* new_next;
        new_next = node->next;
        free(node);
        return new_next;
    }
    node->next = scl_qr_aux(node->next, findend, T, compar);
    return node;
}

void* scl_qr(Scl* list, void* T, int (*compar) (const void*, const void*)){
    void* finded = NULL;
    list->first_element = scl_qr_aux(list->first_element, &finded, T, compar);
    if(finded!=NULL) list->element_number -= 1;
    return finded;
}

Scl_node* scl_qr_andfreeinfo_aux(Scl_node* node, void* T, int (*compar) (const void*, const void*), void (*T_destroyer) (void*)){
    if(node == NULL)
        return NULL;
    if(compar(node->T, T) == 0){
        T_destroyer(node->T);
        Scl_node* new_next;
        if(node->next == NULL)
            new_next = NULL;
        new_next = node->next;
        free(node);
        return new_next;
    }
    node->next = scl_qr_andfreeinfo_aux(node->next, T, compar, T_destroyer);
    return node;
}

void scl_qr_andfreeinfo(Scl* list, void* T, int (*compar) (const void*, const void*), void (*T_destroyer) (void*)){
    void* finded = NULL;
    list->first_element = scl_qr_andfreeinfo_aux(list->first_element, T, compar, T_destroyer);
    if(finded!=NULL) list->element_number -= 1;
    return;
}

void scl_p_winfo_aux(Scl_node* node, void (*print_T)(void*)){
    if(node == NULL)
        return;
    printf("Indirizzo nodo: %p, next: %p, T: ", node, node->next);
    print_T(node->T);
    printf("\n");
    scl_p_winfo_aux(node->next, print_T);
    return;
}

void scl_p_winfo(Scl* list, void (*print_T)(void*)){
    printf("Indirizzo lista: %p, Indirizzo primo elemento: %p, Numero di elementi: %d\n", list, list->first_element, list->element_number);
    scl_p_winfo_aux(list->first_element, print_T);
    return;
}

void scl_p_aux(Scl_node* node){
    if(node == NULL)
        return;
    printf("Indirizzo nodo: %p, next: %p, T-pointer: %p\n", node, node->next, node->T);
    scl_p_aux(node->next);
    return;
}

void scl_p(Scl* list){
    printf("Indirizzo lista: %p, Indirizzo primo elemento: %p, Numero di elementi: %d\n", list, list->first_element, list->element_number);
    scl_p_aux(list->first_element);
    return;
}

void scl_get_new_iter_aux(Iter* iter, Scl_node* node){
    if(node == NULL)
        return;
    printf("Aggiunto %p!\n", node);
    iter_append_element(iter, node->T);
    scl_get_new_iter_aux(iter, node->next);
    return;
}

Iter* scl_get_new_iter(Scl* list){
    Iter* new_iter = iter_new();
    scl_get_new_iter_aux(new_iter, list->first_element);
    return new_iter;
}