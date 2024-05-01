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
} estado_proceso;

typedef struct {
	char* path;
	uint32_t pid;
	uint32_t program_counter;
	uint32_t quantum;
	registros_cpu* registros;
	estado_proceso estado;	
} t_pcb;

/*
printf("---MENU---\n");
	printf("1. EJECUTAR_SCRIPT\n"); //PATH => Archivo de comandos, se encuentra en maquina donde corra el kernel 1
	printf("2. INICIAR_PROCESO\n"); //PATH => Archivo pseudocodigo memoria 1
	printf("3. FINALIZAR_PROCESO\n"); //PID 1
	printf("4. INICIAR_PLANIFICACION\n"); //0
	printf("5. DETENER_PLANIFICACION\n"); //0
	printf("6. MULTIPROGRAMACION\n"); //VALOR 1
	printf("7. PROCESO_ESTADO\n"); // 0
	printf("8. EXIT\n"); //0
*/
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

#endif 
