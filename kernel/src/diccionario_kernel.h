#ifndef DICCIONARIO_KERNEL_H_
#define DICCIONARIO_KERNEL_H_

#include <utils.h>
#include <commons/collections/queue.h>
#include <readline/history.h>
#include <readline/readline.h>
#include "commons/temporal.h"

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
	t_list* recursos;
	t_list* instancias_recursos;
	int grado_multiprogramacion;
} t_config_kernel;

typedef enum {
	NULO,
	NEW,
	READY,
	EXEC,
	BLOCKED,
	FINISHED
} t_estado_proceso;

typedef struct{
    char* nombre;
    int instancias;
	t_list* procesos_bloqueados;
    sem_t sem_recurso;
}t_recurso;

typedef struct{
	t_cde* cde;
	t_estado_proceso estado;
	uint32_t quantum;
	// char* path;
	// t_list* archivos_abiertos;
	// t_list* archivos_solicitados;
	t_list* recursos_asignados;
	t_list* recursos_solicitados;
	bool flag_clock;
	bool fin_q;
}t_pcb;

typedef enum{
    EJECUTAR_SCRIPT,
	INICIAR_PROCESO,
	FINALIZAR_PROCESO,
	INICIAR_PLANIFICACION,
	DETENER_PLANIFICACION,
	MULTIPROGRAMACION,
	PROCESO_ESTADO,
	E_PARAMETROS,
	MENSAJE_VACIO,
	BASURA
} t_codigo_operacion;

typedef struct {
	t_codigo_operacion codigo_operacion;
	char* par1;
} t_script;

typedef struct {
    char* nombre;
    char* tipo;
    int fd;
} t_interfaz;


/* VARIABLES GLOBALES */
extern int pid_a_asignar;
extern int planificacion_detenida;

// Logger Y Config
extern t_log* logger_kernel;
extern t_config* config;
extern t_config_kernel config_kernel;

// PCB
extern t_pcb* pcb_ejecutando;

// Files Descriptors 
extern int fd_memoria;
extern int fd_cpu_dis;
extern int fd_cpu_int;
extern int fd_kernel;
extern int fd_IO;

//Listas
extern t_list* procesos_globales;

// Colas
extern t_queue* colaNEW;
extern t_queue* colaREADY;
extern t_queue* colaREADYMAS;
extern t_queue* colaBLOCKED;
extern t_queue* colaFINISHED;

extern t_queue* colaGenerica;
extern t_queue* colaSTDIN;
extern t_queue* colaSTDOUT;
extern t_queue* colaDIALFS;

//Semaforos

extern pthread_mutex_t mutex_new;
extern pthread_mutex_t mutex_procesos_globales;
extern pthread_mutex_t mutex_ready;
extern pthread_mutex_t mutex_exec;
extern pthread_mutex_t mutex_block;
extern pthread_mutex_t mutex_pcb_en_ejecucion;
extern pthread_mutex_t mutex_finalizados;
extern pthread_mutex_t mutex_colasIO;
extern pthread_mutex_t mutex_colaGEN;

extern sem_t procesos_NEW;
extern sem_t cpu_libre;
extern sem_t procesos_en_ready;
extern sem_t aviso_exec;
extern sem_t bin_recibir_cde;
extern sem_t procesos_en_exit;
extern sem_t sem_iniciar_quantum;
extern sem_t sem_reiniciar_quantum;
extern sem_t procesos_en_blocked;
extern sem_t cont_exec;
extern sem_t sem_timer;
extern sem_t grado_de_multiprogramacion;
extern sem_t pausar_new_a_ready;
extern sem_t pausar_ready_a_exec;
extern sem_t pausar_exec_a_finalizado;
extern sem_t pausar_exec_a_ready;
extern sem_t pausar_exec_a_blocked;
extern sem_t pausar_blocked_a_ready;
extern sem_t sem_colaGEN;


// Temporal
extern t_temporal* timer;
extern uint64_t ms_transcurridos;


#endif 

