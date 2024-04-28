
#include "diccionario_kernel.h"

t_log* logger_kernel;
t_config* config;
t_config_kernel config_kernel;

int fd_memoria;
int fd_cpu_dis;
int fd_cpu_int;
int fd_kernel;
int fd_IO;

// Colas
t_queue* colaNew;
t_queue* colaReady;
t_queue* colaBloqueados;
t_queue* colaFinalizados;

int pid_a_asignar;
