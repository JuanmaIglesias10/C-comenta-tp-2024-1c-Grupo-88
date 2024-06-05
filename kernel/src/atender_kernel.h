#ifndef ATENDER_KERNEL_H_
#define ATENDER_KERNEL_H_

#include <kernel.h>

void* atender_memoria();
void* atender_IO(void* arg);
void* atender_cpu_int();

void* aceptar_conexiones_IO(void* arg);
// void* atender_cpu_dis();

#endif