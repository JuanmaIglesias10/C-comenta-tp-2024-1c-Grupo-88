#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> // close
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h> // SI
#include <commons/config.h> // SI
#include <commons/collections/list.h>
#include <pthread.h>
//#include <stdint.h> // para uint8 y uint32

// headers nuestros
#include <sockets.h>
//#include <buffer.h>
#include <protocolo.h>



void chequearErrores(char* tipoError, int status);

//Hechas por nosotros para conectarse y escuchar
void* server_escuchar(t_log *logger, int fd_cliente, char* nombreCliente);
int conectarse(t_config *config, char *ip, char *puerto, char *nombreDelModulo, t_log*); 
t_config *iniciar_config(char* nombreArchivoconfig);
t_log* iniciar_logger(char* rutaLog, char* nombreProceso , t_log_level level);
char* obtenerNombreServer(char* keyPuerto);

#endif /* UTILS_H */