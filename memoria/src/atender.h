#ifndef ATENDER_H_
#define ATENDER_H_

#include "utils.h"
#include "memoria.h"
#include "diccionario_memoria.h"

void* atender_cpu();
void* atender_IO();
void* atender_kernel();

#endif /* ATENDER_H_ */
