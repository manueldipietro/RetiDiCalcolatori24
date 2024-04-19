#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct Iter_node{
    void* pointer;
    struct Iter_node* next;
}Iter_node;

typedef struct Iter{
    Iter_node* pointers;
    int number_of_remaining_element;
}Iter;

Iter* iter_new();

//Iter_node* iter_append_element_aux(Iter_node* node, Iter_node* new_node);
void iter_append_element(Iter* iter, void* pointer);

void* iter_next(Iter* iter);

void iter_delete(Iter* iter);
int iter_remaining_elem(Iter* iter);

void print_iter(Iter* iter);