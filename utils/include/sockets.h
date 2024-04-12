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



void chequearErrores(char* tipoError, int status);
int iniciar_servidor(char* puerto, t_log*);
int esperar_cliente(int, t_log*);
int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);

//Hechas por nosotros para conectarse y escuchar
int server_escuchar(t_log *logger, char *puerto);
void conectarse(t_config *config, char *ip, char *puerto, char *nombreDelModulo); 

#endif