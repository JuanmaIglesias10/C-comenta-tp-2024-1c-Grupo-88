#ifndef SOCKETS_H_
#define SOCKETS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<string.h>
#include<signal.h>

#include "utils.h"


int iniciar_servidor(char* puerto, t_log* logger);
int esperar_cliente(int, t_log*);
int crear_conexion(char* ip, char* puerto);
int handshake_cliente(int socket_servidor);
int handshake_servidor(int socket_cliente);
void liberar_conexion(int socket_cliente);

#endif