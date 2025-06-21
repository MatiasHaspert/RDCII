#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>

#include "serverdtp.h"
#include "serverpi.h"

#define MAXPENDING 5
#define PORTDEFAULT 21

#endif