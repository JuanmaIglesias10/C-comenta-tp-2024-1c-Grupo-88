#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "utils.h"
#include "atender.h"
#include <pthread.h>

// Declare global variables as extern
extern t_log* logger_memoria;
extern t_config* config_memoria;
extern int fd_memoria;

// Function declarations
void inicializar_memoria();

#endif /* MEMORIA_H_ */

