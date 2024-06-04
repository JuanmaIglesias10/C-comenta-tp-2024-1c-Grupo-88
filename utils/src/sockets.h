#ifndef SOCKETS_H_
#define SOCKETS_H_

#include "utils.h"
#include <sys/select.h>


int iniciar_servidor(int puerto, t_log* logger); 

int esperar_cliente(int, t_log*, char*); 

void liberar_conexion(int socket_cliente); 

void chequearErrores(char* tipoError, int status); // sockets.c

int crear_conexion(char* ip, char* puerto); // utils.c

int handshake_cliente(int socket_servidor); // TODO: no lo usa nadie
int handshake_servidor(int socket_cliente); // TODO: no lo usa nadie

int esperar_cliente_timeout(int socket_servidor, t_log* logger, char* nombre_cliente, int timeout);

#endif /* SOCKETS_H_ */