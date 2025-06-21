#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>   // para pid_t


#include "session.h"    // para current_sess
#include "utils.h"      // para close_fd()

void setup_signals(void);
void reset_signals(void);
