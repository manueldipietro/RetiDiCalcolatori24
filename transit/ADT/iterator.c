#include "iterator.h"

Iter* iter_new(){
    Iter* iter = (Iter*) malloc(sizeof(Iter));
    iter->pointers = NULL;
    iter->number_of_remaining_element = 0;
    return iter;
}

Iter_node* iter_append_element_aux(Iter_node* node, Iter_node* new_node){
    if(node == NULL)
        return new_node;
    node->next = iter_append_element_aux(node->next, new_node);
    return node;
}

void iter_append_element(Iter* iter, void* pointer){
    Iter_node* new_node = (Iter_node*) malloc(sizeof(Iter_node));
    new_node->next = NULL;
    new_node->pointer = pointer;
    
    iter->pointers = iter_append_element_aux(iter->pointers, new_node);
    
    iter->number_of_remaining_element += 1;
    return;
}

void* iter_next(Iter* iter){
    if(iter->pointers == NULL || iter->number_of_remaining_element == 0){
        return NULL;
    }
    
    void* pointer = (iter->pointers)->pointer;

    Iter_node* old_node = iter->pointers;
    
    iter->number_of_remaining_element -= 1;

    iter->pointers = (iter->pointers)->next;
    free(old_node);
    
    return pointer;
}

void iter_delete(Iter* iter){
    while(iter->number_of_remaining_element > 0)
        iter_next(iter);
    free(iter);
    return;
}

int iter_remaining_elem(Iter* iter){
    return iter->number_of_remaining_element;
}

void print_iter_aux(Iter_node* node){
    if(node == NULL)
        return;
    printf("%p, %p, %p\n", node, node->next, node->pointer);
    print_iter_aux(node->next);
    return;
}

void print_iter(Iter* iter){
    printf("Puntatore iter: %p, elem_r: %d, primo: %p\n", iter, iter->number_of_remaining_element, iter->pointers);
    print_iter_aux(iter->pointers);
    return;
}