/**
 * Author: Manuel Di Pietro
 * Date: 08-03-2024
 * Program to manage a small TBDMS
 * Assumption: Equal values cannot be entered in key fields.
*/

#include <stdio.h>
#include "STDBMS.h"

/*
    AUXILIARY FUNCTIONS
*/

void print_tree(IndexNode * root) {
    if (root == NULL) {
        return;
    }
    print_tree(root->left);
    if(root->valueType == INT) printf("%d ", *(root->value.number));
    if(root->valueType == STRING) printf("%s ", root->value.string);
    print_tree(root->right);
}

void printPersona(Persona* persona){
    if(persona == NULL){
        printf("NON TROVATA\n");
        return;
    }
    printf("nome: %s, cognome: %s, indirizzo: %s, età: %d\n", persona->name, persona->surname, persona->address, persona->age);
    return;
}

/*
    Main test function
*/
int main(){
    //Generazione istanze persone
    Persona persona01 = {"Nome01", "Cognome01", "Via01", 1};
    Persona persona02 = {"Nome02", "Cognome02", "Via02", 2};
    Persona persona03 = {"Nome03", "Cognome03", "Via03", 3};
    Persona persona04 = {"Nome04", "Cognome04", "Via04", 4};
    Persona persona05 = {"Nome05", "Cognome05", "Via05", 5};
    Persona persona06 = {"Nome06", "Cognome06", "Via06", 6};
    Persona persona07 = {"Nome07", "Cognome07", "Via07", 7};
    Persona persona08 = {"Nome08", "Cognome08", "Via08", 8};
    Persona persona09 = {"Nome09", "Cognome09", "Via09", 9};
    Persona persona10 = {"Nome10", "Cognome10", "Via10", 10};
    Persona persona11 = {"Nome11", "Cognome11", "Via11", 11};
    Persona persona12 = {"Nome12", "Cognome12", "Via12", 12};
    Persona persona13 = {"Nome13", "Cognome13", "Via13", 13};
    Persona persona14 = {"Nome14", "Cognome14", "Via14", 14};
    Persona persona15 = {"Nome15", "Cognome15", "Via15", 15};

    //Creazione database
    Database db = {NULL, NULL, NULL, NULL};
    insert(&db, &persona01);
    insert(&db, &persona02);
    insert(&db, &persona03);
    insert(&db, &persona04);
    insert(&db, &persona05);
    insert(&db, &persona06);
    insert(&db, &persona07);
    insert(&db, &persona08);
    insert(&db, &persona09);
    insert(&db, &persona10);
    insert(&db, &persona11);
    insert(&db, &persona12);
    insert(&db, &persona13);
    insert(&db, &persona14);
    insert(&db, &persona15);

    /*
    //Stampa alberi
    print_tree(db.name);
    printf("COEFBIL: %d\n", getBalance(db.name));
    print_tree(db.surname);
    printf("COEFBIL: %d\n", getBalance(db.name));
    print_tree(db.address);
    printf("COEFBIL: %d\n", getBalance(db.name));
    print_tree(db.age);
    printf("COEFBIL: %d\n", getBalance(db.name));
    */

    //Alcuni test di funzionamento
    printf("TEST 01:\n");
    printf("\tRicerca per nome: ");
    Persona* trovata1 = findByName(&db, "Nome01");
    printPersona(trovata1);
    printf("\tRicerca per cognome: ");
    Persona * trovata2 = findBySurname(&db, "Cognome01");
    printPersona(trovata2);
    printf("\tRicerca per indirizzo: ");
    Persona * trovata3 = findByAddress(&db, "Via01");
    printPersona(trovata3);
    printf("\tRicerca per età: ");
    Persona * trovata4 = findByAge(&db, 1);
    printPersona(trovata4);
    printf("\n");

    printf("TEST 02:\n");
    printf("\tRicerca per nome: ");
    trovata1 = findByName(&db, "Non esiste");
    printPersona(trovata1);
    printf("\tRicerca per cognome: ");
    trovata2 = findBySurname(&db, "Non esiste");
    printPersona(trovata2);
    printf("\tRicerca per indirizzo: ");
    trovata3 = findByAddress(&db, "Non esiste");
    printPersona(trovata3);
    printf("\tRicerca per età: ");
    trovata4 = findByAge(&db, 100);
    printPersona(trovata4);
    printf("\n");

    printf("TEST 03:\n");
    printf("\tRicerca per nome: ");
    trovata1 = findByName(&db, "Nome07");
    printPersona(trovata1);
    printf("\tRicerca per cognome: ");
    trovata2 = findBySurname(&db, "Cognome07");
    printPersona(trovata2);
    printf("\tRicerca per indirizzo: ");
    trovata3 = findByAddress(&db, "Via07");
    printPersona(trovata3);
    printf("\tRicerca per età: ");
    trovata4 = findByAge(&db, 7);
    printPersona(trovata4);
    printf("\n");

    return 0;
}
