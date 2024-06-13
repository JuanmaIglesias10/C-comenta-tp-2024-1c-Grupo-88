
#include "diccionario_kernel.h"

t_log* logger_kernel;
t_config* config;
t_config_kernel config_kernel;

t_pcb* pcb_ejecutando;

int fd_memoria;
int fd_cpu_dis;
int fd_cpu_int;
int fd_kernel;
int fd_IO;

//Listas
t_list* procesos_globales;

// Colas
t_queue* colaNEW;
t_queue* colaREADY;
t_queue* colaREADYMAS;
t_queue* colaBLOCKED;
t_queue* colaFINISHED;

t_queue* colaGenerica;
t_queue* colaSTDIN;
t_queue* colaSTDOUT;
t_queue* colaDIALFS;

int pid_a_asignar;
int planificacion_detenida = 1;

//Semaforos
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_procesos_globales;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_exec;
pthread_mutex_t mutex_block;
pthread_mutex_t mutex_pcb_en_ejecucion;
pthread_mutex_t mutex_finalizados;
pthread_mutex_t mutex_colasIO;


sem_t procesos_NEW;
sem_t cpu_libre;
sem_t procesos_en_ready;
sem_t aviso_exec;
sem_t bin_recibir_cde;
sem_t procesos_en_exit;
sem_t procesos_en_blocked;
sem_t sem_iniciar_quantum;
sem_t sem_reiniciar_quantum;
sem_t cont_exec;
sem_t sem_timer;
sem_t grado_de_multiprogramacion;


//Temporal
t_temporal* timer;
uint64_t ms_transcurridos;
