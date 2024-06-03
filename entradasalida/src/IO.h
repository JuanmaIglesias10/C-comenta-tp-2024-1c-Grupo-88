#ifndef IO_H_
#define IO_H_

#include "atender_IO.h"
#include "utils.h"
#include "diccionario_IO.h"

void inicializar_IO(char* nombre_interfaz, char* path_archivo_config);
void inicializar_config_IO_GENERICA();
void inicializar_config_IO_STDIN();
void inicializar_config_IO_STDOUT();
void inicializar_config_IO_DIALFS();
void inicializar_conexiones();
void IO_Generica();
#endif /* IO_H_ */