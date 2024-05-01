#pragma once

#include <semaphore.h>

#include "scl_ordered.h"
#include "adt_utility.h"

typedef struct SclTS_ordered{
    Scl_ordered* unsafe_list;
    int reader_number;
    sem_t reader_sem;
    sem_t writer_sem;
}SclTS_ordered;


SclTS_ordered* sclTS_ordered_new(int (*compar)(const void*, const void*));
void sclTS_ordered_append(SclTS_ordered* list, void* T);
int sclTS_ordered_element_number(SclTS_ordered* list);
void* sclTS_ordered_get(SclTS_ordered* list);
void* sclTS_ordered_remove(SclTS_ordered* list);
void sclTS_ordered_remove_andfreeinfo(SclTS_ordered* list, void (*T_destroyer) (void*));
void sclTS_ordered_delete(SclTS_ordered* list);
void sclTS_ordered_delete_andfreeinfo(SclTS_ordered* list, void (*T_destroyer) (void*));
void* sclTS_ordered_q(SclTS_ordered* list, void* T, int (*compar) (const void*, const void*));
void* sclTS_ordered_qr(SclTS_ordered* list, void* T, int (*compar) (const void*, const void*));
void sclTS_ordered_qr_andfreeinfo(SclTS_ordered* list, void* T, int (*compar) (const void*, const void*), void (*T_destroyer) (void*));
void sclTS_ordered_p_winfo(SclTS_ordered* list, void (*print_T)(void*));
void sclTS_ordered_p(SclTS_ordered* list);

Iter* sclTS_ordered_get_new_iter(SclTS_ordered* list);