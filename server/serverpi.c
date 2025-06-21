#include "serverpi.h"

int isValidCommand(const char *command) {
    int i = 0;
    while (valid_commands[i] != NULL) {
        if (strcmp(command, valid_commands[i]) == 0) {
            return arg_commands[i];
        }
        i++;
    }

    return -1;
}

/* 
Esta funcion recibe un comando del cliente en socketDescriptor.
En operation obtengo los comandos ftp.
*/
int recv_cmd(int socketDescriptor, char *operation, char *param) {
    char buffer[BUFSIZE];
    char *token;
    int argsNumber;

    if (recv(socketDescriptor, buffer, BUFSIZE, 0) < 0) {
        fprintf(stderr,"Error al recibir la informacion");
        return 1;
    }

    buffer[strcspn(buffer, "\r\n")] = 0;
    token = strtok(buffer, " ");
    if (token == NULL || strlen(token) < 3 || (argsNumber = isValidCommand(token)) < 0) {
        fprintf(stderr, "Error: comando no válido.\n");
        return 1;
    }

    if (token == NULL || strlen(token) < 4) {
        fprintf(stderr,"Comando ftp invalido");
    } 
    
    strcpy(operation, token);

    if (!argsNumber){
        return 0;
    }
    
    token = strtok(NULL, " ");
    #if DEBUG 
    printf("par %s\n", token);
    #endif
    if (token != NULL) {
        strcpy(param, token);
    } else {
        fprintf(stderr, "Error: se esperaba un argumento para el comando %s.\n", operation);
        return 1;
    }

    return 0;
}
