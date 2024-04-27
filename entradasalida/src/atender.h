#ifndef ATENDER_H_
#define ATENDER_H_


#include <IO.h>

void* atender_memoria(int cliente_fd);
void* atender_kernel(int cliente_fd);


#endif