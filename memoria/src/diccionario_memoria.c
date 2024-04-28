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

extern int fd_memoria;
extern int fd_cpu;
extern int fd_kernel;
extern int fd_IO;