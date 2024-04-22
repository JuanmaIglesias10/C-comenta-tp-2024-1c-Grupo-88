#ifndef KERNEL_H
#define KERNEL_H

#include <../../utils/include/utils.h>
#include "atender.h"


extern t_log* logger_kernel;
extern t_config* config_kernel;
extern int fd_memoria;
extern int fd_cpu_dis;
extern int fd_cpu_int;
extern int fd_kernel;
void inicializarKernel();


#endif