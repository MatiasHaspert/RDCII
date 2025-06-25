#define _GNU_SOURCE
#include "serverdtp.h"

int check_credentials(char *user, char *pass) {
  FILE *file;
  char *path = PWDFILE, *line = NULL, cred[100];
  size_t len = 0;
  int found = -1;

  // crear la cadena de credenciales
  sprintf(cred, "%s:%s", user, pass);

  // comprobar si está presente en alguna línea ftpusers
  file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Error: no se pudo abrir el archivo de usuarios.\n");
    return -1;
  }

  while (getline(&line, &len, file) != -1) {
    strtok(line, "\n");
    if (strcmp(line, cred) == 0) {
      found = 0;
      break;
    }
  }

  fclose(file);
  if (line) free(line);
  return found;
}


int dtp_send_file(ftp_session_t *sess, const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror("fopen");
    return -1;
  }

  char buffer[1024];
  size_t bytes_read;
  ssize_t bytes_written;

  while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
    bytes_written = write(sess->data_sock, buffer, bytes_read);
    if (bytes_written < 0) {
      perror("write");
      fclose(fp);
      return -1;
    } else if ((size_t)bytes_written != bytes_read) {
      fprintf(stderr, "Error: no se escribieron todos los bytes\n");
      fclose(fp);
      return -1;
    }
  }

  if (ferror(fp)) {
    fprintf(stderr, "Error al leer el archivo\n");
    fclose(fp);
    return -1;
  }

  fclose(fp);
  return 0;
}

int dtp_receive_file(ftp_session_t *sess, const char *filename) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    perror("fopen");
    return -1;
  }

  char buffer[1024];
  ssize_t bytes;
  while ((bytes = read(sess->data_sock, buffer, sizeof(buffer))) > 0) {
    if (fwrite(buffer, 1, bytes, fp) != (size_t)bytes) {
      perror("fwrite");
      fclose(fp);
      return -1;
    }
  }

  if (bytes < 0) {
    perror("read");
    fclose(fp);
    return -1;
  }


  if (ferror(fp)) {
    fprintf(stderr, "Error en el archivo luego del cierre.\n");
    return -1;
  }

  fclose(fp);
  return 0;
}

int dtp_open_data_connection(ftp_session_t *sess) {
  sess->data_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sess->data_sock < 0) {
    perror("socket");
    return -1;
  }

  if (connect(sess->data_sock, (struct sockaddr *)&sess->data_addr, sizeof(sess->data_addr)) < 0) {
    perror("connect");
    close(sess->data_sock);
    sess->data_sock = -1;
    return -1;
  }

  return 0;
}
