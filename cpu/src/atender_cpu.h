#ifndef ATENDER_CPU_H_
#define ATENDER_CPU_H_

#include "diccionario_cpu.h"
#include "cpu.h"
#include "utils.h"

void* atender_memoria();
void* atender_kernel_dis();
void* atender_kernel_int();

#endif /* ATENDER_H */