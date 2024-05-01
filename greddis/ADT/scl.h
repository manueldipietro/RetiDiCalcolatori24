#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "iterator.h"

struct Scl_node{
    void* T;
    struct Scl_node *next;
};
typedef struct Scl_node Scl_node;

typedef struct Scl{
    Scl_node* first_element;
    int element_number;
}Scl;

Scl* scl_new();

//Scl_node* scl_append_aux(Scl_node* node, void* T);
void scl_append(Scl* list, void* T);

int scl_element_number(Scl* list);
void* scl_get(Scl* list);
void* scl_remove(Scl* list);

void scl_remove_andfreeinfo(Scl* list, void (*T_destroyer) (void*));

void scl_delete(Scl* list);
void scl_delete_andfreeinfo(Scl* list, void (*T_destroyer) (void*));

//void* scl_q_aux(Scl_node* node, void* T, int (*compar) (const void*, const void*));
void* scl_q(Scl* list, void* T, int (*compar) (const void*, const void*));

//Scl_node* scl_qr_aux(Scl_node* node, void** findend, void* T, int (*compar) (const void*, const void*));
void* scl_qr(Scl* list, void* T, int (*compar) (const void*, const void*));

//Scl_node* scl_qr_andfreeinfo_aux(Scl_node* node, void* T, int (*compar) (const void*, const void*), void (*T_destroyer) (void*));
void scl_qr_andfreeinfo(Scl* list, void* T, int (*compar) (const void*, const void*), void (*T_destroyer) (void*));

//void scl_p_winfo_aux(Scl_node* node, void (*print_T)(void*));
void scl_p_winfo(Scl* list, void (*print_T)(void*));

//void scl_p_aux(Scl_node* node);
void scl_p(Scl* list);

//Iter_node* scl_get_new_iter_aux(Scl_node* node);
Iter* scl_get_new_iter(Scl* list);