/**
 * Author: Manuel Di Pietro
 * Date: 08-03-2024
 * Program to manage a small TDBMS
 * Assumption: Equal values cannot be entered in key fields.
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "STDBMS.h"

/**
 * Function for generating an AVL tree and for create the database.
 * The AVL tree allows for better performance in both insertion and reading O(logN)
 * Some functions were taken/inspired from:
 * //https://www.sanfoundry.com/c-program-create-balanced-binary-tree-incoming-data/
*/

/*
    Function that compares the values of two indexNodes
*/
int IndexComparator(IndexNode* node1, IndexNode* node2){
    if(node1->valueType != node2->valueType) fatalErrorManager("Compare two incompatible types! Execution will be stopped.");
    if(node1->valueType == INT){
        if( *(node1->value.number) == *(node2->value.number) ) return 0;
        if( *(node1->value.number) < *(node2->value.number) ) return -1;
        return 1;
    }
    if(node1->valueType == STRING){
        return strcmp(node1->value.string, node2->value.string);
    }
    fatalErrorManager("Fatal error during comparation! Execution will be stopped.");
}

/*
    Recursive function that returns the height of a subtree starting from its root
*/
int height(IndexNode* node){
    if(node == NULL)
        return 0;
    int h_left = height(node->left);
    int h_right = height(node->right);
    return h_left > h_right ? h_left+1 : h_right+1;
}

/*
    Function that performs a right rotation, used in AVL restructuring
*/
IndexNode* rightRotate(IndexNode* y){
    IndexNode* x = y->left;
    IndexNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    return x;
}

/*
    Function that performs a left rotation, used in AVL restructuring
*/
IndexNode* leftRotate(IndexNode* x){
    IndexNode* y = x->right;
    IndexNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    return y;
}

/*
    Function that returns the balancing coefficient of a subtree given its root node
*/

int getBalance(IndexNode* node){
    if(node  == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

/*
    Function that inserts a new IndexNode into an AVL IndexTree,
    generates a fatal error in case of duplicate values.
    Returned refer reference to the restructured tree
*/
IndexNode* insertIndex(IndexNode* root, IndexNode* toInsert){
    if(root==NULL){
        return toInsert;
    }
    int cmpRes = IndexComparator(root, toInsert);
    if( cmpRes == 0) fatalErrorManager("Fatal error during insert! Collision between indices. Execution will be stopped.");    
    if( cmpRes < 0){
        root->right = insertIndex( root->right, toInsert);
    }
    else root->left=insertIndex(root->left, toInsert);

    int balance = getBalance(root);

    //Left Left Case
    if(balance > 1 && IndexComparator(root, root->left) > 0){
        return rightRotate(root);
    }

    //Right Right Case
    if(balance < -1 && IndexComparator(root, root->right) < 0){
        return leftRotate(root);
    }

    //Left Right Case
    if(balance > 1 && IndexComparator(root, root->left) > 0){
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    //Right Left Case
    if(balance < -1 && IndexComparator(root, root->right) < 0){
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

/*
    Function that create a new IndexNode
*/
IndexNode* createIndexNode(NodeType valueType, genericValue value){
    IndexNode* node = malloc(sizeof(IndexNode));
    node->valueType = valueType;
    if(node->valueType == INT) node->value.number = value.number;
    else node->value.string = value.string;
    node->left = NULL;
    node->right = NULL;
    return node;
}

/*
    Function that inputs a person in the tree database
*/
void insert(Database * database, Persona * persona){
    database->name = insertIndex(database->name, createIndexNode(STRING, (genericValue) persona->name) );
    database->surname = insertIndex( database->surname, createIndexNode(STRING, (genericValue) persona->surname) );
    database->address = insertIndex( database->address, createIndexNode(STRING, (genericValue) persona->address) );
    database->age = insertIndex( database->age, createIndexNode(INT, (genericValue) &persona->age) );
    
    return;
}


/*
    Function that searches for an index. It independently manages polymorphism.
*/
IndexNode* findIndex(IndexNode* root, genericValue value){
    if(root == NULL) return NULL;
    if(root->valueType == STRING){
        if(strcmp( root->value.string, value.string) == 0) return root;
        if(strcmp( root->value.string, value.string) < 0)
            return findIndex(root->right, value);
        return findIndex(root->left, value);
    }
    if(root->valueType == INT){
        if(root == NULL) return NULL;
        if( *(root->value.number) == *(value.number) ) return root;
        if( *(root->value.number) < *(value.number) )
            return findIndex(root->right, value);
        return findIndex(root->left, value);        
    }
    fatalErrorManager("Unrecognized Index Type! Execution will be stopped!");
}

/*
    Function that searches by a name into an index tree and return Persona.
*/
Persona* findByName(Database* database, char* name){
    if(name == NULL) fatalErrorManager("Stringa non valida");
    IndexNode* root = database->name;
    IndexNode* finded = findIndex(root, (genericValue) name);
    if(finded != NULL)
        return (Persona*) ( (void*) finded->value.string - offsetof(struct Persona, name)); //Sfrutto la macro offsetof(st, m);
    return NULL;
}

/*
    Function that searches by a surname into an index tree and return Persona.
*/
Persona* findBySurname(Database* database, char* surname){
    if(surname == NULL) fatalErrorManager("Stringa non valida");
    IndexNode* root = database->surname;
    IndexNode* finded = findIndex(root, (genericValue) surname);
    if(finded != NULL)
        return (Persona*) ( (void*) finded->value.string - offsetof(struct Persona, surname)); //Sfrutto la macro offsetof(st, m);
    return NULL;
}

/*
    Function that searches by an address into an index tree and return Persona.
*/
Persona* findByAddress(Database* database, char* address){
    if(address == NULL) fatalErrorManager("Stringa non valida");
    IndexNode* root = database->address;
    IndexNode* finded = findIndex(root, (genericValue) address);
    if(finded != NULL)
        return (Persona*) ( (void*) finded->value.string - offsetof(struct Persona, address)); //Sfrutto la macro offsetof(st, m);
    return NULL;
}

/*
    Function that searches by an age into an index tree and return Persona.
*/
Persona* findByAge(Database* database, int age){
    if(age<0) fatalErrorManager("Età negativa");
    IndexNode * root = database->age;
    IndexNode * finded = findIndex(root, (genericValue) &age);
    if(finded != NULL)
        return (Persona *) ( (void*) finded->value.number - offsetof(struct Persona, age));
    return NULL;
}


/**
 * functions that free the memory occupied by the database and its substructures
*/

/*
    Function to free an IndexTree
*/
void freeTree(IndexNode* node){
    if(node->left != NULL)
        freeTree(node->left);
    if(node->right != NULL)
        freeTree(node->right);
    free(node);
    return;
}

/*
    Function to free a database
*/
void freeDatabase(Database* database){
    freeTree(database->name);
    freeTree(database->surname);
    freeTree(database->address);
    freeTree(database->age);
    return;
}


/**
 * Error handler function
*/

/*
    Function to handle a fatal error, which compromises execution.
    The program will print an error and stop execution.
*/
void fatalErrorManager(const char* message){
    perror(message);
    exit(-1);   
}

/*
    Function to handle an error. The program will print an error,
    the response action is handled by the calling function.
*/
void errorManager(const char* message){
    perror(message);
    return;
}
