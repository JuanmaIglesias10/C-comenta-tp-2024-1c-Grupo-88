#ifndef DICCIONARIO_KERNEL_H_
#define DICCIONARIO_KERNEL_H_

#include <utils.h>
#include <commons/collections/queue.h>
#include <readline/history.h>
#include <readline/readline.h>


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
	NULO,
	NEW,
	READY,
	EXEC,
	BLOCKED,
	FINISHED
} t_estado_proceso;

typedef struct{
	t_cde* cde;
	t_estado_proceso estado;
	// char* path;
	// int prioridad;
	// t_list* archivos_abiertos;
	// t_list* archivos_solicitados;
	// t_list* recursos_asignados;
	// t_list* recursos_solicitados;
	// bool flag_clock;
	// bool fin_q;
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


/* VARIABLES GLOBALES */
extern int pid_a_asignar;

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
extern t_queue* colaBLOCKED;
extern t_queue* colaFINISHED;

//Semaforos

extern pthread_mutex_t mutex_new;
extern pthread_mutex_t mutex_procesos_globales;
extern pthread_mutex_t mutex_ready;
extern pthread_mutex_t mutex_exec;
extern pthread_mutex_t mutex_pcb_en_ejecucion;


extern sem_t procesos_NEW;
extern sem_t cpu_libre;
extern sem_t procesos_en_ready;
extern sem_t aviso_exec;

#endif 

