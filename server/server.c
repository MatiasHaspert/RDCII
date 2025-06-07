#include "server.h"


#define VERSION "1.0"
#define PORTDEFAULT 21
#define PWDFILE "/etc/ausftp/ftpusers" // Direccion del archivo donde se encuentran los usuarios

int main(int argc, char **argv){

    int port;
    int masterSocket, slaveSocket;
    struct sockaddr_in masterAddr, slaveAddr;
    socklen_t lSlaveAddr;

    if (argc > 2) {
        fprintf(stderr,"Numero de argumentos incorrectos");
    } else if (argc == 2) {
        if ((port = atoi(argv[1]) == 0)) {
            fprintf(stderr,"Numero de puerto invalido");
            exit(EXIT_FAILURE);
        }
    } else {
        port = PORTDEFAULT; // Puerto ftp por defecto
    }
    
    masterAddr.sin_family = AF_INET;
    masterAddr.sin_addr.s_addr = INADDR_ANY;
    masterAddr.sin_port = htons(PORTDEFAULT);

    if((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    /*
    if (bind(masterSocket, (struct sockaddr *) &masterAddr, sizeof(masterAddr)) < 0){
        fprintf(stderr, "bind() fallo!");
        exit(EXIT_FAILURE);
    }
    */
    bind(masterSocket, (struct sockaddr *) &masterAddr, sizeof(masterAddr));

    if (listen(masterSocket, MAXPENDING) < 0){
        fprintf(stderr, "listen() fallo!");
        exit(EXIT_FAILURE);
    }

    while(1){
        lSlaveAddr = sizeof(slaveAddr);
        if ((slaveSocket=accept(masterSocket,(struct sockaddr *) &slaveAddr, &lSlaveAddr)) < 0){
            fprintf(stderr, "Error al aceptar un cliente");
        }
        send(slaveSocket, "220 1", sizeof("220 1"), 0);
        printf("Funciono!");
    }

    close(masterSocket);
    return EXIT_SUCCESS;
}