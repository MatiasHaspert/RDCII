#pragma once

# include "config.h"
#include <netinet/in.h> // sockaddr_in
#include <string.h>
#include <unistd.h>

typedef struct {
  int control_sock;                // Conexión de control
  int data_sock;                   // Conexión de datos
  struct sockaddr_in data_addr;    // Para el comando PORT
  char current_user[USERNAME_MAX]; // Session username
  uint8_t logged_in;               // 0 = false, 1 = true
  int data_addr_set;               // 0 = no seteado, 1 = seteado, para validar si se ingreso PORT antes de un RETR o STOR
} ftp_session_t;

// Puntero global a la sesión actual (establecido por cada proceso hijo)
extern ftp_session_t *current_sess;

ftp_session_t *session_get(void);
void session_init(int control_fd);
void session_cleanup(void);
