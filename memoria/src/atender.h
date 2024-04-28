#ifndef ATENDER_H_
#define ATENDER_H_

#include "commons/log.h"
#include "commons/config.h"

#include "utils.h"
#include "memoria.h"
#include "diccionario_memoria.h"

void* atender_cpu( int cliente_fd);
void* atender_IO( int cliente_fd);
void* atender_kernel( int cliente_fd);

#endif /* ATENDER_H_ */
