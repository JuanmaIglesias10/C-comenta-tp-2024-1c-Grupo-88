#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "utils.h"
#include "atender_memoria.h"
#include "diccionario_memoria.h"


// Function declarations
void inicializar_memoria();
void inicializar_config();
void inicializar_conexiones();
void iniciar_proceso();

t_list* obtenerInstrucciones(char* rutaArchivo);
t_codigo_instruccion obtenerCodigoInstruccion(char* charParametro);
void obtenerParametrosInstruccion(int numParametro, t_instruccion* instrucciones, char* parametros);

#endif /* MEMORIA_H_ */

