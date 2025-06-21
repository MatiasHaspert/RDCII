#include "signals.h"


int server_socket = -1;

// --- Manejador para SIGINT (Ctrl+C) ---
static void handle_sigint(int sig) {
  (void)sig;
  static volatile sig_atomic_t in_handler = 0;

  if (in_handler) {
    fprintf(stderr, "¡El manejador de SIGINT fue reentrado!\n");
    return; // Evita ejecutar el manejador dos veces al mismo tiempo
  }
  in_handler = 1;

  static int sigint_count = 0;
  fprintf(stderr, "Manejador de SIGINT invocado (conteo = %d) en PID %d\n", ++sigint_count, getpid());

  printf("[+] SIGINT recibido. Cerrando...\n");
  fflush(stdout);

  // Cerrar el socket de escucha
  if (server_socket >= 0) {
    close_fd(server_socket, "listen socket");
    server_socket = -1;
  }

  // Bloquear SIGINT mientras se realiza el cierre
  // Evita problemas cuando llegan múltiples señales
  sigset_t blockset, oldset;
  sigemptyset(&blockset);
  sigaddset(&blockset, SIGINT);
  if (sigprocmask(SIG_BLOCK, &blockset, &oldset) < 0) {
    perror("sigprocmask");
  }

  // Restaurar la máscara de señales previa (opcional ya que vamos a salir)
  sigprocmask(SIG_SETMASK, &oldset, NULL);

  exit(EXIT_SUCCESS);
}

// --- Manejador para SIGTERM ---
static void handle_sigterm(int sig) {
  (void)sig;

  static volatile sig_atomic_t in_handler = 0;
  if (in_handler) {
    fprintf(stderr, "¡El manejador de SIGTERM fue reentrado!\n");
    return;
  }
  in_handler = 1;

  fprintf(stderr, "[+] SIGTERM recibido. Cerrando (PID %d)...\n", getpid());

  // Cerrar el socket de escucha si está abierto
  if (server_socket >= 0) {
    close_fd(server_socket, "listen socket");
    server_socket = -1;
  }

  exit(EXIT_SUCCESS);
}

// --- Configura los manejadores de señales ---
void setup_signals(void) {
  struct sigaction sa;

  printf("[DEBUG] Configurando manejadores de señales en el PID %d\n", getpid());

  // Configura SIGINT y SIGTERM para el proceso padre

  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGINT);  // Bloquear SIGINT mientras corre el manejador

  sa.sa_flags = SA_RESTART;        // Reiniciar syscalls interrumpidas

  sa.sa_handler = handle_sigint;

  // Instala manejador para SIGINT
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    perror("sigaction SIGINT");
    exit(EXIT_FAILURE);
  }
  printf("[DEBUG] Manejador de SIGINT instalado en PID %d\n", getpid());

  // Instala manejador para SIGTERM, con misma máscara y flags, pero distinto handler
  sa.sa_handler = handle_sigterm;

  if (sigaction(SIGTERM, &sa, NULL) == -1) {
    perror("sigaction SIGTERM");
    exit(EXIT_FAILURE);
  }
}

// --- Restaura el comportamiento por defecto de señales (usado por hijos antes de exec o salida limpia) ---
void reset_signals(void) {
  struct sigaction sa;

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = SIG_DFL;

  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
}
