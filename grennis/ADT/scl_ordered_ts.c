#include "scl_ordered_ts.h"

/**
 *  Macro that defines the block of code that manages incoming
 *  concurrency for read-only access to the data structure
*/
#define READ_BLK(list, return_on_error){\
    int ret;\
    \
    ret = sem_wait(&(list->writer_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Error: Unable to lock write resource");\
        return return_on_error;\
    }\
    \
    ret = sem_wait(&(list->reader_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Error: Unable to lock read resource");\
        return return_on_error;\
    }\
    \
    list->reader_number += 1;\
    \
    ret = sem_post(&(list->reader_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
        return return_on_error;\
    }\
    \
    ret = sem_post(&(list->writer_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
        return return_on_error;\
    }\
    \
}

/**
 *  Macro that defines the block of code that manages outgoing
 *  concurrency for read-only access to the data structure
*/
#define READ_RLS(list, return_on_error){\
    int ret = sem_wait(&(list->reader_sem));\
    \
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Error: Unable to lock read resource");\
        return return_on_error;\
    }\
    \
    list->reader_number -= 1;\
    \
    ret = sem_post(&(list->reader_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
        return return_on_error;\
    }\
}

/**
 *  Macro that defines the block of code that manages incoming
 *  concurrency for write access to the data structure
*/
#define WRITE_BLK(list, return_on_error){\
    int ret;\
    \
    ret = sem_wait(&(list->writer_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Error: Unable to lock write resource");\
        return return_on_error;\
    }\
    \
    int exit = 0;\
    while(!exit){\
        ret = sem_wait(&(list->reader_sem));\
        if(ret < 0){\
            adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to lock read resource");\
            return return_on_error;\
        }\
        \
        if(list->reader_number == 0)\
            exit = 1;\
        \
        ret = sem_post(&(list->reader_sem));\
        if(ret < 0){\
            adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
            return return_on_error;\
        }\
        \
    }\
}

/**
 *  Macro that defines the block of code that manages outcoming
 *  concurrency for write access to the data structure
*/
#define WRITE_RLS(list, return_on_error){\
    int ret;\
    \
    ret = sem_post(&(list->reader_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
        return return_on_error;\
    }\
}

//OK!
SclTS_ordered* sclTS_ordered_new(int (*compar)(const void*, const void*)){
    SclTS_ordered* list = malloc(sizeof(SclTS_ordered));

    list->unsafe_list = scl_ordered_new(compar);
    list->reader_number = 0;
    sem_init(&(list->reader_sem), 0, 1);
    sem_init(&(list->writer_sem), 0, 1);
    return list;
}

void sclTS_ordered_append(SclTS_ordered* list, void* T){
    WRITE_BLK(list,);
    scl_ordered_append(list->unsafe_list, T);
    WRITE_RLS(list,);
    return;
}

int sclTS_ordered_element_number(SclTS_ordered* list){
    int element_number;
    READ_BLK(list, -1);
    element_number = scl_ordered_element_number(list->unsafe_list);
    READ_RLS(list, -1);
    return element_number;    
}

void* sclTS_ordered_get(SclTS_ordered* list){
    void* getted;
    READ_BLK(list, NULL);
    getted = scl_ordered_get(list->unsafe_list);
    READ_RLS(list, NULL);
    return getted;
}

void* sclTS_ordered_remove(SclTS_ordered* list){
    void* getted;
    WRITE_BLK(list, NULL);
    getted = scl_ordered_remove(list->unsafe_list);
    WRITE_RLS(list, NULL);
    return getted;
}

void sclTS_ordered_remove_andfreeinfo(SclTS_ordered* list, void (*T_destroyer) (void*)){
    WRITE_BLK(list, );
    scl_ordered_remove_andfreeinfo(list->unsafe_list, T_destroyer);
    WRITE_RLS(list, );
    return;
}

void sclTS_ordered_delete(SclTS_ordered* list){
    scl_ordered_delete(list->unsafe_list);
    sem_destroy(&(list->reader_sem));
    sem_destroy(&(list->writer_sem));
    free(list);
    return;
}

void sclTS_ordered_delete_andfreeinfo(SclTS_ordered* list, void (*T_destroyer) (void*)){
    scl_ordered_delete_andfreeinfo(list->unsafe_list, T_destroyer);
    sem_destroy(&(list->reader_sem));
    sem_destroy(&(list->writer_sem));
    free(list);
    return;
}

void* sclTS_ordered_q(SclTS_ordered* list, void* T, int (*compar) (const void*, const void*)){
    void* finded;
    READ_BLK(list, NULL);
    finded = scl_ordered_q(list->unsafe_list, T, compar);
    READ_RLS(list, NULL);
    return finded;
}

void* sclTS_ordered_qr(SclTS_ordered* list, void* T, int (*compar) (const void*, const void*)){
    void* finded;
    WRITE_BLK(list, NULL);
    finded = scl_ordered_qr(list->unsafe_list, T, compar);
    WRITE_RLS(list, NULL);
    return finded;
}

void sclTS_ordered_qr_andfreeinfo(SclTS_ordered* list, void* T, int (*compar) (const void*, const void*), void (*T_destroyer) (void*)){
    WRITE_BLK(list, );
    scl_ordered_qr_andfreeinfo(list->unsafe_list, T, compar, T_destroyer);
    WRITE_RLS(list, );
    return;
}

void sclTS_ordered_p_winfo(SclTS_ordered* list, void (*print_T)(void*)){
    printf("Indirizzo lista bloccante: %p\n", list);
    READ_BLK(list, );
    scl_ordered_p_winfo(list->unsafe_list, print_T);
    READ_RLS(list, );
    return;
}

void sclTS_ordered_p(SclTS_ordered* list){
    printf("Indirizzo lista bloccante: %p\n", list);
    READ_BLK(list, );
    scl_ordered_p(list->unsafe_list);
    READ_RLS(list, );
    return;
}

Iter* sclTS_ordered_get_new_iter(SclTS_ordered* list){
    Iter* iter;
    READ_BLK(list, NULL);
    iter = scl_ordered_get_new_iter(list->unsafe_list);
    READ_RLS(list, NULL);
    return iter;
}