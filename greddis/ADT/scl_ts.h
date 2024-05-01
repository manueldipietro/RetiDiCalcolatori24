#pragma once

#include <semaphore.h>

#include "scl.h"
#include "adt_utility.h"

typedef struct SclTS{
    Scl* unsafe_list;
    int reader_number;
    sem_t reader_sem;
    sem_t writer_sem;
}SclTS;

SclTS* sclTS_new();

void sclTS_append(SclTS* list, void* T);

int sclTS_element_number(SclTS* list);
void* sclTS_get(SclTS* list);
void* sclTS_remove(SclTS* list);

void sclTS_remove_andfreeinfo(SclTS* list, void (*T_destroyer) (void*));

void sclTS_delete(SclTS* list);
void sclTS_delete_andfreeinfo(SclTS* list, void (*T_destroyer) (void*));

void* sclTS_q(SclTS* list, void* T, int (*compar) (const void*, const void*));

void* sclTS_qr(SclTS* list, void* T, int (*compar) (const void*, const void*));
void sclTS_qr_andfreeinfo(SclTS* list, void* T, int (*compar) (const void*, const void*), void (*T_destroyer) (void*));

void sclTS_p_winfo(SclTS* list, void (*print_T)(void*));
void sclTS_p(SclTS* list);

Iter* sclTS_get_new_iter(SclTS* list);