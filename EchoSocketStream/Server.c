#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>             // para close()
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define RCVBUFSIZE 512

int main() {
    struct sockaddr_in addr, caddr;
    int sh;
    int clientSock, msglen;
    char msg[RCVBUFSIZE];

    // Crear socket
    if ((sh = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket failed");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr)); // limpiar struct
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // Cualquier IP local
    addr.sin_port = htons(3333); // Puerto local

    // Enlazar
    if (bind(sh, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind() failed");
        close(sh);
        exit(1);
    }

    // Escuchar
    if (listen(sh, 5) < 0) {
        perror("listen() failed");
        close(sh);
        exit(1);
    }

    printf("Servidor escuchando en puerto 3333...\n");

    while (1) {
        socklen_t clntLen = sizeof(caddr);  // Usar struct correcto
        if ((clientSock = accept(sh, (struct sockaddr *) &caddr, &clntLen)) < 0) {
            perror("accept() failed");
            continue;
        }

        printf("Cliente conectado\n");

        if ((msglen = recv(clientSock, msg, RCVBUFSIZE, 0)) < 0) {
            perror("recv() failed");
            close(clientSock);
            continue;
        }

        while (msglen > 0) {
            if (send(clientSock, msg, msglen, 0) != msglen) {
                perror("send() failed");
                break;
            }

            if ((msglen = recv(clientSock, msg, RCVBUFSIZE, 0)) < 0) {
                perror("recv() failed");
                break;
            }
        }

        printf("Cliente desconectado\n");
        close(clientSock);
    }

    close(sh);
    return 0;
}
