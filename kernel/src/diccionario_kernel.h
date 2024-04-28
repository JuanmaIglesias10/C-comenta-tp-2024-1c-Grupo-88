#ifndef DICCIONARIO_KERNEL_H_
#define DICCIONARIO_KERNEL_H_

#include <commons/log.h>
#include <commons/config.h> 
#include <utils.h>
#include <commons/collections/queue.h>

//STRUCT'S && ENUM
typedef struct{
    int puerto_escucha;
	char* ip_memoria;
    int puerto_memoria;
	char* ip_cpu;
	int puerto_cpu_dispatch;
	int puerto_cpu_interrupt;
	char* algoritmo_planificacion;
	int quantum;
	// t_list* recursos;
	// t_list* instancias_recursos;
	int grado_multiprogramacion;
} t_config_kernel;

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCKED,
	EXIT
} estado_proceso;

typedef struct {
	uint32_t pid;
	uint32_t program_counter;
	uint32_t quantum;
	registros_cpu registros;
	estado_proceso estado;	
} t_pcb;

/* VARIABLES GLOBALES */

// Logger Y Config
extern t_log* logger_kernel;
extern t_config* config;
extern t_config_kernel config_kernel;

// Files Descriptors 
extern int fd_memoria;
extern int fd_cpu_dis;
extern int fd_cpu_int;
extern int fd_kernel;

// Colas
extern t_queue* colaNew;
extern t_queue* colaReady;
extern t_queue* colaBloqueados;
extern t_queue* colaFinalizados;

#endif 
