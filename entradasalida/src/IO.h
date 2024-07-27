#ifndef IO_H_
#define IO_H_

#include "utils.h"
#include "diccionario_IO.h"
#include "atender_IO_generica_std.h"
#include "atender_IO_dialFS.h"

#include "readline/readline.h"

void seleccionar_tipo_interfaz(char* nombre_interfaz, char* nombre_arch_config);

void inicializar_IO_GENERICA(char* nombre_interfaz);
void inicializar_IO_STDIN(char* nombre_interfaz);
void inicializar_IO_STDOUT(char* nombre_interfaz);
void inicializar_IO_DIALFS(char* nombre_interfaz);
void enviar_info_kernel(char* nombre_interfaz, char* tipo);

#endif /* IO_H_ */