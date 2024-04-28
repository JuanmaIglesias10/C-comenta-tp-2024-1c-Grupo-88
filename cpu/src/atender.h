#ifndef ATENDER_H_
#define ATENDER_H_

#include "diccionario_cpu.h"
#include "utils.h"

void* atender_memoria(int cliente_fd);
void* atender_kernel_dis(int cliente_fd);
void* atender_kernel_int(int cliente_fd);

#endif /* ATENDER_H */