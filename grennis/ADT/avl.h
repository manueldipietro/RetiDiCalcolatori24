#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct Avl_node{
    void* T;
    struct Avl_node* left;
    struct Avl_node* right;
}Avl_node;

typedef struct Avl{
    Avl_node* root;
}Avl;

Avl* avl_new();

int avl_height(Avl* tree);

int avl_get_balance(Avl* tree);

void* avl_insert(Avl* tree, void* T, int (*compar)(const void*, const void*));

void* avl_research(Avl* tree, void* T, int (*compar)(const void*, const void*));

void* avl_remove(Avl* tree, void* to_remove, int (*compar)(const void*, const void*));

void avl_delete(Avl* tree);

void avl_delete_andfreeinfo(Avl* tree, void (*T_destroyer) (void*));
