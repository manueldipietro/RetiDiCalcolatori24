#pragma once

typedef struct Persona{
    char name[20];
    char surname[50];
    char address[100];
    int age;
} Persona;

/**
 * Enum used to identify the node type
*/
typedef enum{STRING, INT} NodeType;

/**
 * Union used to handle polymorphism
*/
typedef union{
    char* string;
    int* number;
}genericValue;

/**
 * IndexNode: version with polymorphism
 * Used for store an index used for value
*/
typedef struct IndexNode{
    NodeType valueType;
    genericValue value;
    struct IndexNode* left;
    struct IndexNode* right;
}IndexNode;

/**
 * Struct used for store root node of indexTrees
*/
typedef struct Database{
    IndexNode * name;
    IndexNode * surname;
    IndexNode * address;
    IndexNode * age;
} Database;

int IndexComparator(IndexNode* node1, IndexNode* node2);
int height(IndexNode* node);

IndexNode* rightRotate(IndexNode* y);
IndexNode* leftRotate(IndexNode* x);
int getBalance(IndexNode* node);
IndexNode* insertIndex(IndexNode* root, IndexNode* toInsert);
IndexNode* createIndexNode(NodeType valueType, genericValue value);
void insert(Database * database, Persona * persona);

void freeTree(IndexNode* node);
void freeDatabase(Database* database);

IndexNode* findIndex(IndexNode* root, genericValue value);
Persona* findByName(Database* database, char* name);
Persona* findBySurname(Database* database, char* surname);
Persona* findByAddress(Database* database, char* address);
Persona* findByAge(Database* database, int age);

void fatalErrorManager(const char* message);
void errorManager(const char* message);
