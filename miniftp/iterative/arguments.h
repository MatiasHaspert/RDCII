#pragma once

#include "config.h"
#include <string.h>
#include <argp.h>
#include <unistd.h>  // for geteuid()
#include <ifaddrs.h>
#include <netinet/in.h> // for INET_ADDRSTRLEN
#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>      // perror()

// Arguments struct to hold parsed options
struct arguments {
  int port;
  int port_set;
  char address[INET_ADDRSTRLEN];
  int address_set;
};

// Parses command line arguments into the provided 'args' struct.
// Returns 0 on success, or non-zero on error.
int parse_arguments(int argc, char **argv, struct arguments *args);
