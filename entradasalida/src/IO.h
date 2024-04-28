#ifndef IO_H_
#define IO_H_

#include <pthread.h>

#include "atender.h"
#include "utils.h"
#include "diccionario_IO.h"

void inicializar_IO();
void inicializar_config_IO();
void inicializar_conexiones();

#endif /* IO_H_ */