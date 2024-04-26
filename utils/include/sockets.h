#ifndef SOCKETS_H
#define SOCKETS_H

//#include <utils.h>
#include <commons/log.h> // SI
#include <sys/socket.h> // SI
#include <string.h> // memset -- SI
#include <netdb.h> // uint32_t, uint8_t, getaddrinfo(), 

#include <utils.h>

int iniciar_servidor(char* puerto, t_log* logger);
int esperar_cliente(int, t_log*, char*);
int crear_conexion(char* ip, char* puerto);
int handshake_cliente(int socket_servidor);
int handshake_servidor(int socket_cliente);
void liberar_conexion(int socket_cliente);

#endif /* SOCKETS_H */