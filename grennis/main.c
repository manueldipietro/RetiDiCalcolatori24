#include "expirer_manager.h"
#include "server.h"
#include "tdbms.h"

#define PORT            7379
#define MAX_CONNECTION  10


int main(int argc, char* argv[]){
    //Creo il TDBMS
    TDBMS* tdbms = tdbms_new();

    //Creo l'expirer manager
    Expirer_manager* expirer_manager = expirer_manager_new(tdbms);
    
    //Creo il server
    server_new(PORT, MAX_CONNECTION, tdbms, expirer_manager);
}