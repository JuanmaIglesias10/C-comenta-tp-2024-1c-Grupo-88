#ifndef IO_H
#define IO_H
#include "atender.h"

#include <../../utils/include/utils.h>


extern t_log* logger_IO;
extern t_config* config_IO;
extern int fd_kernel;
extern int fd_memoria;


void inicializar_IO();


#endif