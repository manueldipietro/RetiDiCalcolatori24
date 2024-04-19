#include "scl_ts.h"

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
    ret = sem_post(&(list->writer_sem));\
    if(ret < 0){\
        adt_utils_print_error(__FILE__, __LINE__, __func__, "Unable to release read resource");\
        return return_on_error;\
    }\
}


SclTS* sclTS_new(){
    SclTS* list = malloc(sizeof(SclTS));

    list->unsafe_list = scl_new();
    list->reader_number = 0;
    sem_init(&(list->reader_sem), 0, 1);
    sem_init(&(list->writer_sem), 0, 1);
    return list;
}

void sclTS_append(SclTS* list, void* T){
    WRITE_BLK(list,);
    scl_append(list->unsafe_list, T);
    WRITE_RLS(list,);
    return;
}

int sclTS_element_number(SclTS* list){
    int element_number;
    READ_BLK(list, -1);
    element_number = scl_element_number(list->unsafe_list);
    READ_RLS(list, -1);
    return element_number;    
}

void* sclTS_get(SclTS* list){
    void* getted;
    READ_BLK(list, NULL);
    getted = scl_get(list->unsafe_list);
    READ_RLS(list, NULL);
    return getted;
}

void* sclTS_remove(SclTS* list){
    void* getted;
    WRITE_BLK(list, NULL);
    getted = scl_remove(list->unsafe_list);
    WRITE_RLS(list, NULL);
    return getted;
}

void sclTS_remove_andfreeinfo(SclTS* list, void (*T_destroyer) (void*)){
    WRITE_BLK(list, );
    scl_remove_andfreeinfo(list->unsafe_list, T_destroyer);
    WRITE_RLS(list, );
    return;
}

void sclTS_delete(SclTS* list){
    scl_delete(list->unsafe_list);
    sem_destroy(&(list->reader_sem));
    sem_destroy(&(list->writer_sem));
    free(list);
    return;
}

void sclTS_delete_andfreeinfo(SclTS* list, void (*T_destroyer) (void*)){
    scl_delete_andfreeinfo(list->unsafe_list, T_destroyer);
    sem_destroy(&(list->reader_sem));
    sem_destroy(&(list->writer_sem));
    free(list);
    return;
}

void* sclTS_q(SclTS* list, void* T, int (*compar) (const void*, const void*)){
    void* finded;
    READ_BLK(list, NULL);
    finded = scl_q(list->unsafe_list, T, compar);
    READ_RLS(list, NULL);
    return finded;
}

void* sclTS_qr(SclTS* list, void* T, int (*compar) (const void*, const void*)){
    void* finded;
    WRITE_BLK(list, NULL);
    finded = scl_qr(list->unsafe_list, T, compar);
    WRITE_RLS(list, NULL);
    return finded;
}

void sclTS_qr_andfreeinfo(SclTS* list, void* T, int (*compar) (const void*, const void*), void (*T_destroyer) (void*)){
    WRITE_BLK(list, );
    scl_qr_andfreeinfo(list->unsafe_list, T, compar, T_destroyer);
    WRITE_RLS(list, );
    return;
}

void sclTS_p_winfo(SclTS* list, void (*print_T)(void*)){
    printf("Indirizzo lista bloccante: %p\n", list);
    READ_BLK(list, );
    scl_p_winfo(list->unsafe_list, print_T);
    READ_RLS(list, );
    return;
}

void sclTS_p(SclTS* list){
    printf("Indirizzo lista bloccante: %p\n", list);
    READ_BLK(list, );
    scl_p(list->unsafe_list);
    READ_RLS(list, );
    return;
}

Iter* sclTS_get_new_iter(SclTS* list){
    Iter* iter;
    READ_BLK(list, NULL);
    iter = scl_get_new_iter(list->unsafe_list);
    READ_RLS(list, NULL);
    return iter;
}