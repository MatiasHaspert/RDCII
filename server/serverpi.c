#include "serverpi.h"
#include "server.h"

/* 
Esta funcion recibe un comando del cliente en socketDescriptor.
En operation obtengo los comandos ftp.
*/
int recv_cmd(int socketDescriptor, char *operation, char *param) {
    char buffer[BUFSIZE];
    char *token;

    if (recv(socketDescriptor, buffer, BUFSIZE, 0) < 0) {
        fprintf(stderr,"Error al recibir la informacion");
        return EXIT_FAILURE;
    }

    buffer[strcspn(buffer, "\r\n")] = 0;
    token = strtok(buffer, " ");

    if (token == NULL || strlen(token) < 4) {
        fprintf(stderr,"Comando ftp invalido");
    } else {
        strcpy(operation, token);
        token = strtok(NULL, " ");
        #if DEBUG 
        printf("par %s\n", token);
        #endif
        if (token != NULL) strcpy(param, token);
    }
}
