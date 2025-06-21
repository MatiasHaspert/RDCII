#pragma once

#include <netinet/in.h>
#include "utils.h"
#include "config.h"
#include "serverpi.h"
#include "session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

extern int client_fd;

int server_init(const char *ip, int port);
int server_accept(int server_socket, struct sockaddr_in *client_addr);
void server_set_listen_fd(int fd);
void server_shutdown(void);
void server_loop(int client_fd);
