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




int iniciar_servidor(char* puerto, t_log* logger);
int esperar_cliente(int, t_log*);
int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);

#endif