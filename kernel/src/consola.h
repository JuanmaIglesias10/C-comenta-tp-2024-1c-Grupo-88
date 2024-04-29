
#include "diccionario_kernel.h"
#include "kernel.h"

void iniciar_consola();
void mostrar_opciones_consola();
void atender_consola(char* input);
t_codigo_operacion get_codigo_operacion(char* comando, int cant_par);