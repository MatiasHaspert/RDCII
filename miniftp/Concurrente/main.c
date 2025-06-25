#include "arguments.h"
#include "server.h"
#include "utils.h"
#include "signals.h"
#include <stdio.h>
#include <stdlib.h>     // EXIT_*
#include <string.h>
#include <unistd.h>     // close()
#include <arpa/inet.h>  // inet_ntoa()
#include <errno.h>
#include <signal.h>

int main(int argc, char **argv) {
  struct arguments args;

  if (parse_arguments(argc, argv, &args) != 0)
    return EXIT_FAILURE;

  printf("Servidor levantado en %s:%d\n", args.address, args.port);

  int listen_fd = server_init(args.address, args.port);
  if (listen_fd < 0)
    return EXIT_FAILURE;

  setup_signals();

  while(1) {
    struct sockaddr_in client_addr;

    memset(&client_addr, 0, sizeof(client_addr));
    int new_socket = server_accept(listen_fd, &client_addr);
    if (new_socket < 0) {
      continue;
    }

    pid_t pid = fork();
    if (pid < 0) {
      perror("fork");
      close_fd(new_socket, "cliente (falló fork)");
      continue;
    }

    if (pid == 0) {
      // Proceso hijo

      // Unirse al PGID del padre
      pid_t pgid = getpgrp();  // PGID del padre
      if (setpgid(0, pgid) < 0) {
        perror("setpgid hijo");
      }
      printf("Hijo PID %d PGID %d\n", getpid(), getpgrp());

      setup_child_signals();

      close(listen_fd); // No necesita el socket de escucha

      char client_ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
      printf("[+] Nueva conexión desde %s:%d atendida por el hijo PID %d\n", client_ip, ntohs(client_addr.sin_port), getpid());

      server_loop(new_socket); // Cada hijo configura su propia sesión

      printf("[-] Hijo PID %d cerrando conexión de %s:%d\n", getpid(), client_ip, ntohs(client_addr.sin_port));

      exit(EXIT_SUCCESS);
    } else {
      // Proceso padre
      close_fd(new_socket, "socket de cliente");  // importante para evitar fugas de sockets
    }
  }

  // NUNCA SE LLEGA AQUÍ
  close_fd(listen_fd, "socket de escucha");

  return EXIT_SUCCESS;
}
