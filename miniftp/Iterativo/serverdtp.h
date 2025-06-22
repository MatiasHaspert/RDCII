#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "session.h"

#define PWDFILE "/etc/ausftp/ftpusers"

int check_credentials(char *user, char *pass);
int dtp_send_file(ftp_session_t *sess, const char *filename);
int dtp_receive_file(ftp_session_t *sess, const char *filename);