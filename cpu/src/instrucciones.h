#include "diccionario_cpu.h"
#include "utils.h"


char* obtener_nombre_instruccion(t_instruccion* instruccion);

// FUNCIONALIDAD DE LAS INSTRUCCIONES
void ejecutar_set(char* registro, void* valor_recibido);