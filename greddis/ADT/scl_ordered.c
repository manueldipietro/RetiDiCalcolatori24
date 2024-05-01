#include "scl_ordered.h"

Scl_ordered* scl_ordered_new(int (*compar)(const void*, const void*)){
    Scl_ordered* list = malloc(sizeof(Scl_ordered));
    list->compar = compar;
    list->scl = scl_new();
    return list;
}

//Scl_node* scl_append_aux(Scl_node* node, void* T);
Scl_node* scl_ordered_append_aux(Scl_node* node, void* T, int (*compar)(const void*, const void*)){
    //Cond cortociruitata e node->T > T => Inserisci T;
    if( node == NULL){
        Scl_node* new_node =  malloc(sizeof(Scl_node));
        new_node->next = NULL;
        new_node->T = T;
        printf("G\n");
        return new_node;
    }
    if(compar(node->T, T) > 0 ){
        Scl_node* new_node =  malloc(sizeof(Scl_node));
        new_node->next = node;
        new_node->T = T;
        return new_node;
    }
    node->next = scl_ordered_append_aux(node->next, T, compar);
    return node;
}

void scl_ordered_append(Scl_ordered* list, void* T){
    list->scl->first_element = scl_ordered_append_aux(list->scl->first_element, T, list->compar);
    list->scl->element_number += 1;
    return;
}

int scl_ordered_element_number(Scl_ordered* list){
    return scl_element_number(list->scl);
}

void* scl_ordered_get(Scl_ordered* list){
    return scl_get(list->scl);
}

void* scl_ordered_remove(Scl_ordered* list){
    return scl_remove(list->scl);
}

void scl_ordered_remove_andfreeinfo(Scl_ordered* list, void (*T_destroyer) (void*)){
    scl_remove_andfreeinfo(list->scl, T_destroyer);
    return;
}

void scl_ordered_delete(Scl_ordered* list){
    scl_delete(list->scl);
    return;
}

void scl_ordered_delete_andfreeinfo(Scl_ordered* list, void (*T_destroyer) (void*)){
    scl_delete_andfreeinfo(list->scl, T_destroyer);
    return;
}

void* scl_ordered_q(Scl_ordered* list, void* T, int (*compar) (const void*, const void*)){
    return scl_q(list->scl, T, compar);
}

void* scl_ordered_qr(Scl_ordered* list, void* T, int (*compar) (const void*, const void*)){
    return scl_qr(list->scl, T, compar);
}


void scl_ordered_qr_andfreeinfo(Scl_ordered* list, void* T, int (*compar) (const void*, const void*), void (*T_destroyer) (void*)){
    scl_qr_andfreeinfo(list->scl, T, compar, T_destroyer);
    return;
}

void scl_ordered_p_winfo(Scl_ordered* list, void (*print_T)(void*)){
    scl_p_winfo(list->scl, print_T);
    return;
}

void scl_ordered_p(Scl_ordered* list){
    scl_p(list->scl);
    return;
}

Iter* scl_ordered_get_new_iter(Scl_ordered* list){
    return scl_get_new_iter(list->scl);
}