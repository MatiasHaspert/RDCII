#pragma once

#include <sys/types.h>
#include "server.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void close_fd(int fd, const char *label);
ssize_t safe_dprintf(int fd, const char *format, ...);
