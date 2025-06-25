#include "arguments.h"
#include "server.h"
#include "utils.h"
#include "signals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

int main(int argc, char **argv)
{
  struct arguments args;

  if (parse_arguments(argc, argv, &args) != 0){
    return EXIT_FAILURE;
  }

  int server_socket = server_init(args.address, args.port);
  if (server_socket < 0){
    return EXIT_FAILURE;
  }

  setup_signals();

  while (1){
    struct sockaddr_in client_addr;
    int listen_fd = server_accept(server_socket, &client_addr);
    if (listen_fd < 0)
      continue;

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("Conexion desde %s:%d acceptada\n", client_ip, ntohs(client_addr.sin_port));

    server_loop(listen_fd);

    printf("Conexion desde %s:%d cerrada\n", client_ip, ntohs(client_addr.sin_port));
  }

  // NUNCA SE LLEGA AQUÍ
  close_fd(server_socket, "socket maestro");

  // https://en.cppreference.com/w/c/program/EXIT_status
  return EXIT_SUCCESS;
}
