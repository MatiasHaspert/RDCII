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

  if (!args || strlen(args) != 1) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  if (args[0] == 'I') {
    safe_dprintf(sess->control_sock, MSG_204); // Modo binario
  } else if(args[0] == 'A'){
    safe_dprintf(sess->control_sock, MSG_205); // Modo ASCII
  } else {
    safe_dprintf(sess->control_sock, MSG_504); // Comando no implementado para ese parámetro
  }

}

void handle_PORT(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  int h1, h2, h3, h4, p1, p2;
  if (sscanf(args, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2) != 6) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  // Limpiar y configurar dirección IP y puerto para la conexión de datos
  memset(&sess->data_addr, 0, sizeof(sess->data_addr));
  sess->data_addr.sin_family = AF_INET;
  sess->data_addr.sin_port = htons(p1 * 256 + p2);

  char ip_str[INET_ADDRSTRLEN];
  snprintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d", h1, h2, h3, h4);
  if (inet_pton(AF_INET, ip_str, &sess->data_addr.sin_addr) <= 0) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  safe_dprintf(sess->control_sock, MSG_203); // Comando PORT OK
}

void handle_RETR(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  if (!sess->logged_in) {
    safe_dprintf(sess->control_sock, MSG_530);
    return;
  }

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  safe_dprintf(sess->control_sock, MSG_150); // OK, abriendo conexión de datos

  // Intentar enviar el archivo
  if (dtp_send_file(sess, args) == 0) {
    safe_dprintf(sess->control_sock, MSG_226); // Transferencia OK
  } else {
    safe_dprintf(sess->control_sock, MSG_550, "No se pudo enviar el archivo");
  }
}

void handle_STOR(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  if (!sess->logged_in) {
    safe_dprintf(sess->control_sock, MSG_530);
    return;
  }

  if (!args || strlen(args) == 0) {
    safe_dprintf(sess->control_sock, MSG_501);
    return;
  }

  safe_dprintf(sess->control_sock, MSG_150); // OK, listo para recibir
 
  if (dtp_receive_file(sess, args) == 0) {
    safe_dprintf(sess->control_sock, MSG_226); // Transferencia OK
  } else {
    safe_dprintf(sess->control_sock, MSG_550, "No se pudo guardar el archivo");
  }
}

void handle_NOOP(const char *args) {
  ftp_session_t *sess = session_get();
  (void)args;
  (void)sess;

  safe_dprintf(sess->control_sock, MSG_200); // OK
}
