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

#define PUERTO "4445"

extern t_log* logger;

void chequearErrores(char* tipoError, int status);
int iniciar_servidor(void);
int esperar_cliente(int);
int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);

#endif