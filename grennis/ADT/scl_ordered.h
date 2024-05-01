#pragma once
#include <stdlib.h>

#include "scl.h"

typedef struct Scl_ordered{
    Scl* scl;
    int (*compar)(const void*, const void*);
}Scl_ordered;

Scl_ordered* scl_ordered_new(int (*compar)(const void*, const void*));

void scl_ordered_append(Scl_ordered* list, void* T);

int scl_ordered_element_number(Scl_ordered* list);
void* scl_ordered_get(Scl_ordered* list);

void* scl_ordered_remove(Scl_ordered* list);
void scl_ordered_remove_andfreeinfo(Scl_ordered* list, void (*T_destroyer) (void*));

void scl_ordered_delete(Scl_ordered* list);
void scl_ordered_delete_andfreeinfo(Scl_ordered* list, void (*T_destroyer) (void*));

void* scl_ordered_q(Scl_ordered* list, void* T, int (*compar) (const void*, const void*));
void* scl_ordered_qr(Scl_ordered* list, void* T, int (*compar) (const void*, const void*));
void scl_ordered_qr_andfreeinfo(Scl_ordered* list, void* T, int (*compar) (const void*, const void*), void (*T_destroyer) (void*));

void scl_ordered_p_winfo(Scl_ordered* list, void (*print_T)(void*));
void scl_ordered_p(Scl_ordered* list);

Iter* scl_ordered_get_new_iter(Scl_ordered* list);