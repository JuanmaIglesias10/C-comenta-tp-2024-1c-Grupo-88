#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> // close
#include <sys/socket.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h> // para uint8 y uint32


typedef enum{
	INICIAR_PROCESO_SOLICITUD,
	INICIAR_PROCESO_OK,
	INICIAR_PROCESO_ERROR,
	PAGE_FAULT_SOLICITUD,
	PAGE_FAULT_OK,
	FINALIZAR_PROCESO_SOLICITUD,
	FINALIZAR_PROCESO_OK
}mensajeKernelMem;

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
} registros_cpu;

// conexiones
void* server_escuchar(t_log* logger, int fd_cliente, char* nombreCliente); 
int conectarse(char* IP, int puerto, char* nombreConexion, t_log* logger);

// inicializar
t_config* iniciar_config(char* nombreArchivoconfig);
t_log* iniciar_logger(char* rutaLog, char* nombreProceso , t_log_level level);

// Nos sirve para enviar un codigo de operacion (ENUM) hacia otro modulo.
void enviar_codOp(int fd_cliente , uint8_t codOp);

#endif /* UTILS_H_ */