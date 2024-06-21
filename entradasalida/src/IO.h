#ifndef IO_H_
#define IO_H_

#include "atender_IO.h"
#include "utils.h"
#include "diccionario_IO.h"
#include "readline/readline.h"

void inicializar_IO(char* nombre_interfaz, char* path_archivo_config);
void inicializar_config_IO_GENERICA();
void inicializar_config_IO_STDIN();
void inicializar_config_IO_STDOUT();
void inicializar_config_IO_DIALFS();
void inicializar_IO_generica();
void inicializar_IO_STDIN();
void inicializar_IO_STDOUT();
void inicializar_IO_DIALFS();
void inicializar_conexiones();
void enviar_info_kernel(char* nombre_interfaz, char* tipo);
void exec_IO_STDIN_READ();
#endif /* IO_H_ */