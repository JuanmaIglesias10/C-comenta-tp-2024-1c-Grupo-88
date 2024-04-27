#ifndef DICCIONARIO_KERNEL_H_
#define DICCIONARIO_KERNEL_H_

#include <commons/log.h> // SI
#include <commons/config.h> // SI
#include <utils.h>

extern t_log* logger_kernel;
extern t_config* config_kernel;
extern int fd_memoria;
extern int fd_cpu_dis;
extern int fd_cpu_int;
extern int fd_kernel;

#endif
