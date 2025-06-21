#include "server.h"

extern int server_socket;

int server_init(const char *ip, int port) {
  struct sockaddr_in server_addr;

  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd < 0) {
    fprintf(stderr, "Error al crear el socket");
    perror(NULL);
    return -1;
  }

  // Evitar el problema de reutilizar el socket después de forzar el cierre.
  const int opt = 1;
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    fprintf(stderr, "Error al reutilizar el socket");
    perror(NULL);
    close(listen_fd);
    return -1;
  }

#ifdef SO_REUSEPORT
  if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    fprintf(stderr, "IP inválida: %s\n", ip);
    perror(NULL);
    close(listen_fd);
    return -1;
  }
#endif

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
    fprintf(stderr, "IP inválida: %s\n", ip);
    close(listen_fd);
    return -1;
  }

  if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    fprintf(stderr, "Bind fallo");
    perror(NULL);
    close(listen_fd);
    return -1;
  }

  char ip_buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &server_addr.sin_addr, ip_buf, sizeof(ip_buf));
  printf("Server escuchando en %s:%d\n", ip_buf, port);

  if (listen(listen_fd, SOMAXCONN) < 0) {
    fprintf(stderr, "Listen fallo");
    perror(NULL);
    close(listen_fd);
    return -1;
  }

  server_socket = listen_fd;
  return listen_fd;
}

int server_accept(int listen_fd, struct sockaddr_in *client_addr) {

  // client_addr puede ser NULL si el llamador no necesita información del cliente
  socklen_t addrlen = sizeof(*client_addr);
  int new_socket = accept(listen_fd, (struct sockaddr *)client_addr, &addrlen);

  // EINTR para evitar errores por reentrada de señal
  // https://stackoverflow.com/questions/41474299/checking-if-errno-eintr-what-does-it-mean
  if (new_socket < 0 && errno != EINTR) {
    fprintf(stderr, "Accept failed: ");
    perror(NULL);
    return -1;
  }

  return new_socket;
}

void server_loop(int socket) {

  // Establecer el socket de sesión con el cliente
  session_init(socket);

  // Enviar mensaje inicial de bienvenida al FTP
  if (welcome(current_sess) < 0)
    return;

  while(1) {
    // Obtener comando del canal de control
    if (getexe_command(current_sess) < 0)
      break;
  }

  session_cleanup();
}
