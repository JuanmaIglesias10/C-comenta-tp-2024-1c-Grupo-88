#include "diccionario_memoria.h"

// Logger Y Config
t_log* logger_memoria;
t_config* config;
t_config_memoria config_memoria;

// Files Descriptors 
int fd_memoria;
int fd_cpu;
int fd_kernel;
int fd_IO;

//Listas
t_list* listaProcesos;

//Semaforos
pthread_mutex_t mutex_lista_procesos;

