#ifndef MEMORIA_H
#define MEMORIA_H

#include <utils.h>
#include <atender.h>
// Declare global variables as extern
extern t_log* logger_memoria;
extern t_config* config_memoria;
extern int fd_memoria;

// Function declarations
void inicializar_memoria();

#endif

