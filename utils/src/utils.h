#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> // close
#include <sys/socket.h>
#include <netdb.h>
#include <commons/log.h> // SI
#include <commons/config.h> // SI
#include <commons/collections/list.h>
#include <commons/string.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h> // para uint8 y uint32

// nuestros headers
#include "protocolo.h"
#include "sockets.h"

typedef struct{
    uint32_t pc;
    uint8_t ax;
    uint8_t bx;
    uint8_t cx;
    uint8_t dx;
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t si;
    uint32_t di;
}registros_cpu;

//Hechas por nosotros para conectarse y escuchar
void* server_escuchar(t_log* logger, int fd_cliente, char* nombreCliente); // memoria.c
int conectarse(char* IP, int puerto, char* nombreConexion, t_log* logger); // kernel.c , cpu.c , IO.c
t_config* iniciar_config(char* nombreArchivoconfig); // kernel.c , cpu.c , IO.c
t_log* iniciar_logger(char* rutaLog, char* nombreProceso , t_log_level level); // memoria.c , kernel.c , cpu.c , IO.c

#endif /* UTILS_H_ */