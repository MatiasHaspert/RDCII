#pragma once

#include "config.h"
#include <string.h>
#include <argp.h>
#include <unistd.h>  // para geteuid()
#include <ifaddrs.h>
#include <netinet/in.h> // para INET_ADDRSTRLEN
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>      // para perror()

// Estructura de argumentos para contener las opciones analizadas
struct arguments {
  int port;
  int port_set;
  char address[INET_ADDRSTRLEN];
  int address_set;
};

// Analiza los argumentos de la línea de comandos en la estructura 'args' proporcionada.
// Devuelve 0 en caso de éxito o un valor distinto de cero en caso de error.
int parse_arguments(int argc, char **argv, struct arguments *args);