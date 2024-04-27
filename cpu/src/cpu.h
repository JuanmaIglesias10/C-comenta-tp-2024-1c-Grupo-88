#ifndef CPU_H
#define CPU_H

#include "atender.h"
#include <utils.h>

extern t_log* logger_cpu;
extern t_config* config_cpu;

extern int fd_memoria;
extern int fd_cpuDispatch;
extern int fd_cpuInterrupt;


void inicializarCpu();


#endif