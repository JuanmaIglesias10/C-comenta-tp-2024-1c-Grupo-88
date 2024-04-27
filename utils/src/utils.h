#ifndef UTILS_H_
#define UTILS_H_

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
//#include <pthread.h>
//#include <stdint.h> // para uint8 y uint32

// nuestros headers
#include "protocolo.h"
#include "sockets.h"

void asesinarBuffer(t_buffer buffer);

//Hechas por nosotros para conectarse y escuchar
void* server_escuchar(t_log* logger, int fd_cliente, char* nombreCliente); // memoria.c
int conectarse(t_config* config, char* ip, char* puerto, char* nombreDelModulo, t_log*); // kernel.c , cpu.c , IO.c
t_config* iniciar_config(char* nombreArchivoconfig); // kernel.c , cpu.c , IO.c
t_log* iniciar_logger(char* rutaLog, char* nombreProceso , t_log_level level); // memoria.c , kernel.c , cpu.c , IO.c

char* obtenerNombreServer(char* keyPuerto); // TODO: no lo usa nadie

#endif /* UTILS_H_ */