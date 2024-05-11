
#include "diccionario_kernel.h"

t_log* logger_kernel;
t_config* config;
t_config_kernel config_kernel;

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
t_queue* colaBLOCKED;
t_queue* colaFINISHED;

int pid_a_asignar;

//Semaforos
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_procesos_globales;
pthread_mutex_t mutex_exec;

sem_t procesos_NEW;
sem_t cpu_libre;
sem_t procesos_en_ready;
