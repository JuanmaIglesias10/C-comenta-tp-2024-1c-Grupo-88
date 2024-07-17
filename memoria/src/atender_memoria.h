#ifndef ATENDER_MEMORIA_H_
#define ATENDER_MEMORIA_H_

#include "utils.h"
#include "memoria.h"
#include "diccionario_memoria.h"

void atender_cpu();
void* atender_IO(void* fd_IO_ptr);
void atender_kernel();


void* aceptar_conexiones_IO(void* arg);
#endif /* ATENDER_H_ */
