#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <commons/log.h> // SI
#include <sys/socket.h> // socket() - SI
#include <string.h> // memset() - SI
#include <netdb.h> // uint32_t, uint8_t, getaddrinfo() - SI
#include <unistd.h> // close()
#include <stdlib.h>

// nuestros headers
// ninguno

int iniciar_servidor(char* puerto, t_log* logger); // lo usan los modulos
int esperar_cliente(int, t_log*, char*); // lo usan los modulos
void liberar_conexion(int socket_cliente); // lo usan los modulos

void chequearErrores(char* tipoError, int status); // sockets.c
int crear_conexion(char* ip, char* puerto); // utils.c

int handshake_cliente(int socket_servidor); // TODO: no lo usa nadie
int handshake_servidor(int socket_cliente); // TODO: no lo usa nadie


#endif /* SOCKETS_H_ */