#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h> // close
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdint.h> // para uint8 y uint32
#include <pthread.h>
#include <semaphore.h>

//Commons
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>


typedef enum{
	INICIAR_PROCESO_SOLICITUD,
	INICIAR_PROCESO_OK,
	INICIAR_PROCESO_ERROR,
	PAGE_FAULT_SOLICITUD,
	PAGE_FAULT_OK,
	FINALIZAR_PROCESO_SOLICITUD,
	FINALIZAR_PROCESO_OK
}mensajeKernelMem;

typedef enum{
	EJECUTAR_PROCESO,
	INTERRUPT,
	DESALOJO,
	CDE,
	ALGORITMO_PLANIFICACION,
	HAY_PAGE_FAULT,
	DIRECCION_FISICA_OK
}mensajeKernelCpu;




typedef struct{
	uint32_t PC; ////indica la próxima instrucción a ejecutar
    uint8_t AX;
    uint8_t BX;
    uint8_t CX;
    uint8_t DX;
    uint32_t EAX;
    uint32_t EBX;
    uint32_t ECX;
    uint32_t EDX;
    uint32_t SI; //Contiene la dirección lógica de memoria de origen desde donde se va a copiar un string.
    uint32_t DI; //Contiene la dirección lógica de memoria de destino a donde se va a copiar un string.
} t_registros;

typedef struct{
	uint32_t pid;
	t_registros* registros;
}t_cde;

typedef enum{
	SET,
	MOV_IN,
	MOV_OUT,
    SUM, 
    SUB, 
    JNZ, 
    RESIZE, 
    COPY_STRING, 
	WAIT, 
	SIGNAL, 
	IO_GEN_SLEEP, 
	IO_STDIN_READ, 
	IO_STDOUT_WRITE, 
	IO_FS_CREATE, 
	IO_FS_DELETE, 
	IO_FS_TRUNCATE, 
	IO_FS_WRITE, 
	IO_FS_READ,
    EXIT 
}t_codigo_instruccion;

typedef struct{
	t_codigo_instruccion codigo;
	char* par1;
	char* par2;
	char* par3;
	char* par4;
	char* par5;
}t_instruccion;

// nuestros headers
#include "protocolo.h"
#include "sockets.h"

// conexiones
void* server_escuchar(t_log* logger, int fd_cliente, char* nombreCliente); 
int conectarse(char* IP, int puerto, char* nombreConexion, t_log* logger);

// inicializar
t_config* iniciar_config(char* nombreArchivoconfig);
t_log* iniciar_logger(char* rutaLog, char* nombreProceso , t_log_level level);

// Nos sirve para enviar un codigo de operacion (ENUM) hacia otro modulo.
void enviar_codOp(int fd_cliente , uint8_t codOp);

#endif /* UTILS_H_ */