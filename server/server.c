#include "server.h"


int main(int argc, char **argv){

    int port;
    int masterSocket, slaveSocket;
    struct sockaddr_in masterAddr, slaveAddr;
    socklen_t lSlaveAddr;
    char user_name[BUFSIZE];
    char user_pass[BUFSIZE];
    char buffer[BUFSIZE];
    char command[BUFSIZE];
    int data_len;

    if (argc > 2) {
        fprintf(stderr, "Uso: %s [puerto]\n", argv[0]);
        exit(EXIT_FAILURE);
    } else if (argc == 2) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Puerto inválido. Usando puerto por defecto (%d).\n", PORTDEFAULT);
            port = PORTDEFAULT;
        }
    }else{
        port = PORTDEFAULT;
    }
    
    masterAddr.sin_family = AF_INET;
    masterAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    masterAddr.sin_port = htons(port);

    if((masterSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    
    if (bind(masterSocket, (struct sockaddr *) &masterAddr, sizeof(masterAddr)) < 0){
        fprintf(stderr, "bind() fallo!");
        close(masterSocket);
        exit(EXIT_FAILURE);
    }
  

    if (listen(masterSocket, MAXPENDING) < 0){
        fprintf(stderr, "listen() fallo!");
        close(masterSocket);
        exit(EXIT_FAILURE);
    }

    printf("Servidor FTP escuchando en el puerto %d...\n", port);

    while(1){
        lSlaveAddr = sizeof(slaveAddr);
        if ((slaveSocket=accept(masterSocket,(struct sockaddr *) &slaveAddr, &lSlaveAddr)) < 0){
            fprintf(stderr, "Error al aceptar un cliente");
        }

    
        if (send(slaveSocket, MSG_220, sizeof(MSG_220) - 1, 0) != sizeof(MSG_220) - 1) {
            close(slaveSocket);
            fprintf(stderr, "Error no se pudo enviar el mensaje.\n");
            break;
        }
        
        if (recv_cmd(slaveSocket, command, user_name) != 0) {
            close(slaveSocket);
            fprintf(stderr, "Error: no se pudo recibir el comando USER.\n");
            break;
        }

        if (strcmp(command, "USER") != 0) {
            close(slaveSocket);
            fprintf(stderr, "Error: se esperaba el comando USER.\n");
            continue;
        }

        data_len = snprintf(buffer, BUFSIZE, MSG_331, user_name);

        if (send(slaveSocket, buffer, data_len, 0) < 0) {
            close(slaveSocket);
            fprintf(stderr, "Error: no se pudo enviar el mensaje MSG_331.\n");
            break;
        }
        
        if (recv_cmd(slaveSocket, command, user_pass) != 0) {
            close(slaveSocket);
            fprintf(stderr, "Error: no se pudo recibir el comando PASS.\n");
            break;
        }

        if (strcmp(command, "PASS") != 0) {
            close(slaveSocket);
            fprintf(stderr, "Error: se esperaba el comando PASS.\n");
            continue;
        }

        if (!check_credentials(user_name, user_pass)) {
            data_len = snprintf(buffer, BUFSIZE, MSG_530);
            if (send(slaveSocket, buffer, data_len, 0) < 0) {
                close(slaveSocket);
                fprintf(stderr, "Error: no se pudo enviar el mensaje MSG_530.\n");
                break;
            }
            close(slaveSocket);
            continue;
        }

        data_len = snprintf(buffer, BUFSIZE, MSG_230, user_name);
        if (send(slaveSocket, buffer, data_len, 0) < 0) {
            close(slaveSocket);
            fprintf(stderr, "Error: no se pudo enviar el mensaje MSG_230.\n");
            break;
        }

        while (1) {
            if (recv_cmd(slaveSocket, command, buffer) != 0) {
                close(slaveSocket);
                fprintf(stderr, "Error: no se pudo recibir el comando.\n");
                break;
            }
            if (strcmp(command, "QUIT") == 0) {
                if (send(slaveSocket, MSG_221, sizeof(MSG_221) - 1, 0) < 0) {
                    close(slaveSocket);
                    fprintf(stderr, "Error: no se pudo enviar el mensaje MSG_221.\n");
                    break;
                }
                close(slaveSocket);
                break;
            }
            if (strcmp(command, "SYST") == 0) {
                if (send(slaveSocket, MSG_502, sizeof(MSG_502) - 1, 0) < 0) {
                    close(slaveSocket);
                    fprintf(stderr, "Error: no se pudo enviar el mensaje SYST.\n");
                    break;
                }
                continue;
            }
        
        }

        
    }

    close(masterSocket);
    return EXIT_SUCCESS;
}