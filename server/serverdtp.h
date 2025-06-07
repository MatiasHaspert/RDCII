#ifndef SERVERDTP_H
#define SERVERDTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define PWDFILE "/etc/ausftp/ftpusers" // Direccion del archivo donde se encuentran los usuarios

bool check_credentials(char *, char *);
#endif
