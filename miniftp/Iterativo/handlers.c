#include "responses.h"
#include "serverpi.h"
#include "serverdtp.h"
#include "session.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void handle_USER(const char *args) {
  ftp_session_t *sess = session_get();

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501); // Error de sintaxis en el parametro
    return;
  }

  strncpy(sess->current_user, args, sizeof(sess->current_user) - 1);
  sess->current_user[sizeof(sess->current_user) - 1] = '\0';
  safe_dprintf(sess->control_sock, MSG_331); // Username okay, necesitamos password
}

void handle_PASS(const char *args) {

  ftp_session_t *sess = session_get();

  if (sess->current_user[0] == '\0') {
    safe_dprintf(sess->control_sock, MSG_503); // Mala secuencias de comandos
    return;
  }

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501); // Error de sintaxis en el parametro
    return;
  }

  if (check_credentials(sess->current_user, (char *)args) == 0) {
    sess->logged_in = 1;
    safe_dprintf(sess->control_sock, MSG_230); // User loggeado
  } else {
    safe_dprintf(sess->control_sock, MSG_530); // User no loggeado
    sess->current_user[0] = '\0'; // Restablecer usuario en caso de inicio de sesión fallido
    sess->logged_in = 0;
  }
}

void handle_QUIT(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args; // no utilizado

  safe_dprintf(sess->control_sock, MSG_221); // 221 chau chau.
  sess->current_user[0] = '\0'; // Cerrar sesion
  close_fd(sess->control_sock, "client socket"); // Cerrar socket
  sess->control_sock = -1;
}

void handle_SYST(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args; // no utilizado

  safe_dprintf(sess->control_sock, MSG_215); // 215 <system type>
}

void handle_TYPE(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  // Placeholder
}

void handle_PORT(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  // Placeholder
}

void handle_RETR(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  // Placeholder
}

void handle_STOR(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  // Placeholder
}

void handle_NOOP(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  // Placeholder
}
