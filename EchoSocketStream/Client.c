#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>             // para close()
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define BUF 512

int main() {
    struct sockaddr_in addr; // address of the passive participant
    int sh;
    char msg[BUF] = "Hola mundo!!!\n";
    char msgrecv[BUF];
    int lmsgrecv;

    // Crear socket
    if ((sh = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket failed");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // IP del servidor
    addr.sin_port = htons(3333);

    // Conectar al servidor
    if (connect(sh, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("connect() failed");
        close(sh);
        exit(1);
    }

    // Enviar mensaje
    int msglen = strlen(msg);
    if (send(sh, msg, msglen, 0) != msglen) {
        perror("send() failed");
        close(sh);
        exit(1);
    }

    // Recibir respuesta
    if ((lmsgrecv = recv(sh, msgrecv, BUF - 1, 0)) < 0) {
        perror("recv failed");
        close(sh);
        exit(1);
    }

    msgrecv[lmsgrecv] = '\0'; // Asegurar terminación nula
    printf("Respuesta del servidor: %s\n", msgrecv);

    close(sh);
    return 0;
}
