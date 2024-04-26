#ifndef SOCKETS_H_
#define SOCKETS_H_

#include <utils.h>

int iniciar_servidor(char* puerto, t_log* logger);
int esperar_cliente(int, t_log*, char*);
int crear_conexion(char* ip, char* puerto);
int handshake_cliente(int socket_servidor);
int handshake_servidor(int socket_cliente);
void liberar_conexion(int socket_cliente);

#endif