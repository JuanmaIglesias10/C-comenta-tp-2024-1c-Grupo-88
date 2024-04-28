#include "diccionario_cpu.h"

// Variables globales CPU
t_log* logger_cpu;
t_config* config;
t_config_cpu config_cpu;

// File Descriptors 
int fd_memoria;
int fd_cpu_dis;
int fd_cpu_int;
int fd_kernel_dis;
int fd_kernel_int;

// fin variables globales CPU